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
    init();
    refreshCustomerList();
    ui->pushButtonDelete->setEnabled(false);
    ui->pushButtonUpdate->setEnabled(false);

}

QString MainWindow::userProfilePathOSX = "/Library/Application Support/Customer/";
QString MainWindow::userProfilePathWIN = "/AppData/Local/Customer/";
QString MainWindow::userProfilePathLinux = "/usr/local/share/Customer/";

MainWindow::~MainWindow()
{
    delete ui;
}


QString MainWindow::detectPlatform()
{
    QString path;

    if(QSysInfo::productType() == "osx") {
        path =  userProfilePathOSX;
    }

    if(QSysInfo::productType() == "windows") {
       path = userProfilePathWIN;
    }

    if(QSysInfo::productType() == "ubuntu") {
        path = userProfilePathLinux;
    }

    return path;

}



void MainWindow::init() {
    QVariantMap json_map;
    QString path = detectPlatform();
     qDebug() << path;

    if( QFileInfo(QDir::homePath() + path + "apipaths.json").exists()){
            QFile file_obj(QDir::homePath() + path + "apipaths.json");
            if(!file_obj.open(QIODevice::ReadOnly)){
                qDebug() << "Failed to open ";
                exit(1);
            } else {

                 qDebug() << "File opened ";
                 QTextStream file_text(&file_obj);
                 QString json_string;
                 json_string = file_text.readAll();
                 file_obj.close();

                 QJsonDocument jsonResponse = QJsonDocument::fromJson(json_string.toUtf8());

                 QJsonObject jsonObject = jsonResponse.object();
                 QVariantMap json_map = jsonObject.toVariantMap();


                 MainWindow::customerALL = json_map["all"].toString();
                 MainWindow::customerUpdate = json_map["update"].toString();
                 MainWindow::customerDelete = json_map["delete"].toString();
                 MainWindow::customerAdd = json_map["add"].toString();

            }


    } else {
       qDebug() << "File not exist ";
   }

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

//    const QUrl API_ENDPOINT("http://localhost:8080/customer-api/v1/customers/all");
    const QUrl API_ENDPOINT(MainWindow::customerALL);

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
//    const QUrl API_ENDPOINT("http://localhost:8080/customer-api/v1/customer/" + id);
     const QUrl API_ENDPOINT(MainWindow::customerUpdate + id);


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

//    const QUrl API_ENDPOINT("http://localhost:8080/customer-api/v1/customers/" + id);
    const QUrl API_ENDPOINT(MainWindow::customerDelete + id);

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
//    const QUrl API_ENDPOINT("http://localhost:8080/customer-api/v1/customers");
     const QUrl API_ENDPOINT(MainWindow::customerAdd);

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

void MainWindow::on_actionClose_triggered()
{
    QApplication::quit();
}
