#ifndef IDATASAMPLER_H
#define IDATASAMPLER_H

#include <opencv2/core/core.hpp>

//class IDataSamplerIterator;

class IDataSampler {
public:
    virtual cv::Mat getNextSample() = 0;
//    virtual IDataSamplerIterator begin() const = 0;
//    virtual IDataSamplerIterator end() const = 0;
};

//class IDataSamplerIterator {
//public:
//    virtual cv::Mat operator*() = 0;
//    virtual bool operator!=(IDataSamplerIterator& _otherIt) const = 0;
//    virtual IDataSamplerIterator& operator++() = 0;
//};

#endif // IDATASAMPLER_H
