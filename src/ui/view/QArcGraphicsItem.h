#ifndef QARCGRAPHICSITEM_H
#define QARCGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QPainter>
#include <QtGlobal>


class QArcGraphicsItem : public QGraphicsItem 
{
public:
    QArcGraphicsItem();
	
	~QArcGraphicsItem();

	void setRect(const QRectF &rect);



protected:

	// @brief ����ͼ��������ݡ�
	//
	// ��������� QGraphicsView ��ܵ��ã���������ͼ����Ⱦͼ���
	// 
	// @param painter ָ����л��Ʋ����� QPainter �����ָ�롣
	// @param option �ṩ����ѡ���ͼ�����״̬���� QStyleOptionGraphicsItem �����ָ�롣
	// @param widget ������ͼ������һ�������б����ƣ���˲���ָ��ò�����Ĭ��Ϊ nullptr��
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    
    // @brief boundingRect ���ڷ���ͼ����ı߽���Ρ�
    //
    // ����߽���ζ�����ͼ�������緶Χ����ͼ�����ڳ�����scene������ռ�õľ�������
    // Qt��ͼ����ͼ���ʹ��������ν��и������㣬������ײ��⡢��Ⱦ�Ż��ȡ�
    // �߽����Ӧ�þ����ܵؽ��գ���Ӧ����ͼ�������ƻ򽻻�������
    // 
    // @return ����һ��QRectF���󣬴�����ͼ����ı߽���Ρ�
    // �߽���ε������������ͼ�������������ϵͳ�����ǳ�������ͼ������ϵͳ����
	virtual QRectF boundingRect() const override;


private:
	double calculateAngle(const QPointF& center, const QPointF& point);


private:
    QRectF m_Rect;
};
#endif//QARCGRAPHICSITEM_H