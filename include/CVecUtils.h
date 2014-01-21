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
 *  File:    CVecUtils.h
 *  Author:  Mirko Raca <name.lastname@epfl.ch>
 *  Created: January 21, 2014.
 */

#ifndef CUTILS_H
#define CUTILS_H

#include <opencv2/core/core.hpp>
#include "libvideotools_global.h"

class LIBVIDEOTOOLSSHARED_EXPORT CVecUtils
{
public:
    CVecUtils() {}

    static double direction(cv::Point2f _startPt, cv::Point2f _endPt);
    static double dist(cv::Point2f& p, cv::Point2f& q);
    static double angleDiff(cv::Point2f& p1, cv::Point2f& q1, cv::Point2f& p2, cv::Point2f& q2);
    static double angleDiff(double a1, double a2);
    static cv::Point2f getRectCenter( cv::Rect _rect );
    static cv::Point2f getRectOffsetedCenter( cv::Rect _rect, double _xperc, double _yperc );
    static double getRectOuterRadius( cv::Rect _rect );
    static std::string rectToString( cv::Rect _rect );

    double getAvgDirection( std::vector<cv::Point2f>& _vecPointsStart, std::vector<cv::Point2f>& _vecPointEnd );
};

#endif // CUTILS_H
