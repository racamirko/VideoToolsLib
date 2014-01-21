#ifndef CDATASAMPLERIMG_H
#define CDATASAMPLERIMG_H

#include "IDataSampler.h"
#include <string>

class CDSIIterator;

class CDataSamplerImg : public IDataSampler
{
    friend class CDSIIterator;
protected:
    cv::Mat mWholeSample;
    cv::Size mSampleSize, mSampleSpacing;
    cv::Rect mRoi;

public:
    CDataSamplerImg(std::string _srcFilename, cv::Size _sampleSize, cv::Size _sampleSpacing);

    cv::Mat getNextSample();

    CDSIIterator begin();
    CDSIIterator end() const;
};

class CDSIIterator {
protected:
    CDataSamplerImg* mDataSrc;
    cv::Rect mRoi;
    bool mAtEnd;

    bool end() const;
public:
    CDSIIterator(CDataSamplerImg* _dataSrc);

    cv::Mat operator*();
    bool operator!=(CDSIIterator& _otherIt) const;
    CDSIIterator& operator++();

};

#endif // CDATASAMPLERIMG_H
