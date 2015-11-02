// Author: Aaron Armbruster
// Date:   2012-05-30
// Email:  armbrusa@umich.edu
// Description: minimize a function with a smart retry strategy, eventually compute Minos error

#ifndef MINIMIZE
#define MINIMIZE

#include "RooDataSet.h"
#include "RooRealVar.h"
#include "RooMinimizer.h"
#include "RooNLLVar.h"
#include "TStopwatch.h"
#include "RooFitResult.h"

#include <iostream>
#include <string>

using namespace std;
using namespace RooFit;

RooFitResult* minimize(RooAbsReal* fcn, RooArgSet* minosSet = NULL) ;
    

#endif
