#ifndef __RF_VVJJAnalysisRunner_h__
#define __RF_VVJJAnalysisRunner_h__

///
/// \brief VVJJAnalysisRunner - runner for the VVJJ analysis
/// \author Valerio Ippolito - Harvard University
/// \date Mar 23, 2015
///
/// VVJJAnalysisRunner runs the VVJJ analysis
///

#include "ResonanceFinder/IBinnedTreeAnalysisRunner.h"

#include <RooAbsPdf.h>
#include <RooRealVar.h>

namespace RF {
   class VVJJAnalysisRunner : public IBinnedTreeAnalysisRunner {
   public:
      VVJJAnalysisRunner(TString analysis);
      ~VVJJAnalysisRunner();

      TString bkgName() const;
      void setBkgName(TString name);
      void setFitFunction(TString name, TString obs);
      void setFitFunction(RooAbsPdf *pdf, RooRealVar *obs);
      TString ptrName() const;
      void setPtrName(TString name);
      void setLumiRescale(Double_t oldLumi, Double_t newLumi);

      virtual void manipulate();

   private:
      TString m_bkgName;
      Bool_t m_useStrings;
      TString m_fitFunction;
      TString m_fitObservables;
      RooAbsPdf *m_pdf;
      RooRealVar *m_obs;
      TString m_ptrName;
      Double_t m_oldLumi;
      Double_t m_newLumi;
   };
}

#endif
