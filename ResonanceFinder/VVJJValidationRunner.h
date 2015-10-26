#ifndef __RF_VVJJValidationRunner_h__
#define __RF_VVJJValidationRunner_h__

///
/// \brief VVJJValidationRunner - validation class for the VVJJ analysis
/// \author Valerio Ippolito - Harvard University
/// \date Apr 6, 2015
///
/// VVJJValidationRunner runs the VVJJ analysis in a simplified mode by
/// not using TTrees but, instead, directly histograms.
///

#include "ResonanceFinder/IBinnedAnalysisRunner.h"

namespace RF {
   class VVJJValidationRunner : public IBinnedAnalysisRunner {
   public:
      VVJJValidationRunner(TString analysis);
      ~VVJJValidationRunner();

      virtual void configInputGetter();
      virtual void manipulate();
   };
}

#endif
