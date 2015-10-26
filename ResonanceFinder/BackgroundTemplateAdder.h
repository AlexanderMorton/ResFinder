#ifndef __RF_BackgroundTemplateAdder_h__
#define __RF_BackgroundTemplateAdder_h__

///
/// \brief BackgroundTemplateAdder - add background templates from fit to HistoCollection
/// \author Valerio Ippolito - Harvard University
/// \date Mar 23, 2015
///
/// BackgroundTemplateAdder is the class which, given an input (HistoCollection) for
/// data (nominal) and signal (nominal+variations), modifies this HistoCollection to
/// include also a background model from a data-driven fit to the data histogram.
/// Background uncertainty envelopes are obtained using up/down variations of fitted
/// parameters.
///

#include "ResonanceFinder/IHistoManipulator.h"

#include <RooWorkspace.h>

class RooAbsPdf;
class RooArgSet;

namespace RF {
   class BackgroundTemplateAdder : public IHistoManipulator {
   public:
      class FitFunction {
      public:
         TString name;
         TString obs;
      };

      BackgroundTemplateAdder();
      ~BackgroundTemplateAdder();

      TString channel() const;
      void setChannel(TString name);
      Bool_t doAllVariations() const;
      void setAllVariations(Bool_t val);
      TString bkgName() const;
      void setBkgName(TString val);
      TString varName() const;
      void setVarName(TString val);

      virtual void defineFitFunction(TString factory, TString obs);
      virtual void defineFitFunction(RooAbsPdf *pdf, RooRealVar *obs);
      virtual void manipulate(HistoCollection &hc);

   protected:
      virtual void manipulate(HistoCollection &hc, TString channel);
      virtual std::vector<Histo_t*> createHistos(Histo_t *nominal, std::vector<Histo_t*> &variations);

      FitFunction m_fitFunction;
      RooWorkspace m_ws;

   private:
      TString m_channel;
      Bool_t m_doAllVariations;
      TString m_bkgName;
      TString m_varName;
   };
}

#endif

