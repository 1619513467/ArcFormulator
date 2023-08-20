#ifndef QARCGRAPHICSVIEW_H
#define QARCGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>

class QArcGraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
    QArcGraphicsView(QWidget *parent = nullptr);
	~QArcGraphicsView();

protected:

    void mouseMoveEvent(QMouseEvent *event) override;//����ƶ�

signals:

    void updateItemPosSignal(QPointF pos); //����ƶ��źŷ���
};
#endif //QARCGRAPHICSVIEW_H