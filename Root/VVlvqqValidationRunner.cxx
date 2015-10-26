#include "ResonanceFinder/VVlvqqValidationRunner.h"

#include "ResonanceFinder/ManualHistoCollector.h"

RF::VVlvqqValidationRunner::VVlvqqValidationRunner(TString analysis) : RF::IBinnedAnalysisRunner(analysis, new RF::ManualHistoCollector())
{
}

RF::VVlvqqValidationRunner::~VVlvqqValidationRunner()
{
}

void RF::VVlvqqValidationRunner::configInputGetter()
{
   RF::ManualHistoCollector *mhc = dynamic_cast<RF::ManualHistoCollector*>(m_histoCollector);

   // manually add data (TreeHistoCollector automatically does that, but ManualHistoCollector
   // does not, as it is a low-level class allowing disrespect of rigid directory structure)

   for (auto &kv : m_selections) {
      // selection string is ignored
      mhc->addSample(kv.first, mhc->getHistoCollection().tagData());
   }
}

void RF::VVlvqqValidationRunner::manipulate()
{
   // do nothing
}
