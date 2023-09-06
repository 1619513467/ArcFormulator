#include "QArcTabWidget.h"
#include <QCheckBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QStringListModel>
#include <QLabel>
#include <QDebug>

QArcTabWidget::QArcTabWidget(QWidget *parent) 
    : QWidget(parent)
{
    QHBoxLayout* pHBoxLayout = new QHBoxLayout(this);
    //m_pListView = new QListView(this);
    m_pListWidget = new QListWidget(this);
    m_pStackWidget = new QStackedWidget(this);

    QStringListModel *model = new QStringListModel(this);

    QStringList list = {"aaa", "bbb", "ccc"};
    //model->setStringList(list);//ֻҪ�Ѿ���ģ�ͺ���ͼ�󶨣���ôģ���е����ݾͿ�������ͼ����ʾ
    int index = 0;
    for (auto i: list)
    {
        item = new QListWidgetItem(m_pListWidget);
        item->setSizeHint(QSize(0, 20));
        QArcWidgetItem *pItem = new QArcWidgetItem(index, i, this);
        m_pListWidget->setItemWidget(item, pItem);
        connect(pItem, &QArcWidgetItem::listItemClicked, this, &QArcTabWidget::onListItemChecked);
        ++index;
    }

    //m_pListView->setModel(model);

    pHBoxLayout->addWidget(m_pListWidget);
    pHBoxLayout->addWidget(m_pStackWidget);
}

QArcTabWidget::~QArcTabWidget()
{

}

void QArcTabWidget::onListItemChecked(int index)
{
    QListWidgetItem *item = m_pListWidget->item(index);//��ȡ�û���ǰѡ����
    QArcWidgetItem *pItem = static_cast<QArcWidgetItem *>(m_pListWidget->itemWidget(item));
    pItem->checkBox()->setChecked(!pItem->checkBox()->isChecked());

    int currentIndex = m_pStackWidget->addWidget(new QLabel(pItem->checkBoxText(), m_pStackWidget));
    m_pStackWidget->setCurrentIndex(currentIndex);

}