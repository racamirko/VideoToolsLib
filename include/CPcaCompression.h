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
 *  File:    CPcaCompression.h
 *  Author:  Mirko Raca <name.lastname@epfl.ch>
 *  Created: January 10, 2014.
 */
#ifndef CPCACOMPRESSION_H
#define CPCACOMPRESSION_H

#include <opencv2/core/core.hpp>
#include <vector>

#include "../../include/CCompressorInterface.h"

class CPcaCompression : public CCompressorInterface
{
protected:
    cv::Mat mBigSampleMat;
    int mLastSample;
    int mNumOfPcaComponents;
    cv::Size mSampleSize;
    cv::PCA mPca;

    void init(int _numOfSamples, cv::Size _sampleSize);
    double testCompressionPrecission(bool _withDisplay = false);
    void dropTraining();
    void checkAndPrepareSample(cv::Mat& _sample);
public:
    CPcaCompression(int _numOfComponents, int _numOfSamples, cv::Size _sampleSize);
    // training related methods
    void addSample(cv::Mat _sample);
    double process(bool _withDisplay = false);
    // main interface
    cv::Mat compress(cv::Mat _sample);
    cv::Mat decompress(cv::Mat _compressedSample, bool _convertTo8Bit = false);
    // information & testing
    cv::Mat getPCAMean(bool _asImage = false);
    void getEigenVectors(std::vector<cv::Mat> _outVec, bool _asImage = false);
    int getNumOfComponents();
};

#endif // CPCACOMPRESSION_H
