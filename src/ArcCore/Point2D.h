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
//    �ļ����ƣ�Point2D                                                          //
//    ��   �ߣ�Dust                                                             //
//    ʱ   �䣺2023��8��20��                                                     //
//                                                                             //
//*****************************************************************************//

#ifndef POINT2D_H
#define POINT2D_H

#include <cmath>
#include <iostream>

class Point2D 
{
public:
    // Ĭ�Ϲ��캯���������ʼ��Ϊԭ�� (0, 0)
    Point2D() : m_x(0), m_y(0){}

    // ��������
    Point2D(double x, double y);

    // ��������
    Point2D(const Point2D &point2d);

    // ���õ��x����
    void setX(double x);

    // ���õ��y����
    void setY(double y);

    // ��ȡ���x����
    double getX() const;

    // ��ȡ���y����
    double getY() const;

    // ��������һ��ľ���
    double distanceTo(const Point2D &other) const;

    // ����������
    void print() const;

private:
    double m_x;
    double m_y;
};

#endif // POINT2D_H