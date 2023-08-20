#include "GemBezierCurve.h"
#include <cassert>

GemBezierCurve::GemBezierCurve()
{
}

GemBezierCurve::~GemBezierCurve()
{
}

void GemBezierCurve::SetCtrlPoint(Point2D& stPt)
{
    m_vecCtrlPt.push_back(stPt);
}

bool GemBezierCurve::CreateCurve()
{
    // ȷ���Ƕ������ߣ�2������һ�����Ƶ�
    assert(m_vecCtrlPt.size() == 3);

    // t�������� ����ͨ��setp��Сȷ����Ҫ����������ϵ�ĸ���
    float step = 0.01;
    for (float t = 0.0; t <= 1.0; t += step)
        {
            Point2D stPt;
            CalCurvePoint(t, stPt);
            m_vecCurvePt.push_back(stPt);
        }
    return false;
}

void GemBezierCurve::CalCurvePoint(float t, Point2D& stPt)
{
    // ȷ���Ƕ������ߣ�2������һ�����Ƶ�
    assert(m_vecCtrlPt.size() == 3);

    // �������ߵ����꣬��Ϊ2���㷨���ı�˴�����ʵ�ֶ������
    //float x = (float) m_vecCtrlPt[0].x * pow(1 - t, 2) +
    //          (float) m_vecCtrlPt[1].x * t * (1 - t) * 2 +
    //          (float) m_vecCtrlPt[2].x * pow(t, 2);
    //float y = (float) m_vecCtrlPt[0].y * pow(1 - t, 2) +
    //          (float) m_vecCtrlPt[1].y * t * (1 - t) * 2 +
    //          (float) m_vecCtrlPt[2].y * pow(t, 2);

    float x = 0, y = 0;

    stPt.setX(x);
    stPt.setY(y);
}
