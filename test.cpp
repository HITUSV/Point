#include <iostream>
#include <chrono>
#include "point.h"


int main(int argc, char* argv[]){
    navigation::point::Point p1;
    navigation::point::Point p2;

    ///角度值经纬度赋值, 第三个参数是单位，默认弧度
    p1(122.000, 37.000, navigation::point::kDegree);
    ///弧度值可不加单位参数
    p2(122.00001/180*M_PI, 37.00001/180*M_PI);
    ///或者
    p2(122.00001/180*M_PI, 37.00001/180*M_PI, navigation::point::kRadian);
    navigation::UtmPosition u1;
    navigation::GpsPosition g2;

    double d = navigation::point::Point::Distance(p1, navigation::point::Point(122.001, 37.001));
    std::cout<<"p1 p2距离: "<<d<<std::endl;

    navigation::point::Point p3, p4;

    u1 = p1.Utm();
    g2 = p2.Gps();
    ///通过Utm坐标赋值
    p3 = u1;
    ///通过Gps坐标赋值
    p4 = g2;

    ///移动语义构造， p2被掏空，继续使用将抛出错误Uninitialized
    navigation::point::Point p5(std::move(p2));
    try {
        /// Point比较
        std::cout<<(p5 == p2)<<std::endl;
    }///捕获未初始化错误
    catch (navigation::point::Point::Uninitialized& e){
        std::cout<<"Uninitialized"<<std::endl;
    }

    navigation::point::Point p6;

    ///通过Point赋值
    p6(p5);
    ///p5 p6比较
    std::cout<<(p5 == p6)<<std::endl;

    /// p1p5在ENU下的角度
    double angle = navigation::point::Point::Angle(p1, p5);
    std::cout<<"angle: "<<angle/M_PI*180.0<<std::endl;

    ///输出
    std::cout<<p1<<std::endl;

    p1(120.000, 37.0, navigation::point::kDegree);
    p5(120.000, 37.0002, navigation::point::kDegree);
    std::chrono::steady_clock::time_point t1, t2;
    t1 = std::chrono::steady_clock::now();
    for(size_t i=0; i<1000000; i++){
        navigation::point::Point::Distance(p1, p5);
    }
    t2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> d1 = std::chrono::duration_cast<std::chrono::duration<double >>(t2 - t1);
    std::cout<<"time used: "<<d1.count()*1e6<<" us"<<std::endl;
    return 0;
}