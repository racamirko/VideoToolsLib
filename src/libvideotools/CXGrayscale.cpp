#include "../../include/imgTransformers/CXGrayscale.h"

#include <opencv2/imgproc/imgproc.hpp>
#include "globalInclude.h"

cv::Mat CXGrayscale::transform(cv::Mat& _input) const {
    DLOG(INFO) << "CXGrayscale::trasform";
    cv::Mat rez;
    cv::cvtColor(_input, rez, CV_BGR2GRAY);
    return IXfmr::transform(rez);
}
