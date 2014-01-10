#ifndef IDATASAMPLER_H
#define IDATASAMPLER_H

#include <opencv2/core/core.hpp>

class IDataSampler {
public:
    virtual cv::Mat getNextSample() = 0;
};

#endif // IDATASAMPLER_H
