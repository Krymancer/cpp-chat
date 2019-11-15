#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow){
    ui->setupUi(this);
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
    this->ui->textBrowser->append(message);
}

QPushButton* MainWindow::getSendButton(){
    return this->ui->pushButton;
}
