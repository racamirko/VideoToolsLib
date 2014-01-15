#include "../../include/filters/CFilterNMS.h"

#include <list>
#include "globalInclude.h"

typedef std::pair<float, int> tOrderPair;
bool compareSortOrder(const tOrderPair& _l, const tOrderPair& _r);
void calculateOverlapMatrix( tVecDetections& _candidates, cv::Mat &_overlapMat );
float calcOverlap(const cv::Rect _r1, const cv::Rect _r2);
std::string rect2str(const cv::Rect _r);

CFilterNMS::CFilterNMS(double _overlapThresh)
    : mOverlapThresh(_overlapThresh)
{
    if(mOverlapThresh > 1.0f)
        mOverlapThresh /= 100.0f;
    DLOG(INFO) << "CFIlterNMS(" << mOverlapThresh << ") created";
}

/*
  Sorting trick found here
        http://stackoverflow.com/questions/1577475/c-sorting-and-keeping-track-of-indexes
*/
void CFilterNMS::process(tVecDetections &_candidates) const {
    // sort all candidates in descending order based on the result
    tVecDetections orderedCandidates;
    {
        std::list<tDetectionPair> orderedCandidatesList; // just to control the memory consumption a bit. The result ends in orderedCandidates
        std::vector<tOrderPair> resortBuffer; // <score, original_index>
        int idx = 0;
        for( const tDetectionPair& cand : _candidates )
            resortBuffer.push_back( tOrderPair(cand.second, idx++) );
        std::sort(resortBuffer.begin(), resortBuffer.end(), compareSortOrder);
        for( const tOrderPair& elem : resortBuffer )
            orderedCandidatesList.push_back( _candidates.at(elem.second) );
        std::copy(orderedCandidatesList.begin(), orderedCandidatesList.end(), std::back_inserter(orderedCandidates));
    }
    // create overlap matrix
    cv::Mat overlapMat = cv::Mat::zeros(orderedCandidates.size(), orderedCandidates.size(), CV_32FC1);
    calculateOverlapMatrix(orderedCandidates, overlapMat);
    // interate and insert only the top-scoring
    std::list<tDetectionPair> outputResults;
    {
        for(unsigned int idx = 0; idx < orderedCandidates.size(); ++idx){
            cv::Mat curRow = overlapMat.row(idx);
            DLOG(INFO) << "Cur row:";
            DLOG(INFO) << curRow;
            cv::Mat boolSelection = curRow > mOverlapThresh; // this is outputing 255 instead of "1" for true conditions. Not really a problem, but weird
            DLOG(INFO) << "Cur binary selection";
            DLOG(INFO) << boolSelection;
            if( cv::sum(boolSelection).val[0] == 0 )
                outputResults.push_back(orderedCandidates[idx]);
        }
    }
    // re-insert the top-scoring candidates in the output vector
    _candidates.clear();
    std::copy(outputResults.begin(), outputResults.end(), std::back_inserter(_candidates));
}

// made for descending sort
bool compareSortOrder(const tOrderPair& _l, const tOrderPair& _r){
    return _l.first > _r.first;
}

/*
 *  Important to note that the overlapMat encodes the information
 *      overlap.at<float>(x,y) = (x & y)/x;
 *  The matrix will be 0.0 for any idx <= of the current outer idx
 */
void calculateOverlapMatrix( tVecDetections& _candidates, cv::Mat &_overlapMat ){
    int outerIdx = 0;
    for( const tDetectionPair& _outerC : _candidates ){
        int innerIdx = 0;
        for( const tDetectionPair& _innerC : _candidates ){
            if(innerIdx >= outerIdx){
                break;
            }
            _overlapMat.at<float>(outerIdx, innerIdx) = calcOverlap(_outerC.first, _innerC.first);
            ++innerIdx;
        }
        ++outerIdx;
    }
    DLOG(INFO) << "Overlap matrix: " << _overlapMat;
}

/*
 * Overlap percentage from the "viewpoint" of the first rectangle.
 *  (meaning, the intersection coverage area is devided by the area of the
 *  of the first rectangle.
 */
float calcOverlap(const cv::Rect _r1, const cv::Rect _r2){
    DLOG(INFO) << "Overlap between " << rect2str(_r1) << " and " << rect2str(_r2);
    cv::Rect intersection = _r1 & _r2;
    DLOG(INFO) << "    is " << rect2str(intersection);
    float overlapPct = (float)intersection.area()/(float)_r1.area();
    DLOG(INFO) << "    in the pct of the first rect: " << overlapPct;
    return overlapPct;
}

std::string rect2str(const cv::Rect _r){
    char buffer[100];
    sprintf(buffer, "[%d, %d, %d, %d]", _r.x, _r.y, _r.width, _r.height);
    return std::string(buffer);
}
