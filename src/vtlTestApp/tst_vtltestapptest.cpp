#include <QString>
#include <QtTest>

#include <glog/logging.h>
#include <CDataUtils.h>
#include <string>
#include <imgTransformers/all.h>
#include <CPcaCompression.h>
#include <filters/CFilterNMS.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include "CDataSamplerImg.h"

#include "externalParams.h"

using namespace cv;

class VtlTestAppTest : public QObject
{
    Q_OBJECT
    
public:
    VtlTestAppTest();
    
private Q_SLOTS:

    void testCDataUtils_packRow();
    void testCDataUtils_unpackRow();
    void testCDataUtils_eq();
    void testSampler_test1();
    // pipeline tests
    void testNormImgsPipeline();
    void testCropImgsPipeline();
    // PCA compression
    void testPcaCompressionPrecision();
    // filter testss
    void testPostNMS();
};

VtlTestAppTest::VtlTestAppTest()
{
    google::InitGoogleLogging("VtlTestAppTest");
}


void VtlTestAppTest::testCDataUtils_packRow(){
    Mat bigMat = Mat::zeros(1, 9, CV_32F),
        testInputMat = ( Mat_<float>(3,3,CV_32F) <<  1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f),
        correctMat = ( Mat_<float>(1,9,CV_32F) <<  1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f );
    CDataUtils::packRow(bigMat, 0, testInputMat);

    QVERIFY( CDataUtils::eq(correctMat, bigMat) );
}

void VtlTestAppTest::testCDataUtils_unpackRow(){
    Mat correctMat = ( Mat_<float>(3,3,CV_32F) <<  1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f),
        testInputMat = ( Mat_<float>(1,9,CV_32F) <<  1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f );
    Mat rezMat = CDataUtils::unpackRow(testInputMat, 0, Size(3,3));

    QVERIFY( CDataUtils::eq(rezMat, correctMat) );
}

void VtlTestAppTest::testCDataUtils_eq(){
    Mat mat1 = ( Mat_<float>(3,3,CV_32F) <<  1.0f, 2.0f, 3.0f, 4.0f),
        mat2 = ( Mat_<float>(3,3,CV_32F) <<  1.0f, 2.0f, 3.0f, 4.0f),
        mat3 = ( Mat_<float>(3,3,CV_32F) <<  2.0f, 2.0f, 3.0f, 4.0f);
    QVERIFY( CDataUtils::eq(mat1, mat2) == true );
    QVERIFY( CDataUtils::eq(mat1, mat3) == false );
}

/* passed, left for usage example */
void VtlTestAppTest::testSampler_test1(){
    CDataSamplerImg sampler = CDataSamplerImg(IMG_SAMPLES_GRID_FILENAME,  IMG_SAMPLES_GRID_SIZE, IMG_SAMPLES_GRID_SPACING );
    for( Mat sample : sampler ){
        imshow("testwnd",sample);
        waitKey(1);
    }
}

void VtlTestAppTest::testNormImgsPipeline(){
    CDataSamplerImg sampler = CDataSamplerImg(IMG_SAMPLES_GRID_FILENAME,  IMG_SAMPLES_GRID_SIZE, IMG_SAMPLES_GRID_SPACING );
    // create pipeline
    IXfmr pipeStart;
    pipeStart.add(new CXNormSize(Size(100, 100))).add(new CXGrayscale());
    // run test
    for(Mat sample : sampler){
        Mat outSample = pipeStart.transform(sample);
        imshow("testwnd", outSample);
        waitKey(1);
    }
}

void VtlTestAppTest::testCropImgsPipeline(){
    CDataSamplerImg sampler = CDataSamplerImg(IMG_SAMPLES_GRID_FILENAME,  IMG_SAMPLES_GRID_SIZE, IMG_SAMPLES_GRID_SPACING );
    // create pipeline
    CXCrop cropper(Rect(100, 100, 100, 100));
    // run test
    for(Mat sample : sampler){
        Mat outSample = cropper.transform(sample);
        imshow("testwnd", outSample);
        waitKey(1);
    }
}

void VtlTestAppTest::testPcaCompressionPrecision(){
    CDataSamplerImg sampler = CDataSamplerImg(IMG_SAMPLES_GRID_FILENAME,  IMG_SAMPLES_GRID_SIZE, IMG_SAMPLES_GRID_SPACING );
    IXfmr pipeStart;
    pipeStart.add(new CXNormSize(Size(100, 100))).add(new CXGrayscale());
    CPcaCompression compressor(80, 15, Size(100,100));
    // PCA-training
    for(Mat sample : sampler){
        Mat normSample = pipeStart.transform(sample);
        compressor.addSample(normSample);
    }
    double prec = compressor.process(true);
    std::cout << "Precission = " << prec << std::endl;
    // compress-decompress tests
    for(Mat sample : sampler){
        Mat normSample = pipeStart.transform(sample);
        Mat compSample = compressor.compress(normSample);
        Mat decompSample = compressor.decompress(compSample);
        Mat wholeImage = Mat( normSample.rows, normSample.cols + decompSample.cols, CV_8U );
        normSample.copyTo( wholeImage( Rect(0,0, normSample.cols, normSample.rows ) ) );
        decompSample.copyTo( wholeImage( Rect(normSample.cols, 0, decompSample.cols, decompSample.rows ) ) );
        resize(wholeImage, wholeImage, Size(), 2.0f, 2.0f);
        imshow("testwnd", wholeImage);
        waitKey(1);
    }
}

void VtlTestAppTest::testPostNMS(){
    tVecDetections input;
    tVecDetections expectedOutput;
    // put data in
    input.push_back(tDetectionPair(Rect(12,12,2,2), 0.2));
    input.push_back(tDetectionPair(Rect(10,10,20,20), 1.0));
    input.push_back(tDetectionPair(Rect(12,12,20,20), 0.8));
    input.push_back(tDetectionPair(Rect(28,28,5,5), 1.2));
        // group 2
    input.push_back(tDetectionPair(Rect(40,40,10,10), 1.0));
    input.push_back(tDetectionPair(Rect(40,13,30,30), 1.0));
        // group 3
    input.push_back(tDetectionPair(Rect(5,50,2,2), 1.0));

    // expected output
    expectedOutput.push_back(tDetectionPair(Rect(10,10,20,20), 1.0));
    expectedOutput.push_back(tDetectionPair(Rect(28,28,5,5), 1.2));
    expectedOutput.push_back(tDetectionPair(Rect(40,40,10,10), 1.0));
    expectedOutput.push_back(tDetectionPair(Rect(40,13,30,30), 1.0));
    expectedOutput.push_back(tDetectionPair(Rect(5,50,2,2), 1.0));

    // main test
    CFilterNMS filter(0.8);
    filter.process(input);
    char errorMsgBuffer[10000];

    // check
    sprintf( errorMsgBuffer, "Ouput wrong size: %d, expected %d", input.size(), expectedOutput.size());
    QVERIFY2( expectedOutput.size() == input.size(), errorMsgBuffer );
    for( tDetectionPair& detection : expectedOutput ){
        bool found = false;
        for( tDetectionPair& remainingElem : input ){
            if( remainingElem == detection ){
                found = true;
                break;
            }
        }
        if( !found ) // prepare error message
            sprintf(errorMsgBuffer, "Could not find detection (%d, %d, %d, %d) = %.2f",
                detection.first.x, detection.first.y, detection.first.width, detection.first.height, detection.second);
        QVERIFY2( found, errorMsgBuffer );
    }
}

QTEST_APPLESS_MAIN(VtlTestAppTest)

#include "tst_vtltestapptest.moc"
