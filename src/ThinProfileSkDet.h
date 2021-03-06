/*
 * ThinProfileSkDet.h
 *
 *  Created on: Jul 11, 2013
 *      Author: Michal Busta
 */

#ifndef THINPROFILESKDET_H_
#define THINPROFILESKDET_H_

#include "SkewDetector.h"

namespace cmp
{

/**
 * @class cmp::ThinProfileSkDet
 * 
 * @brief TODO brief description
 *
 * TODO type description
 */
class ThinProfileSkDet : public ContourSkewDetector
{
public:
	/**
	 *
	 * @param approximatioMethod
	 * @param epsilon
	 * @param ignoreAngle in degrees
	 */
	ThinProfileSkDet(int approximatioMethod = CV_CHAIN_APPROX_NONE, double epsilon = 0.023, int ignoreAngle = IGNORE_ANGLE, double profilesRange = 0.1, bool correctWidth = true, bool doConvex = true);

	virtual ~ThinProfileSkDet();

	virtual double detectSkew( std::vector<cv::Point>& contours, double lineK, bool approximate = false, cv::Mat* debugImage = NULL );

	virtual void voteInHistogram( std::vector<cv::Point>& outerContour, double lineK, double *histogram, double weight, bool approximate = false, cv::Mat* debugImage = NULL);

	/** bude hledat odchylku v rozmezi <-90+ignoreAngle; 90-ignoreAngle>; ve stupnich */
	int ignoreAngle;
	//% of profile width 
	double profilesRange;
    
	std::vector<double> probabilities;

private:

	double doEstimate(std::vector<cv::Point>& contours, double* hist, cv::Mat* debugImage = NULL);

	bool correctWidth;
    
    bool doConvex;

};

} /* namespace cmp */
#endif /* THINPROFILESKDET_H_ */
