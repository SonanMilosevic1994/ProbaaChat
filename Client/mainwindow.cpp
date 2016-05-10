#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "startdialog.h"
#include <qthread.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //Definisemo tipove adrese servera, port servera, kao i username
    serverAddr = new QString;
    serverPort = new int;
    userName = new QString;

    //kao pocetni dijalog postavaljamo start sa parametrima gore definisanim
    startDialog start(serverAddr, serverPort, userName,this);
    start.setModal(true);
    start.exec();

    

    ui->setupUi(this);

	
    //Definisemo sta se desava kada kliknemo na dugme,tj koju akciju vrsimo,u ovom slucaju se konektujemo na server
	connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(on_pushButton_clicked()));
		

	socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()), this, SLOT(clientConnected()));
    connect(socket,SIGNAL(readyRead()),this,SLOT(readyRead()));

    socket->connectToHost(*serverAddr,*serverPort);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete serverAddr;
    delete serverPort;
    delete userName;
    delete socket;
}


void MainWindow::clientConnected()
{

   //Metod je pozvan kada smo se uspesno konektovali
   for(int i=0;i<10000000;i++); //waste time for thread on server to setup
	
   connect(socket, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
   socket->write("REG\n");
   socket->flush();
   socket->waitForReadyRead();
   socket->readAll();
   socket->write(userName->toUtf8());
   socket->flush();
   socket->waitForReadyRead();
   socket->readAll();
   m_bUserNameSend = true;
   ui->textBrowser->setText("Successfully connected.\n");
}



void MainWindow::readyRead()
{
   //Obrada poruka i ispis na textBrowser
   QString tempString = socket->readAll();
   ui->textBrowser->append(tempString);
   ui->textBrowser->repaint();
}
void MainWindow::on_pushButton_clicked()
{
    //kada klikndemo na dugme saljemo poruku.
	QString temp, stuff;
	temp=ui->lineEdit->text();
	socket->write("MSG\n");
	socket->flush();

    //cekamo da podaci budu procitani sa sercera
    for(int i=0;i<10000000;i++);

    //pisemo na soket
	socket->write(temp.toUtf8());
	socket->flush();
	ui->lineEdit->clear();


}



void MainWindow::on_pushButton_2_clicked()
{
    //brisemo poruke koje smo ispisali
    ui->textBrowser->clear();
}
