/*
 * run_evaluation.cpp
 *
 *  Created on: Jul 10, 2013
 *      Author: Michal Busta
 */

#include <iostream>

#include "SkewEvaluator.h"

using namespace cmp;

static void help()
{
	std::cout << "\nRuns evaluation \n Call:\n ./run_evaluation input_direcoty \n\n";
}

int main( int argc, char **argv )
{
	if( argc < 2)
	{
		help();
		return -1;
	}
	SkewEvaluator evaluator;
	evaluator.evaluate( argv[1] );

	return 0;
}
