#ifndef __RF_HistoLumiRescaler_h__
#define __RF_HistoLumiRescaler_h__

///
/// \brief HistoLumiRescaler - rescale HistoCollection to given luminosity
/// \author Valerio Ippolito - Harvard University
/// \date Mar 23, 2015
///
/// HistoLumiRescaler is a simple class which takes an HistoCollection as an input,
/// and rescales all contained histograms to a given luminosity.
///

#include "ResonanceFinder/IHistoManipulator.h"

namespace RF {
   class HistoLumiRescaler : public IHistoManipulator {
   public:
      HistoLumiRescaler();
      ~HistoLumiRescaler();

      void setLumi(Double_t original, Double_t target);
      Double_t originalLumi() const;
      void setOriginalLumi(Double_t lumi);
      Double_t targetLumi() const;
      void setTargetLumi(Double_t lumi);

      virtual void manipulate(HistoCollection &hc);
      virtual void rescale(Histo_t *h);

   private:
      Double_t m_originalLumi;
      Double_t m_targetLumi;
   };
}

#endif
