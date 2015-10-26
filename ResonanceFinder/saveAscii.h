// $Id$
/*
 * @file 
 * @author Aaron Armbruster <aaron.james.armbruster@cern.ch>
 * @date Sep, 2015
 * @brief 
 */



#include <set>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <vector>

#include "TFile.h"
#include "TH1D.h"
#include "TMath.h"
#include "Math/ProbFuncMathCore.h"

using namespace std;

// mode
// 0 = sig
// 1 = limit
// 2 = muhat
// 3 = cls
// 4 = llr
// 5 = hypo_tev
// 6 = hypo_llr
// 7 = CLsb (mu_up)
// 8 = CLb( mu_up)
// 9 = CLsb (mu=1)
// 10 = CLb (mu=1)

void saveAscii(string folder) ;
