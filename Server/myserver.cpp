#include "myserver.h"
using namespace::std;
QLinkedList<frameData> *data;
QLinkedList<threadFrame> *frameList;

int *sysFrame;

myserver::myserver(QObject *parent) :
    QTcpServer(parent)
{
}

myserver::~myserver()
{
	delete data;
	delete frameList;
}

void myserver::StartServer()
{
	timer = new QTimer;
    //tajmer koji sluzi za brisanje nezeljenih poruka
    timer->start(300000);

    //kada prodje pet minuta onda se poziva metod cleanUp
    connect(timer,SIGNAL(timeout()),this,SLOT(cleanUp()));
	lock = new QMutex;
	data = new QLinkedList<frameData>;
	frameList = new QLinkedList<threadFrame>;
	sysFrame = new int;
	*sysFrame = 0;

    if(!this->listen(QHostAddress::Any,8060))
    {
        qDebug() << "Could not start server";
    }
    else
    {
        qDebug() << "Listening on port 8060";
    }
}

void myserver::incomingConnection(int socketDescriptor)
{
    qDebug() << socketDescriptor << "Connecting ...";
    mythread *thread = new mythread(lock, sysFrame,socketDescriptor,data, frameList, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}

void myserver::cleanUp()
{
	//this is the cleanup slot. basically iterates through the list and everything that has a frame number before the most recent frame number is removed. This helps remove server load.
	QLinkedList<threadFrame>::iterator fit;
	QLinkedList<frameData>::iterator dit;
	int frame=*sysFrame;
	bool flag=true;
    //zakljucavamo thread da ne bi neki drugi mogao da ga prekine dok radi operacije
    lock->lock();

    //Deo koji nije jasan
	for(fit=frameList->begin();fit!=frameList->end();fit++)
	{
		if(fit->frame < frame)
		{
			frame = fit->frame;
		}
    }

begin: while(flag)
	   {
		   for(dit=data->begin();dit!=data->end();dit++)
		   {
			   flag = false;
			   if(dit->getFame() < frame)
			   {
				   data->erase(dit); //this invalidates the interator and needs to make the loop start all over.
				   flag = true;
				   goto begin;
			   }
		   }
	   }
	   lock->unlock();
}
