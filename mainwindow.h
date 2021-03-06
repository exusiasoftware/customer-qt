#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
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
    QString customerID;
    static QString userProfilePathOSX;
    static QString userProfilePathWIN;
    static QString userProfilePathLinux;
    QString customerALL;
    QString customerUpdate;
    QString customerDelete;
    QString customerAdd;



private slots:

    void dataReadyRead();
    void dataReadFinished();
    void dataReadyReadItem();
    void dataReadFinishedItem();
    void dataReadyReadItemID();
    void dataReadFinishedItemID();
    void refreshCustomerList();
    void refreshCustomer(QListWidgetItem *item,QString listWidget);

    void on_listName_itemClicked(QListWidgetItem *item);

    void on_pushButtonUpdate_clicked();

    void on_pushButtonDelete_clicked();

    void on_pushButtonAdd_clicked();

    void on_pushButtonRefresh_clicked();

    void on_listID_itemClicked(QListWidgetItem *item);

    void on_actionPreferences_triggered();
     void init();
      QString detectPlatform();

      void on_actionClose_triggered();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager * mNetManager;
    QNetworkReply * mNetReply;
    QNetworkReply * mNetReplyItem;
    QByteArray * mDataBuffer;
    QByteArray * mDataBufferItem;



};
#endif // MAINWINDOW_H
