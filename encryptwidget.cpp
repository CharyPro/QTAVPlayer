#include "encryptwidget.h"
#include "ui_encryptwidget.h"
#include <QFileDialog>
#include <QMessageBox>

#define OK 0
#define ERR -1
#define KEY 0xa9

EncryptWidget::EncryptWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EncryptWidget)
{
//    this->setFixedSize(this->width(), this->height());
    this->setFixedHeight(120);
    this->setWindowTitle(QStringLiteral("文件加密解密工具"));

    ui->setupUi(this);
}

EncryptWidget::~EncryptWidget()
{
    delete ui;
}

void EncryptWidget::on_Btn1_clicked()
{
    QString filename = QFileDialog::getOpenFileName(nullptr,
                       QStringLiteral("选择将操作的文件"),
                       "G:/Resource",
                       QStringLiteral(""));
    if (filename.isEmpty()) return;
    srcfile = filename;
    ui->lineEdit1->setText(filename);
}

void EncryptWidget::on_Btn2_clicked()
{
    QString filename = QFileDialog::getSaveFileName(nullptr,
                       QStringLiteral("选择将保存的文件"),
                       "G:/Resource",
                       QStringLiteral(""));
    if (filename.isEmpty()) return;
    ui->lineEdit2->setText(filename);
    dstfile = filename;
}

void EncryptWidget::on_runBtn_clicked()
{
    if(srcfile == dstfile) {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("不能将源文件和目标文件作为同一文件"));
        return;
    }

    if(Encrypt() < 0)
        QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("运行失败"));
    else
        QMessageBox::information(this, QStringLiteral("成功"), QStringLiteral("运行成功"));
}

int EncryptWidget::Encrypt()
{
    char szOldFile[100];
    strcpy(szOldFile, srcfile.toLocal8Bit().toStdString().c_str());
    char szNewFile[100];
    strcpy(szNewFile, dstfile.toLocal8Bit().toStdString().c_str());

    FILE* pOldFile = NULL;
    FILE* pNewFile = NULL;//指针初始化为NULL
    char cTemp;


    pOldFile = fopen(szOldFile, "rb");//二进制读写文件
    if (pOldFile == NULL)
    {
        return ERR;
    }

    pNewFile = fopen(szNewFile, "wb");
    if (pNewFile == NULL)
    {
        fclose(pOldFile);
        return ERR;
    }
    //一定要先读取字符然后做文件末尾判断
    cTemp = fgetc(pOldFile);
    while (!feof(pOldFile))//遇到文件结束 返回值为0
    {
        //putchar(cTemp);
        fputc(cTemp, pNewFile);
        cTemp = fgetc(pOldFile) ^ KEY;//异或运算加密文件

    }
    fclose(pOldFile);
    fclose(pNewFile);//用完文件之后需要关闭文件

    return OK;
}
