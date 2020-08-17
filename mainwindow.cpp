#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qurl.h"
#include "mainwindow.h"
#include "mainwindow.h"
#include "mainwindow.h"
#include "mainwindow.h"
#include "mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariantMap>

#include <QMessageBox>
#include <QListWidgetItem>
#include "preferences.h"





MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
     mNetManager(new QNetworkAccessManager(this)),
     mNetReply(nullptr),
     mNetReplyItem(nullptr),
     mDataBuffer(new QByteArray),
     mDataBufferItem(new QByteArray)

{

    ui->setupUi(this);
    refreshCustomerList();
    ui->pushButtonDelete->setEnabled(false);
    ui->pushButtonUpdate->setEnabled(false);



}

MainWindow::~MainWindow()
{
    delete ui;
}



///////////////////////// Click on Customer List //////////////////////////////

void MainWindow::on_listName_itemClicked(QListWidgetItem *item)
{

   QString listWidget = "listName";
   ui->pushButtonDelete->setEnabled(true);
   ui->pushButtonUpdate->setEnabled(true);
   refreshCustomer(item, listWidget);


}


void MainWindow::on_listID_itemClicked(QListWidgetItem *item)
{

      QString listWidget = "listID";
      ui->pushButtonDelete->setEnabled(true);
      ui->pushButtonUpdate->setEnabled(true);
      refreshCustomer(item, listWidget);


}




void  MainWindow::refreshCustomer(QListWidgetItem *item,QString listWidget) {

    ui->lineEditName->clear();
    ui->lineEditTitle->clear();
    ui->lineEditDepartment->clear();
    mDataBufferItem->clear();

    int rowNumber = 0;

    if (listWidget == "listID") {
         rowNumber = ui->listID->row(item);
    } else {
         rowNumber = ui->listName->row(item);
    }

    ui->listID->item(rowNumber)->setSelected(true);
    ui->listName->item(rowNumber)->setSelected(true);

    QListWidgetItem *itemID = ui->listID->item(rowNumber);


    MainWindow::customerID = itemID->text();

    const QUrl API_ENDPOINT("http://localhost:8080/customer-api/v1/customer/" + MainWindow::customerID);
    QNetworkRequest request;
    request.setUrl(API_ENDPOINT);

     qDebug() << MainWindow::customerID;

    mNetReplyItem = mNetManager->get(request);
    connect(mNetReplyItem,&QIODevice::readyRead,this,&MainWindow::dataReadyReadItem);
    connect(mNetReplyItem,&QNetworkReply::finished,this,&MainWindow::dataReadFinishedItem);

}



void MainWindow::dataReadyReadItem()
{
    mDataBufferItem->append(mNetReplyItem->readAll());

}

void MainWindow::dataReadFinishedItem()
{

    if ( mNetReplyItem->error()) {
        qDebug() << "Error : " << mNetReplyItem->errorString();
    } else {
        QJsonDocument doc = QJsonDocument::fromJson(*mDataBufferItem);


        QJsonObject customer = doc.object();
            qDebug() <<  customer;

        QVariantMap map = customer.toVariantMap();
        QString name = map["name"].toString();
        QString title = map["title"].toString();
        QString department = map["department"].toString();
        ui->lineEditName->insert(name);
        ui->lineEditTitle->insert(title);
        ui->lineEditDepartment->insert(department);
        mDataBufferItem->clear();


    }
}

/////////////////////////////////////////////////////////////////////////////








void MainWindow::refreshCustomerList() {

    mDataBuffer->clear();

    ui->listID->clear();
    ui->listName->clear();

    ui->lineEditName->clear();
    ui->lineEditTitle->clear();
    ui->lineEditDepartment->clear();

    const QUrl API_ENDPOINT("http://localhost:8080/customer-api/v1/customers/all");
    QNetworkRequest request;
    request.setUrl(API_ENDPOINT);

    mNetReply = mNetManager->get(request);
    connect(mNetReply,&QIODevice::readyRead,this,&MainWindow::dataReadyRead);
    connect(mNetReply,&QNetworkReply::finished,this,&MainWindow::dataReadFinished);

}


void MainWindow::dataReadyRead()
{
    mDataBuffer->append(mNetReply->readAll());

}

void MainWindow::dataReadFinished()
{
    if ( mNetReply->error()) {
        qDebug() << "Error : " << mNetReply->errorString();
    } else {

        QJsonDocument doc = QJsonDocument::fromJson(*mDataBuffer);

        QJsonArray array = doc.array();


        for ( int i = 0; i < array.size(); i++) {
            QJsonObject object = array.at(i).toObject();
            QVariantMap map = object.toVariantMap();

            QString id = map["id"].toString();
            QString name = map["name"].toString();
            QString title = map["title"].toString();
            QString department = map["department"].toString();

            QListWidgetItem *idItem  = new QListWidgetItem(id);
            QListWidgetItem *nameItem  = new QListWidgetItem(name);

            ui->listID->addItem(idItem);
            ui->listName->addItem(nameItem);

        }

    }

}








void MainWindow::on_pushButtonUpdate_clicked()
{

    QString id = MainWindow::customerID;
    QString name = ui->lineEditName->text();
    QString title = ui->lineEditTitle->text();
    QString department = ui->lineEditDepartment->text();



    QVariantMap testMapData;
    testMapData.insert("name",name);
    testMapData.insert("title",title);
    testMapData.insert("department",department);

    QJsonDocument testJsonData = QJsonDocument::fromVariant(testMapData);
    const QUrl API_ENDPOINT("http://localhost:8080/customer-api/v1/customer/" + id);
    QNetworkRequest request;
    request.setUrl(API_ENDPOINT);
    request.setRawHeader("Content-Type", "application/json");

    mNetReplyItem = mNetManager->put(request, testJsonData.toJson());
    connect(mNetReplyItem,&QIODevice::readyRead,this,&MainWindow::dataReadyReadItemID);
    connect(mNetReplyItem,&QNetworkReply::finished,this,&MainWindow::dataReadFinishedItemID);




}








void MainWindow::on_pushButtonDelete_clicked()
{

    QString id = MainWindow::customerID;

    const QUrl API_ENDPOINT("http://localhost:8080/customer-api/v1/customers/" + id);
    QNetworkRequest request;
    request.setUrl(API_ENDPOINT);


    mNetReplyItem = mNetManager->deleteResource(request);
    connect(mNetReplyItem,&QIODevice::readyRead,this,&MainWindow::dataReadyReadItemID);
    connect(mNetReplyItem,&QNetworkReply::finished,this,&MainWindow::dataReadFinishedItemID);



}


void MainWindow::dataReadyReadItemID()
{
    mDataBufferItem->append(mNetReplyItem->readAll());

}


void MainWindow::dataReadFinishedItemID()
{

    if ( mNetReplyItem->error()) {
        qDebug() << "Error : " << mNetReplyItem->errorString();
    } else {

         refreshCustomerList();
    }

}










void MainWindow::on_pushButtonAdd_clicked()
{

    QString name = "New Customer";
    QString title = "Title";
    QString department = "Department";



    QVariantMap testMapData;
    testMapData.insert("name",name);
    testMapData.insert("title",title);
    testMapData.insert("department",department);

    QJsonDocument testJsonData = QJsonDocument::fromVariant(testMapData);
    const QUrl API_ENDPOINT("http://localhost:8080/customer-api/v1/customers");
    QNetworkRequest request;
    request.setUrl(API_ENDPOINT);
    request.setRawHeader("Content-Type", "application/json");

    mNetReplyItem = mNetManager->post(request, testJsonData.toJson());
    connect(mNetReplyItem,&QIODevice::readyRead,this,&MainWindow::dataReadyReadItemID);
    connect(mNetReplyItem,&QNetworkReply::finished,this,&MainWindow::dataReadFinishedItemID);
}







void MainWindow::on_pushButtonRefresh_clicked()
{
    ui->pushButtonDelete->setEnabled(false);
    ui->pushButtonUpdate->setEnabled(false);
     refreshCustomerList();


}


void MainWindow::on_actionPreferences_triggered()
{
    Preferences d;
    d.setModal(true);
    d.exec();
}
