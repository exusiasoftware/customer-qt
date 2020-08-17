#include "preferences.h"
#include "ui_preferences.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QDir>
#include <QFileInfo>

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);
    init();

}

QString Preferences::userProfilePathOSX = "/Library/Application Support/Customer/";
QString Preferences::userProfilePathWIN = "/AppData/Local/Customer/";
QString Preferences::userProfilePathLinux = "/usr/local/share/Customer/";


Preferences::~Preferences()
{
    delete ui;
}


void Preferences::init() {
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

                 ui->lineEditAll->setText(json_map["all"].toString());
                 ui->lineEditAdd->setText(json_map["add"].toString());
                 ui->lineEditUpdate->setText(json_map["update"].toString());
                 ui->lineEditDelete->setText(json_map["delete"].toString());

            }


    } else {
       qDebug() << "File not exist ";
   }

}




QString Preferences::detectPlatform()
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


QString Preferences::detectOS()
{
    return QSysInfo::productVersion();
}





void Preferences::on_pushButtonCancel_clicked()
{
    reject();
}

void Preferences::on_pushButtonSave_clicked()
{
     QString path = detectPlatform();

   if (!QFileInfo::exists(QDir::homePath() + path))  {
        QDir().mkdir(QDir::homePath() + path);
        qDebug() << "Customer folder created ";
    } else {
         qDebug() << "Customer folder exist ";
    }


    QString allObject = ui->lineEditAll->text();
    QString addObject = ui->lineEditAdd->text();
    QString updateObject = ui->lineEditUpdate->text();
    QString deleteObject = ui->lineEditDelete->text();

    QVariantMap APIMapData;
    APIMapData.insert("all",allObject);
    APIMapData.insert("add", addObject);
    APIMapData.insert("update",updateObject);
    APIMapData.insert("delete",deleteObject);

    QJsonDocument APIJsonData = QJsonDocument::fromVariant(APIMapData);

    QFile File(QDir::homePath() + path + "apipaths.json");

    File.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
    File.write(APIJsonData.toJson());
    File.close();



    reject();

}



