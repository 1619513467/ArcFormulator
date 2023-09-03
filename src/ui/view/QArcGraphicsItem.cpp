#include "QArcGraphicsItem.h"
#include <qmath.h>

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

// ��������֮��Ļ��Ƚ�
double QArcGraphicsItem::calculateAngle(const QPointF& center, const QPointF& point)
{
	double dx = point.x() - center.x();
	double dy = point.y() - center.y();
	return atan2(dy, dx);
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

    QPointF point(100, 100);
    painter->drawText(point, "Hello world!");


    // ����������������꣬����Ҫ�������滻Ϊ���ʵ������
	QPointF		   pointA(100, 100);
	QPointF		   pointB(200, 200);
	QPointF		   pointC(300, 100);

	// ���������ε����ĵ㣬��������������Ƕ���
	QPointF		   center((pointA.x() + pointB.x() + pointC.x()) / 3,
						  (pointA.y() + pointB.y() + pointC.y()) / 3);

	// ��������֮��ĽǶȲ�
	double	angleA = calculateAngle(center, pointA);
	double	angleB = calculateAngle(center, pointB);

	// ���㻡�Ȳȷ�������[0, 2*��]��Χ��
	double angleDifference = fmod((angleB - angleA + 2 * M_PI), (2 * M_PI));

	// ����һ��Բ���Ļ���·��
	QPainterPath   paths;
	paths.arcTo(50, 50, 30, 30, qRadiansToDegrees(angleA), qRadiansToDegrees(angleDifference));
	painter->drawPath(paths);
}

QRectF QArcGraphicsItem::boundingRect() const
{
    return m_Rect;
}

