VideoToolsLib
=============

VideoToolsLib is an attempt to follow the DRY (don't reapeat yourself)  principle. It'll be a collection of developt independent code components that I find useful from projec to project. It'll usually be some  ML-algorithms purposed for CV (computer vision) usage, and it will heavly rely on OpenCV.

Compress data
-------------
So, you have a bunch of data samples and a need to compress them into something a ML algorithm can use. PCA is the thing for you! So, how?
Generic interface:
<pre>
class CCompressorInterface
{
public:
    virtual cv::Mat compress(cv::Mat _sample) = 0;
    virtual cv::Mat decompress(cv::Mat _compressedSample, bool _convertTo8Bit = false) = 0;
};
</pre>

and a CPcaCompression class which follows that. Simple steps:

<pre>
	CPcaCompression c(sampleNumber, sampleSize, dimensionsToUse);
	// train PCA
	while(  a_lot_of_samples ){
		c.addSample(smallSample);
	}
	c.process();
	// main usage
	cv::Mat smallData = c.compress(bigData);
	cv::Mat bigData = c.decompress(smallData); // simple, no?
</pre>

NMS
---

A bunch of detection in the format of pairs std::pair<cv::Rect, double> and you need to NMS it? No problem.

<pre>
	tVecDetections input;
	input.push_back(tDetectionPair(Rect(12,12,2,2), 0.2));
	...
	CFilterNMS filter(0.8);
	filter.process(input);
</pre>

Voila!

Input transformations
---------------------

There should be a bunch of transformer classes that you can chain-up to get the desired input. Currently:
 - crop
 - grayscale conversion
 - image size normalization

Which can be used as such:
<pre>
    IXfmr pipeStart;
    pipeStart.add(new CXNormSize(Size(100, 100))).add(new CXGrayscale());
    Mat outSample = pipeStart.transform(inSample);
</pre>

Dependencies
============
 - OpenCV (used 2.4)
 - glog (google logging library)
 - qt (for testing and qmake)
 - boost

Contact
=======

firstname.lastname@epfl.ch
