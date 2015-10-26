#ifndef __RF_IBinnedTreeAnalysisRunner_h__
#define __RF_IBinnedTreeAnalysisRunner_h__

///
/// \brief IBinnedTreeAnalysisRunner - interface class for BinnedTreeAnalysisRunner objects
/// \author Valerio Ippolito - Harvard University
/// \date Mar 23, 2015
///
/// IBinnedTreeAnalysisRunner is an extension of IBinnedAnalysisRunner which deals with
/// binned workspace production from TTrees.
///

#include "ResonanceFinder/IBinnedAnalysisRunner.h"
#include "ResonanceFinder/ITreeRunner.h"

#include "ResonanceFinder/TreeHistoCollector.h"

namespace RF {
   class IBinnedTreeAnalysisRunner : public IBinnedAnalysisRunner, public ITreeRunner {
   public:
      inline IBinnedTreeAnalysisRunner(TString analysis, IWorkspaceBuilder *workspaceBuilder, IHistoCollector *histoCollector);
      inline IBinnedTreeAnalysisRunner(TString analysis);
      inline virtual ~IBinnedTreeAnalysisRunner() = 0;

      inline Int_t nbins() const;
      inline void setNbins(Int_t nbins);
      inline Double_t obsMin() const;
      inline void setObsMin(Double_t min);
      inline Double_t obsMax() const;
      inline void setObsMax(Double_t max);
      inline void setBins(Double_t* bins);

   protected:
      inline virtual void configInputGetter();
      virtual void manipulate() = 0;

   private:
      Int_t m_nbins;
      Double_t m_obs_min;
      Double_t m_obs_max;
      Double_t* m_bins;
   };
}

RF::IBinnedTreeAnalysisRunner::IBinnedTreeAnalysisRunner(TString analysis, RF::IWorkspaceBuilder *workspaceBuilder, RF::IHistoCollector *histoCollector) : RF::IBinnedAnalysisRunner(analysis, workspaceBuilder, histoCollector), m_bins(nullptr)
{
}

RF::IBinnedTreeAnalysisRunner::IBinnedTreeAnalysisRunner(TString analysis) : RF::IBinnedAnalysisRunner(analysis, new RF::TreeHistoCollector()), m_bins(nullptr)
{
}

RF::IBinnedTreeAnalysisRunner::~IBinnedTreeAnalysisRunner()
{
}

Int_t RF::IBinnedTreeAnalysisRunner::nbins() const
{
   return m_nbins;
}

void RF::IBinnedTreeAnalysisRunner::setNbins(Int_t nbins)
{
   m_nbins = nbins;
}

void RF::IBinnedTreeAnalysisRunner::setBins(Double_t* bins)
{
   m_bins = bins;
}

Double_t RF::IBinnedTreeAnalysisRunner::obsMin() const
{
   return m_obs_min;
}

void RF::IBinnedTreeAnalysisRunner::setObsMin(Double_t min)
{
   m_obs_min = min;
}

Double_t RF::IBinnedTreeAnalysisRunner::obsMax() const
{
   return m_obs_max;
}

void RF::IBinnedTreeAnalysisRunner::setObsMax(Double_t max)
{
   m_obs_max = max;
}

void RF::IBinnedTreeAnalysisRunner::configInputGetter()
{
   RF::TreeHistoCollector *thc = dynamic_cast<RF::TreeHistoCollector*>(m_histoCollector);

   thc->setObservable(treeObs());
   thc->setWeight(treeWeight());
   thc->setNbins(nbins());
   thc->setObsMin(obsMin());
   thc->setObsMax(obsMax());
   if (m_bins) thc->setBins(m_bins);

   for (auto &kv : m_selections) {
      thc->addChannel(kv.first, kv.second);
   }
}

#endif
