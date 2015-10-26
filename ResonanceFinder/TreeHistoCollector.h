#ifndef __RF_TreeHistoCollector_h__
#define __RF_TreeHistoCollector_h__

///
/// \brief TreeHistoCollector - a class preparing a histogram collection from trees
/// \author Valerio Ippolito - Harvard University
/// \date Mar 23, 2015
///
/// TreeHistoCollector implements IHistoCollector for input from TTrees. It creates
/// 1D histograms with the specified binning, using TTree::Draw with a single
/// observable and a single weight variable, plus an optional selection string for
/// defining channel.
///
/// Note: data histogram is collected automatically when a channel is added.
///

#include "ResonanceFinder/IHistoCollector.h"

class TTree;

namespace RF {
   class TreeHistoCollector : public IHistoCollector {
   public:
      TreeHistoCollector();
      TreeHistoCollector(TString name);
      ~TreeHistoCollector();

      TString observable() const;
      void setObservable(TString var);
      TString weight() const;
      void setWeight(TString var);
      Int_t nbins() const;
      void setNbins(Int_t nbins);
      Double_t obsMin() const;
      void setObsMin(Double_t min);
      Double_t obsMax() const;
      void setObsMax(Double_t max);
      void setBins(Double_t* bins);
      void addChannel(TString channel, TString formula = "");
      std::vector<TString> getChannels();
      virtual void addSample(TString channel, TString sample);

   protected:
      virtual void addSample(TString channel, TString sample, Bool_t isData);
      Histo_t* processTree(TTree *tree, TString channel);

   private:
      virtual void addData(TString channel);
      TString m_obs;
      TString m_weight;
      Int_t m_nbins;
      Double_t m_obs_min;
      Double_t m_obs_max;
      Double_t* m_bins;
      Bool_t m_use_bins;
      std::map<TString, TString> m_selection; // key: channel
   };
}

#endif

