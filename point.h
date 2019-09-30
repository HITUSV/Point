/******************************************************************************
 *
 * Copyright 2019 wumo1999@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *****************************************************************************/

//
// Created by wumode on 19-8-19.
//

#ifndef BOAT_POINT_H
#define BOAT_POINT_H
#include <utm.h>
#include <datum.h>
#include <cmath>
#include <iostream>
#include <iomanip>


namespace navigation
{
    typedef struct Angle{
        double roll = 0.0;
        double pitch = 0.0;
        double yaw = 0.0;
    }Angle;

    typedef struct fAngle{
        volatile float roll = 0.0;
        volatile float pitch = 0.0;
        volatile float yaw = 0.0;
    }fAngle;

    typedef struct GpsPosition{
        double latitude = 0.0;
        double longitude = 0.0;
    }GpsPosition;

    typedef struct UtmPosition{
        double x = 0.0;
        double y = 0.0;
        GridZone gridZone = GRID_AUTO;
        Hemisphere hemisphere = HEMI_AUTO;
    }UtmPosition;

    typedef struct Position{
        UtmPosition utm_position;
        double height = 0.0;
    }Position;


    inline double CalcAngleUtm(UtmPosition* key_position, UtmPosition* now_position){
        float angle;
        angle = atan2(key_position->y-now_position->y, key_position->x-now_position->x);
        if(angle<0){
            angle += 2*M_PI;
        }
        return angle;
    }

    namespace point{
        enum class CoordinateSystem{
            kWgs84Coordinate = 1,
            kUtmCoordinate = 2
        };
        typedef enum AngleUnit{
            kRadian = 0,    ///弧度制
            kDegree = 1     ///角度制
        }AngleUnit;
        class Point {
        public:
            struct Uninitialized : public std::exception{
                const char * what () const noexcept override
                {
                    return "Point uninitialized";
                }
            };
        public:
            Point();
            explicit Point(const navigation::GpsPosition& gpsPosition);
            explicit Point(const navigation::UtmPosition& utmPosition);
            Point(double longitude, double latitude);
            Point(double x, double y, GridZone zone, Hemisphere hemisphere);
            Point(const Point &P);

            ///移动构造，提高性能
            Point(Point&& P) noexcept ;
            ~Point();
            Point& operator=(const Point &P);
            Point& operator=(const UtmPosition& utmPosition);
            Point& operator=(const GpsPosition& gpsPosition);
            Point& operator()(const Point& P);
            Point& operator()(double longitude, double latitude, AngleUnit u = kRadian);
            Point& operator()(double x, double y, GridZone zone, Hemisphere hemisphere);
            bool operator ==(const Point& P);
            friend std::ostream &operator<<(std::ostream &output, const Point &D );
            UtmPosition Utm () const;
            GpsPosition Gps() const;
            double Height() const;
            double Height(double h);
            double Altitude() const;
            double Altitude(double altitude);
            bool Initialized() const;
            double Angle(const navigation::point::Point& B);
            static double Distance(const navigation::point::Point& point1, const Point& point2);
            ///返回向量AB在ENU参考系的角度
            static double Angle(const navigation::point::Point& A, const Point& B);
        private:
            bool initialized_;
            GpsPosition* gps_;
            UtmPosition* utm_;
            /// 相对高度
            double relative_height_;
            /// 海拔高度
            double altitude_;
            void GpsToUtm();
            void UtmToGps();
        };

        //double Distance(Point* point1, Point* point2);
        //double Distance(navigation::point::Point&& point1, Point&& point2);
        //double Distance(const navigation::point::Point& point1, const Point& point2);
        double CalcAngle(navigation::point::Point* end_point, Point* starting_point);
    }
}

#endif //BOAT_POINT_H
