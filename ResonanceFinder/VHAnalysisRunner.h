#ifndef __RF_VHAnalysisRunner_h__
#define __RF_VHAnalysisRunner_h__

///
/// \brief VHAnalysisRunner - binned histogram class for the VH analysis
/// \author Wade Fisher
/// \date June 22 2015
///
/// VHAnalysisRunner runs the VH analysis using histograms
///

#include "ResonanceFinder/IBinnedAnalysisRunner.h"

namespace RF {
   class VHAnalysisRunner : public IBinnedAnalysisRunner {
   public:
      VHAnalysisRunner(TString analysis);
      ~VHAnalysisRunner();

      virtual void configInputGetter();
      virtual void manipulate();

      void setTagNominal(TString tag);
      void setTagData(TString tag);

      void scaleSignal(double sf,TString name);
      void scaleBkgd(double sf, TString name);
      void scaleData(double sf);
      void setLumiRescale(Double_t oldLumi, Double_t newLumi);

      virtual void addOneSideVariation(TString variation, TString tagUp, TString tagDn);

  private:
      map<TString, double> m_sigScales;
      map<TString, double> m_bkgdScales;
      vector<TString> m_onesidevar;
      vector<TString> m_onesidetagUp;
      vector<TString> m_onesidetagDn;

      double m_dataScale;
      double m_oldLumi;
      double m_newLumi;
   };
}

#endif
