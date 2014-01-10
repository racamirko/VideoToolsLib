#ifndef CDATASAMPLERIMG_H
#define CDATASAMPLERIMG_H

#include "IDataSampler.h"
#include <string>

class CDataSamplerImg : public IDataSampler
{
protected:
    cv::Mat mWholeSample;
    cv::Size mSampleSize, mSampleSpacing;
    cv::Rect mCurSample;

public:
    CDataSamplerImg(std::string _srcFilename, cv::Rect _sampleSize, cv::Size _sampleSpacing);

    cv::Mat getNextSample();
};

#endif // CDATASAMPLERIMG_H
