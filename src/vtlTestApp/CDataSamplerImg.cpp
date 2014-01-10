#include "CDataSamplerImg.h"

#include <glog/logging.h>
#include <exception>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

CDataSamplerImg::CDataSamplerImg(string _srcFilename, Size _sampleSize, Size _sampleSpacing)
    : mSampleSize(_sampleSize)
    , mSampleSpacing(_sampleSpacing)
    , mCurSample( mSampleSpacing.width, mSampleSpacing.height, mSampleSize.width, mSampleSize.height )
{
    mWholeSample = imread(_srcFilename);
    if( mWholeSample.empty() ){
        LOG(FATAL) << "No sample read from " << _srcFilename;
        throw exception();
    }
}

Mat CDataSamplerImg::getNextSample(){
    Mat curSample = mWholeSample(mCurSample);
    // TODO: stopped here, logic for moving the sample location
    return Mat();
}
