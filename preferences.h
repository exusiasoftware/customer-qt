#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>

namespace Ui {
class Preferences;
}

class Preferences : public QDialog
{
    Q_OBJECT

public:
    explicit Preferences(QWidget *parent = nullptr);
    ~Preferences();
    static QString userProfilePathOSX;
    static QString userProfilePathWIN;
    static QString userProfilePathLinux;

private slots:
    void on_pushButtonCancel_clicked();

    void on_pushButtonSave_clicked();
    QString detectPlatform();
    QString detectOS();
    void init();


private:
    Ui::Preferences *ui;
};

#endif // PREFERENCES_H
