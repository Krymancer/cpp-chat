#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QTimer>
#include <QPushButton>
#include <QInputDialog>
#include <QLineEdit>

#include <string.h>
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

static MainWindow*  uiUpdateHandler;
static QVector<QString> messagesQeue;

static struct sockaddr_in serverAdress;
static int mySocket;
static int portno;
static pthread_t reciveMessageThread;
static char username[100];
static char ip[INET_ADDRSTRLEN];


static void sendMessage(){
    QString reciver = uiUpdateHandler->getReciver();
    QString subject = uiUpdateHandler->getSubject();
    QString message = uiUpdateHandler->getMessage();

    QString msg = "To: " + reciver + "\nSubject: " + subject + "\nMessage: " + message + "\n";
    QByteArray msgByte = msg.toLocal8Bit();
    const char* parsedMessage = msgByte.data();

    if (write(mySocket, parsedMessage, strlen(parsedMessage) ) < 0) {
        perror("Message not sent");
        exit(1);
    }

}

static void writeMessage(QString message){
    messagesQeue.push_back(message);
}

void sendMessagetoUI(){
     for(int i=0;i<messagesQeue.size();i++){
         uiUpdateHandler->addMessage(messagesQeue.takeFirst());
     }
}

bool itsToMe(QString message){
    QString me = "To: " + QString::fromUtf8(username);
    qInfo() << message << me;
    return me == message;
}

void *reciveMessage(void *socket) {
    int their_sock = *(static_cast<int*>(socket));
    char msg[500];
    int len;
    while ((len = static_cast<int>(recv(their_sock, msg, 500, 0)) > 0)) {
        QString message = QString::fromUtf8(msg);
        qInfo() << message;
        if(itsToMe(message.QString::split('\n')[0]))
        writeMessage(message);
        memset(msg, '\0', sizeof(msg));
    }
    return nullptr;
}


int main(int argc, char *argv[]){


    QApplication a(argc, argv);
    MainWindow w;
    uiUpdateHandler = &w;

    qInfo() << argc << argv[0];

    bool ok;
    QString text = QInputDialog::getText(nullptr,
            "Cpp-Chat", "Username:", QLineEdit::Normal,
            QString(), &ok,nullptr );
    if ( ok && !text.isEmpty() ) {
        QByteArray usernameByte = text.toLocal8Bit();
        const char* parsedUsername = usernameByte.data();
       strcpy(username,parsedUsername);
    } else {
        exit(1);
    }

    portno = 1010;
    mySocket = socket(AF_INET, SOCK_STREAM, 0);
    memset(serverAdress.sin_zero,'\0',sizeof(serverAdress.sin_zero));
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_port = htons(static_cast<uint16_t>(portno));
    serverAdress.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(mySocket, reinterpret_cast<struct sockaddr*>(&serverAdress),sizeof(serverAdress)) < 0 ){
        perror("Conection not estabilshed");
        exit(1);
    }

    inet_ntop(AF_INET, reinterpret_cast<struct sockaddr*>(&serverAdress), ip, INET_ADDRSTRLEN);
    qInfo() << "Conect to " << ip;
    pthread_create(&reciveMessageThread,nullptr,reciveMessage,&mySocket);

    QObject::connect(w.getSendButton(),&QPushButton::clicked,sendMessage);

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout,sendMessagetoUI);
    timer.start(500);

    w.show();
    return a.exec();

    pthread_join(reciveMessageThread, nullptr);
    close(mySocket);

}
