#include "QArcRibbonRegisterDialog.h"
#include "ui_QArcRibbonRegisterDialog.h"

QArcRibbonRegisterDialog::QArcRibbonRegisterDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::QArcRibbonRegister)
{
	ui->setupUi(this);
    // �Ƴ��Ϸ����ʺ�
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

QArcRibbonRegisterDialog::~QArcRibbonRegisterDialog()
{
    delete ui;
}
