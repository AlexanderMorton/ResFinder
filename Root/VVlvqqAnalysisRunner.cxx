#include "ResonanceFinder/VVlvqqAnalysisRunner.h"
#include "ResonanceFinder/DownVariationAdder.h"

RF::VVlvqqAnalysisRunner::VVlvqqAnalysisRunner(TString analysis) : IBinnedTreeAnalysisRunner(analysis)
{
   // note: we create the RF::BackgroundTemplateAdder although it's not really needed
   // destruction is, in any case, not our task by IBinnedAnalysisRunner's task
}

RF::VVlvqqAnalysisRunner::~VVlvqqAnalysisRunner()
{
}

void RF::VVlvqqAnalysisRunner::addOneSideVariation(TString variation)
{

   if (variation != "") {
      bool alreadyAdded =  std::find(m_onesidevar.begin(), m_onesidevar.end(), variation) != m_onesidevar.end();
      if (!alreadyAdded) {
         m_onesidevar.push_back(variation);
      }

   }
   return ;
}

void RF::VVlvqqAnalysisRunner::manipulate()
{
   // do nothing (TODO: possibly implement something a la VVJJ?)
   RF::HistoCollection &hc = histoCollection();
   for (auto kv : m_onesidevar) {

      // add down variations
      if (kv != "") {
         RF::DownVariationAdder dva;
         dva.setVariation(kv + "_up", kv + "_down");
         dva.manipulate(hc);
      }

   }

}
