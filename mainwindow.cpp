#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow){
    ui->setupUi(this);
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(onListItemClicked(QListWidgetItem*)));
}

MainWindow::~MainWindow(){
    delete ui;
}

QString MainWindow::getMessage(){
    return this->ui->plainTextEdit_message->toPlainText();
}

QString MainWindow::getSubject(){
    return this->ui->lineEdit_subject->text();
}

QString MainWindow::getReciver(){
    return this->ui->lineEdit_to->text();
}

void MainWindow::addMessage(QString message){
    QStringList splitedMessage = message.split('\n');
    QString sender = splitedMessage.last();
    splitedMessage.pop_back();
    splitedMessage.pop_front();
    message = "From: " + sender + "\n" + splitedMessage.join("\n") + "\n\n";
    addChat(sender,message);
}

QPushButton* MainWindow::getSendButton(){
    return this->ui->pushButton;
}

void MainWindow::addChat(QString sender, QString message){

    if(!this->messages.contains(sender)){
        this->messages.insert(sender,message);
        int row = this->ui->listWidget->model()->rowCount();
        this->ui->listWidget->insertItem(row,sender);
    }else{
        QString oldMessages = this->messages.find(sender).value();
        QString newMessages = oldMessages + message;
        this->messages.insert(sender,newMessages);
    }

    this->updateChat(sender);
}

void MainWindow::onListItemClicked(QListWidgetItem* item){
    QString name = item->text();
    this->updateChat(name);
    QString messagesPlainText = this->ui->textBrowser->toPlainText();
    QString m = this->messages.find(name).value();
    this->messages.insert(name,messagesPlainText);

}

void MainWindow::updateChat(QString name){
        QString messages = this->messages.find(name).value();
        this->ui->textBrowser->setText(messages);

}
