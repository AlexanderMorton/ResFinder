#include "ResonanceFinder/VVJJValidationRunner.h"

#include "ResonanceFinder/ManualHistoCollector.h"

RF::VVJJValidationRunner::VVJJValidationRunner(TString analysis) : RF::IBinnedAnalysisRunner(analysis, new RF::ManualHistoCollector())
{
}

RF::VVJJValidationRunner::~VVJJValidationRunner()
{
}

void RF::VVJJValidationRunner::configInputGetter()
{
   RF::ManualHistoCollector *mhc = dynamic_cast<RF::ManualHistoCollector*>(m_histoCollector);

   // manually add data (TreeHistoCollector automatically does that, but ManualHistoCollector
   // does not, as it is a low-level class allowing disrespect of rigid directory structure)

   for (auto &kv : m_selections) {
      // selection string is ignored
      mhc->addSample(kv.first, mhc->getHistoCollection().tagData());
   }
}

void RF::VVJJValidationRunner::manipulate()
{
   // do nothing
}
