/*
 *
 *  Software License Agreement (BSD License)
 *
 *  Copyright (c) 2013, Chili lab, EPFL.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Chili, EPFL nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  File:    CVecUtils.cpp
 *  Author:  Mirko Raca <name.lastname@epfl.ch>
 *  Created: January 21, 2014.
 */

#include "../../include/CVecUtils.h"

#include <math.h>
#include <iostream>
//#include "CProbUtils.h"

#include "globalInclude.h"

using namespace cv;

double CVecUtils::direction(cv::Point2f _startPt, cv::Point2f _endPt){
    /**
      * Important to remember, quadrants go like this:
      *
      *         2*pi | 1.5*pi
      *         -------------
      *       0.5*pi | pi
      */
    return atan2(_endPt.y-_startPt.y, _endPt.x-_startPt.x);
}

double CVecUtils::angleDiff(cv::Point2f& p1, cv::Point2f& q1, cv::Point2f& p2, cv::Point2f& q2){
    return angleDiff( direction(p1, q1), direction(p2, q2) );
}

double CVecUtils::angleDiff(double a1, double a2){
//    DLOG(INFO) << "Angle diff between: " << a1 << " and " << a2;
    double meas1 = std::abs(a1-a2);
//    DLOG(INFO) << "\tm1 = "  << meas1;
    if( a1<a2 )
        a1 += M_PI*2;
    else
        a2 += M_PI*2;
    double meas2 = std::abs(a1-a2);
//    DLOG(INFO) << "\tm2 = " << meas2;
    double result = min(meas1, meas2);
//    DLOG(INFO) << "\tReturned: " << result << " which is: " << result/(2*M_PI)*360.0 << " degs";
    return result;
}

double CVecUtils::dist(cv::Point2f& p, cv::Point2f& q){
    return sqrt( std::pow( p.x-q.x,2.0 ) + std::pow( p.y-q.y ,2.0 ) );
}

double CVecUtils::getAvgDirection( std::vector<Point2f>& _vecPointsStart, std::vector<Point2f>& _vecPointEnd ){
    float totalX = 0, totalY = 0;
    for( unsigned int i = 0; i < _vecPointsStart.size(); ++i ){
        Point2f p = _vecPointsStart[i],
                q = _vecPointEnd[i];
        totalX += q.x - p.x;
        totalY += q.y - p.y;
    }
    return direction(Point2f(0,0), Point2f(totalX, totalY));
}

cv::Point2f CVecUtils::getRectCenter( cv::Rect _rect ){
    Point2f centerPt;
    centerPt.x = ((double)_rect.x)+((double)_rect.width)/2.0;
    centerPt.y = ((double)_rect.y)+((double)_rect.height)/2.0;
    return centerPt;
}

cv::Point2f CVecUtils::getRectOffsetedCenter( cv::Rect _rect, double _xperc, double _yperc ){
    Point2f centerPt;
    centerPt.x = ((double)_rect.x)+((double)_rect.width)*_xperc;
    centerPt.y = ((double)_rect.y)+((double)_rect.height)*_yperc;
    return centerPt;
}

double CVecUtils::getRectOuterRadius( cv::Rect _rect ){
    double w = _rect.width,
           h = _rect.height;
    return std::pow( std::pow(w/2.0, 2.0) + std::pow(h/2.0, 2.0), 0.5 );
}

std::string CVecUtils::rectToString( cv::Rect _rect ){
    char buffer[100];
    sprintf( buffer, "[%d, %d, %d, %d]", _rect.x, _rect.y, _rect.x+_rect.width, _rect.y+_rect.height );
    return string(buffer);
}
