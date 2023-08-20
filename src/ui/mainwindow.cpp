
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_pArcView = new QArcGraphicsView(this);
    ui->verticalLayout->addWidget(m_pArcView);

    m_pArcSence = new QGraphicsScene(this);//����
    m_pArcView->setScene(m_pArcSence);

    m_pArcItem = new QArcGraphicsItem();//����
    m_pArcSence->addItem(m_pArcItem);


    connect(m_pArcView, &QArcGraphicsView::updateItemPosSignal, this, &MainWindow::onMousePosition);//״̬����ʾ����



    statusLabel = new QLabel(this);
    statusLabel->setAlignment(Qt::AlignRight);
    ui->statusbar->addPermanentWidget(statusLabel);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::onMousePosition(QPointF pos)
{
    QString str;
    if (pos.isNull())
    {
        str = QString("��ǰ���꣺(0,0)");
    }
    str = QString("��ǰ���꣺ ") + "(" + QString::number(pos.x()) + "," + QString::number(pos.y()) + ")";
    statusLabel->setText(str);
}