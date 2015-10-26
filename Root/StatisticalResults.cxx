#include "ResonanceFinder/StatisticalResults.h"

#include <RooStats/HypoTestResult.h>
#include <RooStats/HypoTestInverterResult.h>
#include <stdexcept>

void RF::StatisticalResults::readHypoTest(RooStats::HypoTestResult *r)
{
   if (!r) throw std::runtime_error("Void pointer to HypoTestResult");

   this->null_pvalue = r->NullPValue();
   this->null_pvalue_err = r->NullPValueError();
   this->alt_pvalue = r->AlternatePValue();
   this->CLb = r->CLb();
   this->CLs = r->CLs();
   this->CLsplusb = r->CLsplusb();
}

void RF::StatisticalResults::readHypoTestInv(RooStats::HypoTestInverterResult *r)
{
   if (!r) throw std::runtime_error("Void pointer to HypoTestInverterResult");

   if (r->IsTwoSided()) {
      obs_lowerlimit = r->LowerLimit();
      obs_lowerlimit_err = r->LowerLimitEstimatedError();
   } else {
      obs_lowerlimit = -9999;
      obs_lowerlimit_err = -9999;
   }
   obs_upperlimit = r->UpperLimit();
   obs_upperlimit_err = r->UpperLimitEstimatedError();
   exp_upperlimit = r->GetExpectedUpperLimit(0);
   exp_upperlimit_plus1 = r->GetExpectedUpperLimit(1);
   exp_upperlimit_plus2 = r->GetExpectedUpperLimit(2);
   exp_upperlimit_minus1 = r->GetExpectedUpperLimit(-1);
   exp_upperlimit_minus2 = r->GetExpectedUpperLimit(-2);
}
