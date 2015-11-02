#ifndef __RF_StatisticalResults_h__
#define __RF_StatisticalResults_h__

///
/// \brief StatisticalResults - holder for statistical results for a mass point
/// \author Valerio Ippolito - Harvard University
/// \date Apr 4, 2015
///
/// StatisticalResults is the class which holds statistical results
/// computed out of an individual RooWorkspace. It provides methods
/// to read from RooStats hypothesis test (inversion) results.
///

#include <Rtypes.h>

namespace RooStats {
   class HypoTestResult;
   class HypoTestInverterResult;
}

namespace RF {
   class StatisticalResults {
   public:
      // hypothesis test
      Double_t null_pvalue;
      Double_t null_pvalue_err;
      Double_t alt_pvalue;
      Double_t CLb;
      Double_t CLs;
      Double_t CLsplusb;
      // inverted hypothesis test
      Double_t obs_lowerlimit;
      Double_t obs_lowerlimit_err;
      Double_t obs_upperlimit;
      Double_t obs_upperlimit_err;
      Double_t exp_upperlimit;
      Double_t exp_upperlimit_plus1;
      Double_t exp_upperlimit_plus2;
      Double_t exp_upperlimit_minus1;
      Double_t exp_upperlimit_minus2;

      void readHypoTest(RooStats::HypoTestResult *r); // for p0s
      void readHypoTestInv(RooStats::HypoTestInverterResult *r); // for limits
   };
}

#endif

