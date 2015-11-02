#ifndef __RF_VVllqqAnalysisRunner_h__
#define __RF_VVllqqAnalysisRunner_h__

///
/// \brief VVllqqAnalysisRunner - runner for the llqq analysis
/// \author Valerio Ippolito - Harvard University
/// \date Apr 18, 2015
///
/// VVllqqAnalysisRunner runs the VVllqq analysis
///

#include "ResonanceFinder/IBinnedTreeAnalysisRunner.h"

#include <RooAbsPdf.h>
#include <RooRealVar.h>

namespace RF {
   class VVllqqAnalysisRunner : public IBinnedTreeAnalysisRunner {
   public:
      VVllqqAnalysisRunner(TString analysis);
      ~VVllqqAnalysisRunner();

      virtual void manipulate();
   };
}

#endif
