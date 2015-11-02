#ifndef __RF_ManualHistoCollector_h__
#define __RF_ManualHistoCollector_h__

///
/// \brief ManualHistoCollector - a class ordering a list of histograms into a HistoCollection
/// \author Valerio Ippolito - Harvard University
/// \date Mar 23, 2015
///
/// ManualHistoCollector implements IHistoCollector for input from histograms.
/// It allows both to use the rigid directory structure
///    source/sample_channel.root
/// with each file containing the full list of histograms named like the variation they
/// refer to, and to specify manually each histogram's location.
///
/// Note: data histogram must be added manually.
///

#include "ResonanceFinder/IHistoCollector.h"

namespace RF {
   class ManualHistoCollector : public IHistoCollector {
   public:
      ManualHistoCollector();
      ManualHistoCollector(TString name);
      ~ManualHistoCollector();

      virtual void addSample(TString channel, TString sample);
      virtual void addSample(TString channel, TString sample, TString variation, TString hname, TString fname);

   private:
      std::map<TString, std::map<TString, TString>> m_samples; // key1: channel, key2:
      std::vector<TString> m_channels;
   };
}

#endif


