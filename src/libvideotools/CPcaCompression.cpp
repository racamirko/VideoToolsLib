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
 *  File:    CPcaCompression.cpp
 *  Author:  Mirko Raca <name.lastname@epfl.ch>
 *  Created: January 10, 2014.
 */

#include "../../include/CPcaCompression.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "globalInclude.h"
#include "../../include/CDataUtils.h"

using namespace cv;

CPcaCompression::CPcaCompression(int _numOfComponents, int _numOfSamples, cv::Size _sampleSize)
    : mLastSample(0)
    , mNumOfPcaComponents(_numOfComponents)
    , mSampleSize(_sampleSize)
{
    init(_numOfSamples, _sampleSize);
}

void CPcaCompression::init(int _numOfSamples, cv::Size _sampleSize){
    DLOG(INFO) << "init()";
    mBigSampleMat = Mat( _numOfSamples, _sampleSize.height*_sampleSize.width, CV_32F );
    mLastSample = 0;
}

void CPcaCompression::addSample(cv::Mat _sample){
    DLOG(INFO) << "addSample()";
    if( mLastSample >= mBigSampleMat.rows ){
        LOG(WARNING) << "Samples already collected. New samples rejected";
        return;
    }
    checkAndPrepareSample(_sample);
    CDataUtils::packRow(mBigSampleMat, mLastSample++, _sample);
}

double CPcaCompression::process(bool _withDisplay){
    DLOG(INFO) << "process()";
    mBigSampleMat = mBigSampleMat(Range(0,mLastSample), Range::all());
    mPca = PCA(mBigSampleMat, Mat(), CV_PCA_DATA_AS_ROW, mNumOfPcaComponents);
    double score = testCompressionPrecission(_withDisplay);
    dropTraining();
    return score;
}

cv::Mat CPcaCompression::compress(cv::Mat _sample){
    DLOG(INFO) << "compress()";
    checkAndPrepareSample(_sample);
    if( _sample.rows > 1 )
        _sample = _sample.reshape(0, 1);
    Mat compressed;
    mPca.project(_sample, compressed);
    return compressed;
}

cv::Mat CPcaCompression::decompress(cv::Mat _compressedSample, bool _convertTo8Bit){
    DLOG(INFO) << "decompress()";
    if( _compressedSample.rows > 1 ){
        DLOG(INFO) << "Invalid compressed sample, rows > 1 (" << _compressedSample.rows  << ")";
        throw Exception();
    }
    if( _compressedSample.type() != CV_32F ){
        DLOG(WARNING) << "Invalid compressed sample, type not CV_32F  (" << _compressedSample.type()  << ")";
        _compressedSample.convertTo(_compressedSample, CV_32F);
    }
    Mat decompressed;
    mPca.backProject(_compressedSample, decompressed);
    decompressed = decompressed.reshape(0, mSampleSize.height);
    normalize(decompressed, decompressed, 0, 255, NORM_MINMAX);
    if( _convertTo8Bit )
        decompressed.convertTo(decompressed, CV_8U);
    return decompressed;
}

void CPcaCompression::dropTraining(){
    DLOG(INFO) << "dropTraining";
    mBigSampleMat.release();
}

void CPcaCompression::checkAndPrepareSample(cv::Mat& _sample){
    DLOG(INFO) << "checkAndPrepareSample";
    if( _sample.channels() > 1 ){
        DLOG(INFO) << "\tConverting to grayscale";
        cvtColor(_sample, _sample, CV_BGR2GRAY);
    }
    if( _sample.type() != CV_32F ){
        DLOG(INFO) << "\tConverting to float matrix";
        _sample.convertTo(_sample, CV_32F );
    }
    // normalise
    normalize(_sample, _sample, 1.0);
}

cv::Mat CPcaCompression::getPCAMean(bool _asImage){
    DLOG(INFO) << "getPCAMean()";
    Mat tmpRes = mPca.mean.clone();
    tmpRes = tmpRes.reshape(0, mSampleSize.height);
    if( _asImage )
        tmpRes.convertTo(tmpRes, CV_8U);
    return tmpRes;
}

void CPcaCompression::getEigenVectors(std::vector<cv::Mat> _outVec, bool _asImage){
    DLOG(INFO) << "getEigenVectors()";
    _outVec.clear();
    int tmpLimit = mNumOfPcaComponents;
    if( tmpLimit == 0 )
        tmpLimit = mPca.eigenvectors.rows;
    for( int i = 0; i < tmpLimit; ++i  ){
        Mat unpackMat = CDataUtils::unpackRow(mPca.eigenvectors, i, mSampleSize);
        if( _asImage )
            unpackMat.convertTo(unpackMat, CV_8U);
        _outVec.push_back(unpackMat);
    }
}

double CPcaCompression::testCompressionPrecission(bool _withDisplay){
    DLOG(INFO) << "testCompressionPrecission()";
    if( mBigSampleMat.rows == 0 || mBigSampleMat.cols == 0){
        LOG(ERROR) << "No test samples!";
        throw Exception();
    }
    double score = 0;
    for( int sampleNo = 0; sampleNo < mLastSample; ++sampleNo ){
        Mat origSample = CDataUtils::unpackRow(mBigSampleMat, sampleNo, mSampleSize);
        normalize(origSample, origSample, 0, 255, NORM_MINMAX);
        Mat tmpOutput;
        origSample.convertTo(tmpOutput, CV_8U);
        Mat compressed = compress(origSample);
        Mat back = decompress(compressed);
        Mat diff = origSample-back;
        double sampleDiff = cv::sum(cv::abs(diff))[0];
        DLOG(INFO) << "Sample #" << sampleNo << " diff = " << sampleDiff;
        score += sampleDiff; // it should have only 1 channel
        if( _withDisplay ){
            origSample.convertTo(origSample, CV_8U);
            back.convertTo(back, CV_8U);
            // display
            imshow("originaWnd", tmpOutput);
            imshow("backWnd", back);
            waitKey(200);
        }
    }
    return score;
}

int CPcaCompression::getNumOfComponents(){
    DLOG(INFO) << "getNumOfPcaComponents()";
    return mPca.eigenvectors.rows;
}
