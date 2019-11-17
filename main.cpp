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

static bool isNullEmpyOrOnlySpaces(QString string){
    std::string str = string.toUtf8().data();
    return string.isNull()
            || string.isEmpty()
            || str.find_first_not_of (' ') == str.npos
            || str.find_first_not_of ('\t') == str.npos
            || str.find_first_not_of ('\n') == str.npos
            || str.find_first_not_of ('\r') == str.npos
            || str.find_first_not_of ('\v') == str.npos
            || str.find_first_not_of ('\f') == str.npos
            || str.find_first_not_of (" \n\t\r\v\f") == str.npos;
}
static void sendMessage(){
    QString reciver = uiUpdateHandler->getReciver();
    QString subject = uiUpdateHandler->getSubject();
    QString message = uiUpdateHandler->getMessage();
    QString sing = username;



    if(isNullEmpyOrOnlySpaces(reciver) || isNullEmpyOrOnlySpaces(message)){
        return;
    }

    QString msg = "To: " + reciver + "\nSubject: " + subject + "\nMessage: " + message + "\n" + sing;
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
    return me == message;
}

void *reciveMessage(void *socket) {
    int their_sock = *(static_cast<int*>(socket));
    char msg[500];
    int len;
    while ((len = static_cast<int>(recv(their_sock, msg, 500, 0)) > 0)) {
        QString message = QString::fromUtf8(msg);
        if(itsToMe(message.QString::split('\n')[0]))
        writeMessage(message);
        memset(msg, '\0', sizeof(msg));
    }
    return nullptr;
}

void debugMessages(){
    writeMessage("To: a1\nSubject: a2\nMessage: Message sent from a2\na2");
    writeMessage("To: a1\nSubject: a3\nMessage: Message sent from a3\na3");
    writeMessage("To: a1\nSubject: a4\nMessage: Message sent from a4\na4");

    writeMessage("To: a1\nSubject: a2\nMessage: Another message sent from a2\na2");
    writeMessage("To: a1\nSubject: a2\nMessage: More one message sent from a2\na2");

    writeMessage("To: a1\nSubject: a4\nMessage: Message No 2 sent from a4\na4");

    writeMessage("To: a1\nSubject: a3\nMessage: Message No 2 sent from a3\na3");
    writeMessage("To: a1\nSubject: a3\nMessage: Message No 3 sent from a3\na3");
    writeMessage("To: a1\nSubject: a3\nMessage: Message No 4 sent from a3\na3");
}


int main(int argc, char *argv[]){


    QApplication a(argc, argv);
    MainWindow w;
    uiUpdateHandler = &w;

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

    QString serverIP = QInputDialog::getText(nullptr,
            "Cpp-Chat", "Server Address:", QLineEdit::Normal,
            QString(), &ok,nullptr );
    if ( ok && !text.isEmpty() ) {
        QByteArray serverIpByte = serverIP.toLocal8Bit();
        const char* parsedServerIp = serverIpByte.data();
       strcpy(ip,parsedServerIp);
    } else {
        exit(1);
    }

    portno = 1010;
    mySocket = socket(AF_INET, SOCK_STREAM, 0);
    memset(serverAdress.sin_zero,'\0',sizeof(serverAdress.sin_zero));
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_port = htons(static_cast<uint16_t>(portno));
    //serverAdress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAdress.sin_addr.s_addr = inet_addr(ip);

    if(connect(mySocket, reinterpret_cast<struct sockaddr*>(&serverAdress),sizeof(serverAdress)) < 0 ){
        perror("Conection not estabilshed");
        exit(1);
    }

    inet_ntop(AF_INET, reinterpret_cast<struct sockaddr*>(&serverAdress), ip, INET_ADDRSTRLEN);
    qInfo() << "Conect to " << inet_ntoa(serverAdress.sin_addr);
    pthread_create(&reciveMessageThread,nullptr,reciveMessage,&mySocket);

    //debugMessages();

    QObject::connect(w.getSendButton(),&QPushButton::clicked,sendMessage);

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout,sendMessagetoUI);
    timer.start(500);

    w.show();
    return a.exec();

    pthread_join(reciveMessageThread, nullptr);
    close(mySocket);

}
