// Author: Aaron Armbruster
// Date:   2012-05-30
// Email:  armbrusa@umich.edu
// Description: Find the value of sigma evaluated at a specified N

#ifndef FINDSIGMA
#define FINDSIGMA

#include "TMath.h"
#include "Math/ProbFuncMathCore.h"
#include "Math/QuantFuncMathCore.h"

#include "RooRealVar.h"
#include "Math/MinimizerOptions.h"
#include "RooMinimizerFcn.h"
#include "RooMinimizer.h"


#include "ResonanceFinder/minimize.h"

#include <map>
#include <iostream>

using namespace std;
using namespace RooFit;


// get sigma assuming nll -2logLR is parabolic in par
double findSigma(RooAbsReal* nll, double nll_val_true, RooRealVar* par, double val_guess, double val_true, double N, double precision) ;

// ____________________________________________________________________________|__________
double getSigma(RooAbsReal* nll, double nll_val_true, RooRealVar* par, double val, double val_true, double& tmu) ;

// ____________________________________________________________________________|__________
double getTpar(RooAbsReal* nll, double nll_val_true, RooRealVar* par, double val) ;

// ____________________________________________________________________________|__________
double getNLL(RooAbsReal* nll) ;
// ____________________________________________________________________________|__________
void setVal(RooRealVar* par, double val) ;

#endif
