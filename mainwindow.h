#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

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

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
