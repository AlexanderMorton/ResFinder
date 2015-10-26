#ifndef __RF_VVlvqqAnalysisRunner_h__
#define __RF_VVlvqqAnalysisRunner_h__

///
/// \brief VVlvqqAnalysisRunner - runner for the lvqq analysis
/// \author Valerio Ippolito - Harvard University
/// \date Apr 18, 2015
///
/// VVlvqqAnalysisRunner runs the VVlvqq analysis
///

#include "ResonanceFinder/IBinnedTreeAnalysisRunner.h"

#include <RooAbsPdf.h>
#include <RooRealVar.h>

namespace RF {
   class VVlvqqAnalysisRunner : public IBinnedTreeAnalysisRunner {
   public:
      VVlvqqAnalysisRunner(TString analysis);
      ~VVlvqqAnalysisRunner();
      virtual void addOneSideVariation(TString variation);
      vector<TString> m_onesidevar;
      virtual void manipulate();
   };
}

#endif
