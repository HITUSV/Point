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

#include "point.h"


namespace navigation{
    namespace point{
        Point::Point() {
            utm_ = new navigation::UtmPosition;
            gps_ = new navigation::GpsPosition;
            utm_->x = 0.0;
            utm_->y = 0.0;
            gps_->latitude = 0.0;
            gps_->longitude = 0.0;
            relative_height_ = 0.0;
            altitude_ = 0.0;
            initialized_ = false;
        }
        Point::Point(const navigation::GpsPosition& gpsPosition) {
            utm_ = new navigation::UtmPosition;
            gps_ = new navigation::GpsPosition;
            *gps_ = gpsPosition;
            GpsToUtm();
            relative_height_ = 0.0;
            altitude_ = 0.0;
            initialized_ = true;
        }

        Point::Point(const navigation::UtmPosition& utmPosition) {
            utm_ = new navigation::UtmPosition;
            gps_ = new navigation::GpsPosition;
            *utm_ = utmPosition;
            UtmToGps();
            relative_height_ = 0.0;
            altitude_ = 0.0;
            initialized_ = true;
        }

        Point::Point(double longitude, double latitude){
            utm_ = new navigation::UtmPosition;
            gps_ = new navigation::GpsPosition;
            gps_->longitude = longitude/180.0*M_PI;
            gps_->latitude = latitude/180.0*M_PI;
            GpsToUtm();
            relative_height_ = 0.0;
            altitude_ = 0.0;
            initialized_ = true;
        }

        Point::Point(double x, double y, GridZone zone, Hemisphere hemisphere) {
            utm_ = new navigation::UtmPosition;
            gps_ = new navigation::GpsPosition;
            utm_->y = y;
            utm_->x = x;
            utm_->hemisphere = hemisphere;
            utm_->gridZone = zone;
            UtmToGps();
            relative_height_ = 0.0;
            altitude_ = 0.0;
            initialized_ = true;
        }

        Point::Point(const navigation::point::Point &P) {
            utm_ = new navigation::UtmPosition;
            gps_ = new navigation::GpsPosition;
            *utm_ = *P.utm_;
            *gps_ = *P.gps_;
            initialized_ = P.initialized_;
            relative_height_ = P.relative_height_;
            altitude_ = P.altitude_;
        }

        Point::Point(navigation::point::Point &&P) noexcept {
            utm_ = P.utm_;
            gps_ = P.gps_;
            P.utm_ = nullptr;
            P.gps_ = nullptr;
            initialized_ = P.initialized_;
            P.initialized_ = false;
            relative_height_ = P.relative_height_;
            altitude_ = P.altitude_;
        }

        UtmPosition Point::Utm() const {
            return *utm_;
        }

         GpsPosition Point::Gps() const {
            return *gps_;
        }

        double Point::Height() const {
            return relative_height_;
        }

        double Point::Height(double h) {
            relative_height_ = h;
            return relative_height_;
        }

        double Point::Altitude() const {
            return altitude_;
        }

        double Point::Altitude(double altitude) {
            altitude_ = altitude;
            return altitude_;
        }

        bool Point::Initialized() const {
            return initialized_;
        }

        Point& Point::operator=(const Point &P) {
            *gps_ = P.Gps();
            *utm_ = P.Utm();
            relative_height_ = P.relative_height_;
            initialized_ = P.Initialized();
            //std::cout<<"operator ="<<std::endl;
            return *this;
        }

        Point& Point::operator=(const UtmPosition &utmPosition) {
            *utm_ = utmPosition;
            UtmToGps();
            relative_height_ = 0.0;
            initialized_ = true;
            return *this;
        }

        Point& Point::operator=(const GpsPosition& gpsPosition) {
            *gps_ = gpsPosition;
            GpsToUtm();
            relative_height_ = 0.0;
            initialized_ = true;
            return *this;
        }

        Point& Point::operator()(double longitude, double latitude, AngleUnit u) {
            if(u & kDegree){
                gps_->latitude = latitude/180*M_PI;
                gps_->longitude = longitude/180*M_PI;
            } else {
                gps_->latitude = latitude;
                gps_->longitude = longitude;
            }
            GpsToUtm();
            initialized_ = true;
            return *this;
        }

        Point& Point::operator()(double x, double y, GridZone zone, Hemisphere hemisphere) {
            utm_->x = x;
            utm_->y = y;
            UtmToGps();
            initialized_ = true;
            return *this;
        }

        std::ostream &operator<<(std::ostream &output, const Point &P) {
            output<<"gps longitude: "<<std::fixed << std::setprecision(7)<<P.gps_->longitude*180/M_PI<<" latitude: "<<P.gps_->latitude*180/M_PI<<std::endl;
            output<<"utm x: "<<P.utm_->x<<" y: "<<P.utm_->x<<" grid zone: "<<P.utm_->gridZone<<" hemisphere: "<<P.utm_->hemisphere<<std::endl;
            output<<"altitude: "<<P.altitude_;
            return output;
        }

        bool Point::operator==(const navigation::point::Point &P) {
            if(!initialized_ || ! P.initialized_){
                throw Uninitialized();
            }
            return fabs(P.utm_->x - utm_->x) < 0.000001 &&
                   fabs(P.utm_->y - utm_->y) < 0.000001 &&
                   fabs(altitude_ - P.altitude_) < 0.000001;
        }

        void Point::GpsToUtm() {
            double lat = gps_->latitude;
            double lon = gps_->longitude;
            const Ellipse* e = standard_ellipse(ELLIPSE_WGS84);
            utm_->gridZone = GRID_AUTO;
            geographic_to_grid(e->a, e->e2, lat, lon, &utm_->gridZone, &utm_->hemisphere, &utm_->y, &utm_->x);
        }

        void Point::UtmToGps() {
            double lat;
            double lon;
            const Ellipse* e = standard_ellipse(ELLIPSE_WGS84);
            grid_to_geographic(e->a, e->e2, utm_->gridZone, utm_->hemisphere, utm_->y, utm_->x, &lat, &lon);
            gps_->latitude = lat;
            gps_->longitude = lon;
        }

        Point &Point::operator()(const Point &P) {
            utm_ = P.utm_;
            UtmToGps();
            initialized_ = P.initialized_;
            return *this;
        }

        double Point::Angle(const navigation::point::Point &A, const Point &B) {
            if(!A.initialized_ || ! B.initialized_){
                throw Uninitialized();
            }
            double angle;
            angle = atan2(B.utm_->y-A.utm_->y, B.utm_->x-A.utm_->x);
            if(angle<0){
                angle += 2*M_PI;
            }
            return angle;
        }

        double Point::Angle(const navigation::point::Point &B) {
            if(!initialized_ || ! B.initialized_){
                throw Uninitialized();
            }
            double angle;
            angle = atan2(B.utm_->y-utm_->y, B.utm_->x-utm_->x);
            if(angle<0){
                angle += 2*M_PI;
            }
            return angle;
        }

        Point::~Point(){
            delete utm_;
            delete gps_;
        }

        double Point::Distance(const navigation::point::Point &point1, const Point &point2) {
            if(!point1.initialized_ || ! point2.initialized_){
                throw Uninitialized();
            }
            if(point1.utm_->gridZone == point2.utm_->gridZone){
                double a,b,c;
                a = point1.utm_->x - point2.utm_->x;
                b = point1.utm_->y - point2.utm_->y;
                c = point1.altitude_ - point2.altitude_;
                return sqrt(a*a+b*b+c*c);
            }
            const StandardEllipse* e = _precompiled_ellipse(ELLIPSE_WGS84);
            double delta_lat = fabs(point1.gps_->latitude-point2.gps_->latitude);
            double delta_lon = fabs(point1.gps_->longitude-point2.gps_->longitude);
            if(delta_lat+delta_lon<0.01){
                double sin_delta_lat = sin(delta_lat/2);
                double sin_delta_lon = sin(delta_lon/2);
                double distance = e->a*2*asin(sqrt(sin_delta_lat*sin_delta_lat)+
                        cos(point1.gps_->latitude)*cos(point2.gps_->latitude)*sin_delta_lon*sin_delta_lon);
                return distance;
            }
            double distance = e->a * acos(sin(point1.gps_->latitude)*sin(point2.gps_->latitude) +
                    cos(point1.gps_->latitude)*cos(point2.gps_->latitude)*cos(delta_lon));
            return distance;
        }
    }
}