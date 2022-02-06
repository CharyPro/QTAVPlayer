#ifndef ENCRYPTWIDGET_H
#define ENCRYPTWIDGET_H

#include <QWidget>

namespace Ui {
class EncryptWidget;
}

class EncryptWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EncryptWidget(QWidget *parent = nullptr);
    ~EncryptWidget();

public slots:
    void on_Btn1_clicked();
    void on_Btn2_clicked();
    void on_runBtn_clicked();

private:
    Ui::EncryptWidget *ui;

    QString srcfile;
    QString dstfile;

    int Encrypt();
};

#endif // ENCRYPTWIDGET_H
