#ifndef FILTERNMS_H
#define FILTERNMS_H

#include "../libvideotools_global.h"
#include "IGenericPostFilter.h"
#include <vector>
#include <utility>
#include <opencv2/core/core.hpp>

class LIBVIDEOTOOLSSHARED_EXPORT CFilterNMS : public IGenericPostFilter {
protected:
    double mOverlapThresh;
public:
    CFilterNMS(double _overlapThresh);

    void process(tVecDetections& _candidates) const;
};

#endif // FILTERNMS_H
