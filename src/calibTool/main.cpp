/*
 *  Software License Agreement (BSD License)
 *
 *  Copyright (c) 2013, Chili lab, EPFL.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Chili, EPFL nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  File:    main.cpp
 *  Author:  Mirko Raca <name.lastname@epfl.ch>
 *  Created: January 21, 2014
 */

#include <iostream>
#include <vector>
#include <string>
#include <exception>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <glog/logging.h>

using std::vector;
using std::string;
using std::cout; using std::endl;
using namespace cv;
namespace po = boost::program_options;

// input preparations
void getPointsFromVideoFile( string _calibVideo, Size _boardSize, vector<vector<Point2f>>& _imagePts, Size& _imgSize,
                             unsigned int _delayPeriod = 50, unsigned int _framesToCollect = 14,
                             bool _showImg = false );
void getPointsFromFolderImgs( string _folder, Size _boardSize, vector<vector<Point2f>>& _imagePts, Size& _imgSize,
                              unsigned int _framesToCollect = 14, bool _showImg = false );
void getPointsFromCam( int _camId, Size _boardSize, vector<vector<Point2f>>& _imagePts, Size& _imgSize,
                             unsigned int _delayPeriod = 50, unsigned int _framesToCollect = 14,
                             bool _showImg = false );

// main
void calibrateInternal(vector<vector<Point2f>>& _imagePts, Size _imgSize, Size _boardSize, float _squareSizeInMm, Mat& _cameraMatrix, Mat& _distCoef);
// helper functions
bool getPoints( Mat& _frame, Size _boardSize, vector<Point2f>& _pointbuf, bool _showImg );
void generateObjectPts(vector< vector<Point3f> > & _vec, Size _boardSize, float _squareSizeInMm, int _numOfSamples);
void parseArguments( int _argc, char* _argv[], string* _inSrc, string* _outFilename);
bool isNumber(const std::string& s);
void writeYaml(std::string _outFilename, cv::Mat& _camMat, cv::Mat& _distCoef);

int main(int argc, char* argv[]){
    google::InitGoogleLogging(argv[0]);
    string inSrc, outFilename;
    parseArguments(argc, argv, &inSrc, &outFilename);

    // process variables
    vector<vector<Point2f>> imagePts;
    Size boardSize = Size(6,7); // Size(6,7); Size(7,10);
    float checkerSquareSize = 26.0f; // mm
    Size imgSize; // received from the functions
    unsigned int numToCollect = 14;
    unsigned int delayPeriod = 50;
    bool verbose = true;

    // output variables
    Mat camMat, distCoef;

    // branch the input based on type
    if( isNumber(inSrc) ){ // cam
        getPointsFromCam( atoi(inSrc.c_str()), boardSize, imagePts, imgSize, delayPeriod, numToCollect, verbose );
    } else {
        if( boost::filesystem::is_directory(boost::filesystem::path(inSrc)) ){ // folder
            getPointsFromFolderImgs(inSrc, boardSize, imagePts, imgSize, numToCollect, verbose);
        } else { // video-file
            getPointsFromVideoFile( inSrc, boardSize, imagePts, imgSize, delayPeriod, numToCollect, verbose );
        }
    }

    // main process
    calibrateInternal(imagePts, imgSize, boardSize, checkerSquareSize, camMat, distCoef );

    // write output
    writeYaml(outFilename, camMat, distCoef);

    return 0;
}

void getPointsFromVideoFile( string _calibVideo, Size _boardSize, vector<vector<Point2f>>& _imagePts, Size& _imgSize,
                             unsigned int _delayPeriod, unsigned int _framesToCollect,
                             bool _showImg )
{
    Mat frame;
    vector<Point2f> pointbuf;
    int delayFrames = 0;

    _imagePts.clear();
    VideoCapture vc(_calibVideo);
    if( !vc.isOpened()){
        LOG(FATAL) << "Could not open file " << _calibVideo;
        return;
    }

    long frameCnt = vc.get(CV_CAP_PROP_FRAME_COUNT),
         frameNo = 0;
    _imgSize.width = vc.get(CV_CAP_PROP_FRAME_WIDTH);
    _imgSize.height = vc.get(CV_CAP_PROP_FRAME_HEIGHT);

    while( frameNo < frameCnt ){
        ++frameNo;
        vc >> frame;
        if( delayFrames > 0 ){
            delayFrames--;
            continue;
        }

        if( getPoints(frame, _boardSize, pointbuf, _showImg) ){
            _imagePts.push_back(pointbuf);
            if( _imagePts.size() < _framesToCollect )
                delayFrames = _delayPeriod;
            else
                break;
        }
    }
    destroyAllWindows();
    vc.release();
}

void getPointsFromFolderImgs( string _folder, Size _boardSize, vector<vector<Point2f>>& _imagePts, Size& _imgSize,
                             unsigned int _framesToCollect, bool _showImg )
{
    namespace fs = boost::filesystem;
    fs::directory_iterator end_iter;
    vector<Point2f> pointbuf;

    _imagePts.clear();
    if ( !fs::exists(_folder) || !fs::is_directory(_folder)){
        LOG(ERROR) << "Folder not found: " << _folder;
        return;
    }

    for( fs::directory_iterator dir_iter(_folder) ; dir_iter != end_iter ; ++dir_iter)
    {
        if (fs::is_regular_file(dir_iter->status()) ){ //&& !boost::filesystem::extension(*dir_iter).compare("jpg") )
            DLOG(INFO) << "Loading file for frame " << _framesToCollect << " : " << dir_iter->path().native();
            Mat tmpImg = imread(dir_iter->path().native());
            _imgSize.width = tmpImg.cols; _imgSize.height = tmpImg.rows;
            if( _framesToCollect > 0 ){
                if( getPoints(tmpImg, _boardSize, pointbuf, _showImg) ){
                    _imagePts.push_back(pointbuf);
                    _framesToCollect--;
                }
            }
        }
    }
    destroyAllWindows();
}

void getPointsFromCam( int _camId, Size _boardSize, vector<vector<Point2f>>& _imagePts, Size& _imgSize,
                             unsigned int _delayPeriod, unsigned int _framesToCollect,
                             bool _showImg)
{
    LOG(FATAL) << "getPointsFromCam not implemented";
    throw std::runtime_error("Not implemented");
    destroyAllWindows();
}


bool getPoints( Mat& _frame, Size _boardSize, vector<Point2f>& _pointbuf, bool _showImg ){
    _pointbuf.clear();
    if( _frame.channels() > 1 ){
        cvtColor(_frame, _frame, CV_BGR2GRAY);
    }
    if(_showImg){
        imshow("dbgwnd", _frame);
        waitKey(100);
    }
    bool found = findChessboardCorners( _frame, _boardSize, _pointbuf,
                        CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
    if(found){
        cornerSubPix( _frame, _pointbuf, Size(11,11), Size(-1,-1), TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
        // begin display debug
        if(_showImg) {
            drawChessboardCorners(_frame, _boardSize, Mat(_pointbuf), found);
            imshow("dbgwnd", _frame);
            waitKey(2000);
        }
        // end debug
        DLOG(INFO) << "Found dots";
        // store points
    } else {
        DLOG(INFO) << "Dots not found";
    }
    return found;
}

void calibrateInternal(vector<vector<Point2f>>& _imagePts, Size _imgSize, Size _boardSize, float _squareSizeInMm, Mat& _cameraMatrix, Mat& _distCoef){
    vector<Mat> rvecs; // don't really care for this, too innacurate for now
    vector<Mat> tvecs;
    vector<vector<Point3f>> objectPts;
    generateObjectPts(objectPts, _boardSize, _squareSizeInMm, _imagePts.size());

    double rms = calibrateCamera(objectPts, _imagePts, _imgSize, _cameraMatrix, _distCoef, rvecs, tvecs, CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);
    LOG(INFO) << "RMS error reported by calibrateCamera: " << rms;

    // save calibration
    LOG(INFO) << "Camera matrix:";
    LOG(INFO) << _cameraMatrix.at<double>(0,0) << ", " << _cameraMatrix.at<double>(0,1) << ", " << _cameraMatrix.at<double>(0,2);
    LOG(INFO) << _cameraMatrix.at<double>(1,0) << ", " << _cameraMatrix.at<double>(1,1) << ", " << _cameraMatrix.at<double>(1,2);
    LOG(INFO) << _cameraMatrix.at<double>(2,0) << ", " << _cameraMatrix.at<double>(2,1) << ", " << _cameraMatrix.at<double>(2,2);
    LOG(INFO) << "Distorsion coefficients: ";
    for( int i = 0; i < 8; ++i )
        LOG(INFO) << "\t" << _distCoef.at<double>(i);
}

void generateObjectPts(vector< vector<Point3f> > & _vec, Size _boardSize, float _squareSizeInMm, int _numOfSamples){
    _vec.clear();
    vector<Point3f> tmpVec;
    for( int i = 0; i < _boardSize.height; i++ )
        for( int j = 0; j < _boardSize.width; j++ )
            tmpVec.push_back(Point3f(float(j*_squareSizeInMm), float(i*_squareSizeInMm), 0));
    for( int i = 0; i < _numOfSamples; ++i )
        _vec.push_back(tmpVec);
}

/*
 *  calibTool intputSrc outputFilename.xml
 *
 */
void parseArguments( int _argc, char* _argv[], // input arguments
                     string* _inSrc,
                     string* _outFilename)
{
    po::options_description opts("Program parameters");
    opts.add_options()
            ("help,h","produce help message")
            ("input,i", po::value<string>(_inSrc), "Input source. Can be either: videoFile, folder of pictures or camera id")
            ("output,o", po::value<string>(_outFilename),"Output file");

    po::positional_options_description p;
    p.add("input", 1);
    p.add("output", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(_argc, _argv).options(opts).positional(p).run(), vm);
    po::notify(vm);

    if( vm.count("help") ){
        cout << opts << endl;
        exit(0);
    }

    if( vm.count("input") == 0 ){
        std::cerr << "No input source specified. Check --help for usage" << endl;
//        LOG(FATAL) << "No source specified.";
        exit(-1);
    }

    if( vm.count("output") == 0 ){
        std::cerr << "No output filename specified. Check --help for usage" << endl;
//        LOG(FATAL) << "No output filename specified.";
        exit(-2);
    }
}

/*
 *  (c) Charles Salvia (at StackOverflow)
 */
bool isNumber(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(),
        s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

void writeYaml(std::string _outFilename, cv::Mat& _camMat, cv::Mat& _distCoef){
    FileStorage fs(_outFilename, FileStorage::WRITE);
    fs << "cameraMatrix" << _camMat << "distCoeffs" << _distCoef;
    fs.release();
}
