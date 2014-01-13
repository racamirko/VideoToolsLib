#include "../../include/CXNormSize.h"

#include <opencv2/imgproc/imgproc.hpp>
#include "globalInclude.h"

CXNormSize::CXNormSize(cv::Size _stdSize)
    : mStdSize(_stdSize)
{
    DLOG(INFO) << "Xfmr::NormSize = (" << mStdSize.width << ", " << mStdSize.height << ")";
}

CXNormSize::CXNormSize(int _width, int _height)
    :mStdSize(_width, _height)
{ }


cv::Mat CXNormSize::transform(cv::Mat& _input) const {
    DLOG(INFO) << "CXNormSize::transform";
    cv::Mat rez;
    cv::resize(_input, rez, mStdSize, 0, 0, cv::INTER_CUBIC);
    return IXfmr::transform(rez);
}
