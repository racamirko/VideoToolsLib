#include "CDataSamplerImg.h"

#include <glog/logging.h>
#include <exception>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

CDataSamplerImg::CDataSamplerImg(string _srcFilename, Size _sampleSize, Size _sampleSpacing)
    : mSampleSize(_sampleSize)
    , mSampleSpacing(_sampleSpacing)
    , mRoi( mSampleSpacing.width, mSampleSpacing.height, mSampleSize.width, mSampleSize.height )
{
    mWholeSample = imread(_srcFilename);
    if( mWholeSample.empty() ){
        LOG(FATAL) << "No sample read from " << _srcFilename;
        throw exception();
    }
}

Mat CDataSamplerImg::getNextSample(){
    Mat curSample = mWholeSample(mRoi);
    mRoi.x += mSampleSize.width + mSampleSpacing.width;
    if( mRoi.x >= mWholeSample.cols-mSampleSpacing.width ){
        mRoi.x = mSampleSpacing.width;
        mRoi.y += mSampleSpacing.height + mSampleSize.height;
    }
    if( mRoi.y >= mWholeSample.rows-mSampleSpacing.height ){
        mRoi.x = mSampleSpacing.width;
        mRoi.y = mSampleSpacing.height;
    }
    return curSample;
}

CDSIIterator CDataSamplerImg::begin(){
    return CDSIIterator(this);
}

CDSIIterator CDataSamplerImg::end() const{
    return CDSIIterator(NULL);
}

// Iterator definition

CDSIIterator::CDSIIterator(CDataSamplerImg* _dataSrc)
    : mDataSrc(_dataSrc)
    , mAtEnd(false)
{
    if(_dataSrc == NULL )
        mAtEnd = true;
    else
        mRoi = Rect( _dataSrc->mSampleSpacing.width, _dataSrc->mSampleSpacing.height,
                     _dataSrc->mSampleSize.width, _dataSrc->mSampleSize.height );
}

cv::Mat CDSIIterator::operator*(){
    if( end() )
        return Mat();
    Mat origSample = mDataSrc->mWholeSample(mRoi);
    resize(origSample, origSample, mDataSrc->mSampleSize);
    return origSample;
}

bool CDSIIterator::operator!=(CDSIIterator& _otherIt) const {
    if( ( end() && _otherIt.end() ) || ( mRoi == _otherIt.mRoi && mDataSrc == _otherIt.mDataSrc ) )
        return false;
    return true;
}

CDSIIterator& CDSIIterator::operator++(){
    if( end() )
        return *this;
    // moving logic
    mRoi.x += mDataSrc->mSampleSize.width + mDataSrc->mSampleSpacing.width;
    // clip width if needed
    if( mRoi.x+mRoi.width > mDataSrc->mWholeSample.cols )
        mRoi.width = mDataSrc->mWholeSample.cols - mRoi.x;
    // goto new row of data
    if( mRoi.x >= mDataSrc->mWholeSample.cols-mDataSrc->mSampleSpacing.width ){
        mRoi.x = mDataSrc->mSampleSpacing.width;
        mRoi.y += mDataSrc->mSampleSpacing.height + mDataSrc->mSampleSize.height;
        if( mRoi.y + mRoi.height > mDataSrc->mWholeSample.rows )
            mRoi.height = mDataSrc->mWholeSample.rows - mRoi.y;
        mRoi.width = mDataSrc->mSampleSize.width;
    }
    if( mRoi.y >= mDataSrc->mWholeSample.rows-mDataSrc->mSampleSpacing.height ){
        mAtEnd = true;
    }
    // output
    return *this;
}

bool CDSIIterator::end() const {
    return mAtEnd;
}
