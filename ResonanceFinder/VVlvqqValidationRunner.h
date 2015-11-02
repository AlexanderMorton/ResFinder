#ifndef __RF_VVlvqqValidationRunner_h__
#define __RF_VVlvqqValidationRunner_h__

///
/// \brief VVlvqqValidationRunner - validation class for the VVlvqq analysis
/// \author Valerio Ippolito - Harvard University
/// \date Apr 6, 2015
///
/// VVlvqqValidationRunner runs the VVlvqq analysis in a simplified mode by
/// not using TTrees but, instead, directly histograms.
///

#include "ResonanceFinder/IBinnedAnalysisRunner.h"

namespace RF {
   class VVlvqqValidationRunner : public IBinnedAnalysisRunner {
   public:
      VVlvqqValidationRunner(TString analysis);
      ~VVlvqqValidationRunner();

      virtual void configInputGetter();
      virtual void manipulate();
   };
}

#endif
