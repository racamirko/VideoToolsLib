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
 *  File:    CDataUtils.cpp
 *  Author:  Mirko Raca <name.lastname@epfl.ch>
 *  Created: January 8, 2014.
 */

#include <../../include/CDataUtils.h>

using namespace cv;

#include "globalInclude.h"

void CDataUtils::packRow(cv::Mat& _bigMat, int _rowNo, cv::Mat& _newData){
    if( !_newData.isContinuous() )
        _newData = _newData.clone();
    if( _bigMat.type() != CV_32F ){
        LOG(ERROR) << "Wrong type of the bigMat " << _bigMat.type() << "(should be " << CV_32F << ")";
        throw Exception();
    }
    if( _newData.type() != CV_32F ){
        LOG(ERROR) << "Wrong type of the _newData " << _newData.type() << "(should be " << CV_32F << ")";
        throw Exception();
    }
    if( _rowNo >= _bigMat.rows ){
        LOG(ERROR) << "Row number too big, mat size: " << _bigMat.rows << " , attempted to access row #" << _rowNo;
        throw Exception();
    }
    Mat rowData = _newData.reshape(0, 1);
    for( int col = 0; col < _bigMat.cols; ++col ){
        _bigMat.at<float>(_rowNo, col) = rowData.at<float>(0, col);
    }
}

Mat CDataUtils::unpackRow(cv::Mat& _bigMat, int _rowNo, cv::Size _outSize){
    DLOG(INFO) << "unpackRow method";
    if( _bigMat.type() != CV_32F ){
        LOG(ERROR) << "Wrong type of the bigMat " << _bigMat.type() << "(should be " << CV_32F << ")";
        throw Exception();
    }
    Mat outMat(1,_outSize.height* _outSize.width, CV_32F);
    for( int colCnt = 0; colCnt < _bigMat.cols; ++colCnt ){
        outMat.at<float>(0,colCnt) = _bigMat.at<float>(_rowNo, colCnt);
    }
    outMat = outMat.reshape(0, _outSize.height);
    return outMat;
}

bool CDataUtils::eq(const cv::Mat& _mat1, const cv::Mat& _mat2){
    Mat diff = (_mat1 != _mat2);
    return ( countNonZero(diff) == 0 );
}
