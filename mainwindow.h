#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QMap>
#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString getMessage();
    QString getSubject();
    QString getReciver();

    void addMessage(QString message);
    QPushButton* getSendButton();
    void addChat(QString sender, QString message);
    void updateChat(QString name);

public slots:
    void onListItemClicked(QListWidgetItem* item);

private:
    Ui::MainWindow *ui;
    QMap<QString,QString> messages;


};
#endif // MAINWINDOW_H
