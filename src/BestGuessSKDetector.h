/*
 * BestGuessSKDetector.h
 *
 *  Created on: Jul 26, 2013
 *      Author: Michal Busta
 */

#ifndef BESTGUESSSKDETECTOR_H_
#define BESTGUESSSKDETECTOR_H_

#include "SkewDetector.h"

namespace cmp
{

/**
 * @class cmp::BestGuessSKDetector
 * 
 * @brief TODO brief description
 *
 * TODO type description
 */
class BestGuessSKDetector : public SkewDetector
{
public:
	BestGuessSKDetector();
	virtual ~BestGuessSKDetector();


	virtual double detectSkew( cv::Mat& mask, double lineK, cv::Mat* debugImage = NULL );

	std::vector<double> weights;
private:
	std::vector<cv::Ptr<SkewDetector> > detectors;
};

} /* namespace cmp */
#endif /* BESTGUESSSKDETECTOR_H_ */
