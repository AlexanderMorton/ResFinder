#ifndef __RF_VVllqqValidationRunner_h__
#define __RF_VVllqqValidationRunner_h__

///
/// \brief VVllqqValidationRunner - validation class for the VVllqq analysis
/// \author Valerio Ippolito - Harvard University
/// \date Apr 6, 2015
///
/// VVllqqValidationRunner runs the VVllqq analysis in a simplified mode by
/// not using TTrees but, instead, directly histograms.
///

#include "ResonanceFinder/IBinnedAnalysisRunner.h"

namespace RF {
   class VVllqqValidationRunner : public IBinnedAnalysisRunner {
   public:
      VVllqqValidationRunner(TString analysis);
      ~VVllqqValidationRunner();

      virtual void configInputGetter();
      virtual void manipulate();
   };
}

#endif
