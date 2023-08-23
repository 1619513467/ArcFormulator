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
//    �ļ����ƣ�Point3D                                                          //
//    ��   �ߣ�Dust                                                             //
//    ʱ   �䣺2023��8��23��                                                     //
//                                                                             //
//*****************************************************************************//


#ifndef POINT3D_H
#define POINT3D_H

#include <cmath>
#include <ranges>

class Point3D
{
public:
    // Ĭ�Ϲ��캯������ʼ����Ϊԭ�㣨0,0,0��
	Point3D();
    // ���������캯�������ݸ����� x��y �� z ��ʼ����
    Point3D(double x, double y, double z);
    // �������캯��
    Point3D(const Point3D &other);
    // �ƶ����캯��
    Point3D(Point3D &&other);
    // ˫�������캯������ʼ�� x �� y ���꣬z ����Ĭ��Ϊ 0
    Point3D(double x, double y); 
    // ��������
    ~Point3D();
    
    // ��ֵ������
    Point3D &operator=(const Point3D &other);
    
    // ��ȡ x ����
    double getX() const;
    // ��ȡ y ����
    double getY() const;
    // ��ȡ z ����
    double getZ() const;

    // ���� x ����
    void setX(double x);
    // ���� y ����
    void setY(double y);
    // ���� z ����
    void setZ(double z);

    // ���㵽��һ����ľ���
    double distanceTo(const Point3D &other) const;

private:
    double m_x;
    double m_y;
    double m_z;
};
#endif //POINT3D_H