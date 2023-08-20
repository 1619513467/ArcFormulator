//*****************************************************************************//
//                  _____                                                      //
//                 / ____|                     /\                              //
//                | |  __   ___  _ __ ___     /  \    _ __  ___                //
//                | | |_ | / _ \| '_ ` _ \   / /\ \  | '__|/ __|               //
//                | |__| ||  __/| | | | | | / ____ \ | |  | (__                //
//                 \_____| \___||_| |_| |_|/_/    \_\|_|   \___|               //
//                                                                             //
//                                                                             //
//                                                                             //
//                                                                             //
//                                                                             //
//    �ļ����ƣ�GemBezierCurve                                                   //
//    ��   �ߣ�Dust                                                             //
//    ʱ   �䣺2023��8��20��                                                     //
//                                                                             //
//*****************************************************************************//


#ifndef GEMBEZIERCURVE_H
#define GEMBEZIERCURVE_H

#include <vector>
#include "Point2D.h"

class GemBezierCurve
{
public:
    GemBezierCurve();

    ~GemBezierCurve();

    void SetCtrlPoint(Point2D& stPt);

    bool CreateCurve();

private:
    // ��Ҫ�㷨���������߸���������
    void CalCurvePoint(double t, Point2D& stPt);

private:
    // ����Ϳ��Ƶ�����
    std::vector<Point2D> m_vecCtrlPt;
    // �����ϸ�����������
    std::vector<Point2D> m_vecCurvePt;
};
#endif //GEMBEZIERCURVE_H
