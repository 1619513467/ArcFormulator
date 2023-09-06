#include "QArcRibbonRegisterDialog.h"
#include "ui_QArcRibbonRegisterDialog.h"
#include <QFileDialog>
#include <QMessageBox>

QArcRibbonRegisterDialog::QArcRibbonRegisterDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::QArcRibbonRegister)
{
	ui->setupUi(this);
    // �Ƴ��Ϸ����ʺ�
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    connect(ui->mButtonIcon, &QPushButton::clicked, this, &QArcRibbonRegisterDialog::onAddIcon);
    connect(ui->mButtonOK, &QPushButton::clicked, this, &QArcRibbonRegisterDialog::onRegisterButton);
    connect(ui->mButtonQuit, &QPushButton::clicked, this, &QArcRibbonRegisterDialog::onQuitButton);
}

QArcRibbonRegisterDialog::~QArcRibbonRegisterDialog()
{
    delete ui;
}

void QArcRibbonRegisterDialog::onRegisterButton()
{
    QString strName = ui->mLineEditName->text();
    QString strAlsaName = ui->mLineEditAlasName->text();
    QString strIconPath = ui->mLineEditIcon->text();
    QString strHotKey = ui->mLineEditHotKey->text();
    QString strCategory = ui->mComboBoxCategory->currentText();
    QString strPannel = ui->mComboBoxPannel->currentText();

    if (strName.isEmpty())
    {
        QMessageBox::information(this, "��ʾ", "��������Ҫע��˵������ƣ�", QMessageBox::Ok);
        return;
    }
    if (strAlsaName.isEmpty() || strCategory.isEmpty() )//|| strPannel.isEmpty())
    {
        strAlsaName = strName;
        strCategory = strName;
        strPannel = strName;
    }

    Data data;
    data.m_strName = strName;
    data.m_strAlsaName = strAlsaName;
    data.m_strIconPath = strIconPath;
    data.m_strHotKey = strHotKey;
    data.m_strCategory = strCategory;
    data.m_strPannel = strPannel;

    pDataBase = new QArcDataBase(this);
    QString strError;
    bool isSucceed = pDataBase->insertData(data, strError);
    if (!isSucceed)
    {
        QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("�˵�ע��ʧ�ܣ������룺%1").arg(strError), QMessageBox::Ok);
    }
    else
    {
        QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("�˵�ע��ɹ���"), QMessageBox::Ok);
    }
}

void QArcRibbonRegisterDialog::onQuitButton()
{
    QArcTabWidget *tabWidget = new QArcTabWidget();
    tabWidget->setWindowModality(Qt::ApplicationModal); // ����ģ̬
    tabWidget->show();
}

void QArcRibbonRegisterDialog::onAddIcon()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"/home",tr("Images (*.png *.xpm *.jpg)"));
    ui->mLineEditIcon->setText(fileName);
}