#include "QArcGraphicsItem.h"

QArcGraphicsItem::QArcGraphicsItem()
{

}

QArcGraphicsItem::~QArcGraphicsItem()
{

}

void QArcGraphicsItem::setRect(const QRectF &rect)
{
    m_Rect = rect;
}

void QArcGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) 
{
    painter->setRenderHint(QPainter::Antialiasing);
    QPen show_pen;
    show_pen.setColor(Qt::red);
    show_pen.setStyle(Qt::SolidLine);
    painter->setPen(show_pen);
    // ������㡢���Ƶ���յ�
    QPointF start(20, 150);
    QPointF ctrl(50, 50);
    QPointF end(100, 120);

    // ����QPainterPath�����ƶ������
    QPainterPath path(start);

    // ��Ӷ��α���������
    path.quadTo(ctrl, end);

    // ����·��
    painter->drawPath(path);

    QPointF point(10, 10);
    painter->drawText(point, "Hello world!");
}

QRectF QArcGraphicsItem::boundingRect() const
{
    return m_Rect;
}

