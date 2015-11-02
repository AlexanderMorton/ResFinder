#include "ResonanceFinder/HistoLumiRescaler.h"

RF::HistoLumiRescaler::HistoLumiRescaler() : m_originalLumi(1), m_targetLumi(1)
{
}

RF::HistoLumiRescaler::~HistoLumiRescaler()
{
}

void RF::HistoLumiRescaler::setLumi(Double_t original, Double_t target)
{
   m_originalLumi = original;
   m_targetLumi = target;
}

Double_t RF::HistoLumiRescaler::originalLumi() const
{
   return m_originalLumi;
}

void RF::HistoLumiRescaler::setOriginalLumi(Double_t lumi)
{
   m_originalLumi = lumi;
}

Double_t RF::HistoLumiRescaler::targetLumi() const
{
   return m_targetLumi;
}

void RF::HistoLumiRescaler::setTargetLumi(Double_t lumi)
{
   m_targetLumi = lumi;
}

void RF::HistoLumiRescaler::manipulate(HistoCollection &hc)
{
   for (auto kv : hc.histos()) {
     rescale(kv.second);
   }
}

void RF::HistoLumiRescaler::rescale(RF::Histo_t *h)
{
   h->Scale(m_targetLumi / m_originalLumi);
}
