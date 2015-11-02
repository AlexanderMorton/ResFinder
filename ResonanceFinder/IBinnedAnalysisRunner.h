#ifndef __RF_IBinnedAnalysisRunner_h__
#define __RF_IBinnedAnalysisRunner_h__

///
/// \brief IBinnedAnalysisRunner - interface class for BinnedAnalysisRunner objects
/// \author Valerio Ippolito - Harvard University
/// \date Mar 23, 2015
///
/// IBinnedAnalysisRunner is an extension of IAnalysisRunner which deals with binned
/// workspace production. It relies on IHistoCollector.
///

#include "ResonanceFinder/IAnalysisRunner.h"

#include "ResonanceFinder/IHistoCollector.h"
#include "ResonanceFinder/BinnedWorkspaceBuilder.h"

namespace RF {
   class IBinnedAnalysisRunner : public IAnalysisRunner {
   public:
      inline IBinnedAnalysisRunner(TString analysis, IWorkspaceBuilder *workspaceBuilder, IHistoCollector *histoCollector = nullptr);
      inline IBinnedAnalysisRunner(TString analysis, IHistoCollector *histoCollector = nullptr);
      inline virtual ~IBinnedAnalysisRunner() = 0;

   protected:
      inline void getInput();
      virtual void configInputGetter() = 0;
      virtual void manipulate() = 0;
      inline virtual void configWSBuilder(TString signal);

      inline HistoCollection &histoCollection();
      IHistoCollector *m_histoCollector;
   };
}

RF::IBinnedAnalysisRunner::IBinnedAnalysisRunner(TString analysis, RF::IWorkspaceBuilder *workspaceBuilder, RF::IHistoCollector *histoCollector): IAnalysisRunner(analysis, workspaceBuilder), m_histoCollector(histoCollector)
{
  TH1::SetDefaultSumw2(); // needed for all histogram errors to make sense
}

RF::IBinnedAnalysisRunner::IBinnedAnalysisRunner(TString analysis, RF::IHistoCollector *histoCollector) : IAnalysisRunner(analysis, new RF::BinnedWorkspaceBuilder("")), m_histoCollector(histoCollector)
{
  TH1::SetDefaultSumw2(); // needed for all histogram errors to make sense
}

RF::IBinnedAnalysisRunner::~IBinnedAnalysisRunner()
{
   if (m_histoCollector) delete m_histoCollector;
}

void RF::IBinnedAnalysisRunner::getInput()
{
   // first, get standard HC
   TString input_txt_path = releaseDir() + "/" + analysis() + "/data/" + inputListTag();
   m_histoCollector->setSource(input_txt_path);

   configInputGetter();

   for (auto chan : channels()) {
      for (auto sample : channel(chan).samples()) {
         m_histoCollector->addSample(chan, sample);
      }
   }

   // then, manipulate it
   manipulate();

   // now, the HC is final

   // persist it to file for diagnostics
   TString output_diagnostic_path = getDiagnosticsDir();
   m_histoCollector->getHistoCollection().persist(output_diagnostic_path + "/histos_" + analysis() + "_" + outputWSTag() + ".root");
}

RF::HistoCollection &RF::IBinnedAnalysisRunner::histoCollection()
{
   return m_histoCollector->getHistoCollection();
}

void RF::IBinnedAnalysisRunner::configWSBuilder(TString signal)
{
   RF::BinnedWorkspaceBuilder *bwb = dynamic_cast<RF::BinnedWorkspaceBuilder*>(m_workspaceBuilder);
   bwb->setName(signal);
   bwb->setHistoCollection(histoCollection());
   bwb->setOutputFilePrefix(releaseDir() + "/" + analysis() + "/tmp/" + outputWSTag() + "/" + signal + "_test");
}

#endif
