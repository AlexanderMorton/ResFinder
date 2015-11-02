#ifndef __RF_IAnalysisRunner_h__
#define __RF_IAnalysisRunner_h__

///
/// \brief IAnalysisRunner - interface class for AnalysisRunner objects
/// \author Valerio Ippolito - Harvard University
/// \date Mar 23, 2015
///
/// IAnalysisRunner is the interface for all AnalysisRunner objects.
/// It deals with i/o in terms of tags and rigid directory structures,
/// and allows the production of multiple workspaces (i.e. one per mass point).
///

#include "ResonanceFinder/IWorkspaceBuilder.h"
#include "ResonanceFinder/WorkspaceCollector.h"
#include "ResonanceFinder/LimitRunner.h"
#include "ResonanceFinder/LimitTool.h"
#include "ResonanceFinder/SignalInjector.h"
#include "ResonanceFinder/Plotter.h"
#include "ResonanceFinder/PullRunner.h"
#include "ResonanceFinder/PullTool.h"

#include <RooWorkspace.h>

namespace RF {
   class IAnalysisRunner {
   public:
      inline IAnalysisRunner(TString analysis, IWorkspaceBuilder *workspaceBuilder = nullptr);
      inline virtual ~IAnalysisRunner() = 0;

      inline TString analysis() const;
      inline void setAnalysis(TString name);
      inline Channel& channel(TString name);
      inline std::vector<TString> channels() const;
      inline void addChannel(TString name, TString selection = "");
      inline std::vector<TString> signals() const;
      inline void addSignal(TString name, Double_t point);
      inline TString poi() const;
      inline void setPOI(TString name);
      inline TString releaseDir() const;
      inline void setReleaseDir(TString name);
      inline TString inputListTag() const;
      inline void setInputListTag(TString tag);
      inline TString outputWSTag() const;
      inline void setOutputWSTag(TString tag);
      inline TString injectionSample() const;
      inline void setInjectionSample(TString name);
      inline Double_t injectionStrength() const;
      inline void setInjectionStrength(Double_t value);
      inline Bool_t doObserved() const;
      inline void setDoObserved(Bool_t value);

      inline LimitTool& limitTool();
      inline LimitRunner& limitRunner();
      inline StatisticalResultsCollection& getStatResults();

      inline PullTool&   pullTool();
      inline PullRunner& pullRunner();

      inline void run();
      inline void runFromFile(TString filename);
      inline void produceWS();
      inline void interpretWS();
      
      inline void doPull(bool prodPull);
      inline void doLimit(bool prodLimits);
      inline void doPValue(bool prodPvals);

      inline void setTestStatType(TestStatType tst);
      inline void setCalculatorType(CalculatorType calc);

   protected:
      virtual void getInput() = 0;
      virtual void configWSBuilder(TString signal) = 0;
      inline TString getWSDir();
      inline TString getDiagnosticsDir();
      std::map<TString, TString> m_selections; // key = channel, value = TTree selection
      IWorkspaceBuilder *m_workspaceBuilder;
      WorkspaceCollector *m_workspaceCollector; // TODO: use interface
      LimitRunner *m_limitRunner;
      StatisticalResultsCollection m_statResults;
      std::map<TString, Channel> m_channels;

      PullRunner *m_pullRunner;

   private:
      TString m_analysis;
      TString m_poi;
      TString m_releaseDir;
      TString m_inputListTag;
      TString m_outputWSTag;
      TString m_injectionSample;
      Double_t m_injectionStrength;
      Bool_t m_doObserved;
      std::map<TString, Double_t> m_signals; // key = sample name, value = mass

      bool m_doPull;

   };
}

RF::IAnalysisRunner::IAnalysisRunner(TString analysis, RF::IWorkspaceBuilder *workspaceBuilder) : m_workspaceBuilder(workspaceBuilder), m_analysis(analysis), m_poi("mu"), m_releaseDir(""), m_inputListTag(""), m_outputWSTag(""), m_injectionSample(""), m_injectionStrength(1.0), m_doObserved(kTRUE), m_doPull(false)
{
   m_workspaceCollector = new WorkspaceCollector("collector");
   m_limitRunner = new LimitRunner("runner");
   m_pullRunner = new PullRunner("pullrunner");

}

void RF::IAnalysisRunner::doPull(bool prodPull)
{
  m_doPull = prodPull ;
}

void RF::IAnalysisRunner::doLimit(bool prodLimit)
{
  m_limitRunner->limitTool().setDoPValue(prodLimit);
}

void RF::IAnalysisRunner::doPValue(bool prodPValue)
{
  m_limitRunner->limitTool().setDoPValue(prodPValue);
}

void RF::IAnalysisRunner::setTestStatType(RF::TestStatType tst)
{
  m_limitRunner->limitTool().setTestStatType(tst);
}
void RF::IAnalysisRunner::setCalculatorType(RF::CalculatorType calc)
{
  m_limitRunner->limitTool().setCalcType(calc);
}

RF::IAnalysisRunner::~IAnalysisRunner()
{
   if (m_workspaceBuilder) delete m_workspaceBuilder;
   if (m_workspaceCollector) delete m_workspaceCollector;
   if (m_limitRunner) delete m_limitRunner;
}

TString RF::IAnalysisRunner::analysis() const
{
   return m_analysis;
}

void RF::IAnalysisRunner::setAnalysis(TString name)
{
   m_analysis = name;
}

RF::Channel& RF::IAnalysisRunner::channel(TString name)
{
   if (m_channels.find(name) == m_channels.end()) throw std::runtime_error("Channel not specified");
   return m_channels[name];
}

std::vector<TString> RF::IAnalysisRunner::channels() const
{
   std::vector<TString> result;
   for (auto &kv : m_selections) result.push_back(kv.first);
   return result;
}

void RF::IAnalysisRunner::addChannel(TString name, TString selection)
{
   // TODO: the first part of this should go in ITreeRunner
   m_selections[name] = selection;
   m_channels[name] = RF::Channel(name);
}

std::vector<TString> RF::IAnalysisRunner::signals() const
{
   std::vector<TString> result;
   for (auto &kv : m_signals) result.push_back(kv.first);
   return result;
}

void RF::IAnalysisRunner::addSignal(TString name, Double_t point)
{
   m_signals[name] = point;
}

TString RF::IAnalysisRunner::poi() const
{
   return m_poi;
}

void RF::IAnalysisRunner::setPOI(TString name)
{
   m_poi = name;
}

TString RF::IAnalysisRunner::releaseDir() const
{
   return m_releaseDir;
}

void RF::IAnalysisRunner::setReleaseDir(TString name)
{
   m_releaseDir = name;
}

TString RF::IAnalysisRunner::inputListTag() const
{
   return m_inputListTag;
}

void RF::IAnalysisRunner::setInputListTag(TString tag)
{
   m_inputListTag = tag;
}

TString RF::IAnalysisRunner::outputWSTag() const
{
   return m_outputWSTag;
}

void RF::IAnalysisRunner::setOutputWSTag(TString tag)
{
   m_outputWSTag = tag;
}

TString RF::IAnalysisRunner::injectionSample() const
{
   return m_injectionSample;
}

void RF::IAnalysisRunner::setInjectionSample(TString name)
{
   m_injectionSample = name;
}

Double_t RF::IAnalysisRunner::injectionStrength() const
{
   return m_injectionStrength;
}

void RF::IAnalysisRunner::setInjectionStrength(Double_t value)
{
   m_injectionStrength = value;
}

Bool_t RF::IAnalysisRunner::doObserved() const
{
   return m_doObserved;
}

void RF::IAnalysisRunner::setDoObserved(Bool_t value)
{
   m_doObserved = value;
}

RF::LimitTool & RF::IAnalysisRunner::limitTool()
{
   return m_limitRunner->limitTool();
}

RF::LimitRunner & RF::IAnalysisRunner::limitRunner()
{
   return *m_limitRunner;
}

RF::StatisticalResultsCollection & RF::IAnalysisRunner::getStatResults()
{
   return m_statResults;
}

void RF::IAnalysisRunner::run()
{
   produceWS();
   interpretWS();
}

void RF::IAnalysisRunner::runFromFile(TString inFileName)
{
  
  const TString wsDir = getWSDir();
  
  // start preparing the WS collector
  m_workspaceCollector->setSource(wsDir);
  m_workspaceCollector->setPrefix(analysis());
  m_workspaceCollector->setSuffix(outputWSTag());
  m_workspaceCollector->setWsName("combined");
  
  WorkspaceCollection &coll = m_workspaceCollector->getWorkspaceCollection();
  coll.retrieve(inFileName);
  
  TString dset_to_use = (m_doObserved) ? "obsData" : "asimovData";
  
  const TString nickname = analysis() + "_" + outputWSTag();
  
  ////
  // run limit
  ////
  // first %s is replaced in LimitRunner either with "hypotestinv" or "hypotest"
  // second %s is the placeholder for the mass value string (actual filename is formatted in LimitRunner)
  m_limitRunner->setOutputFileFormatString(getDiagnosticsDir() + "/%s_" + nickname + "_%s.root");
  m_limitRunner->limitTool().setDataName(dset_to_use);
  
  m_statResults = m_limitRunner->run(coll);
  m_statResults.persist(wsDir + "/results_" + nickname + ".root");

  if (m_doPull) m_pullRunner->run(coll);

 }


void RF::IAnalysisRunner::produceWS()
{
   const TString wsDir = getWSDir();

   getInput();

   m_workspaceBuilder->setPOI(poi());

   for (auto &kv_sig : m_signals) {
      const TString thisSig = kv_sig.first;
      // prepare workspaceBuilder for holding channels
      m_workspaceBuilder->resetChannels();

      // we want to have a different workspace per signal point,
      // so we have to set the WS builder to have channels
      // which do not contain
      // add channels, excluding all signals which are not the current
      // signal (i.e. thisSig)
      for (auto &kv_chans : m_channels) {
         const TString thisChanName = kv_chans.first;
         const Channel& thisChan = kv_chans.second;
         m_workspaceBuilder->addChannel(thisChan);

         for (auto &kv_sig2 : m_signals) {
            const TString thisOtherSig = kv_sig2.first;

            if (thisOtherSig != thisSig) {
               // all signal samples but the current one are removed!
               m_workspaceBuilder->channel(thisChanName).removeSample(thisOtherSig);
            }
         } // loop over defined signals, to remove them from the channel definition
      } // loop over available channels, to add them to WS builder

      m_workspaceBuilder->setName(analysis() + "_" + thisSig);
      m_workspaceBuilder->setDest(releaseDir() + "/" + analysis() + "/tmp/" + outputWSTag());
      configWSBuilder(thisSig);
      m_workspaceBuilder->build();

      RooWorkspace *ws = m_workspaceBuilder->ws();

      // save
      ws->writeToFile(wsDir + "/" + analysis() + "_" + thisSig + "_" + outputWSTag() + ".root");
   }
}

void RF::IAnalysisRunner::interpretWS()
{
   const TString wsDir = getWSDir();

   // start preparing the WS collector
   m_workspaceCollector->setSource(wsDir);
   m_workspaceCollector->setPrefix(analysis());
   m_workspaceCollector->setSuffix(outputWSTag());
   m_workspaceCollector->setWsName(m_workspaceBuilder->wsName());

   for (auto &kv_sig : m_signals) {
      const TString thisSig = kv_sig.first;

      // collect it
      m_workspaceCollector->addMassPoint(m_signals[thisSig], thisSig);
   }


   ////
   // Process WS
   ////
   WorkspaceCollection &coll = m_workspaceCollector->getWorkspaceCollection();

   TString dset_to_use = (m_doObserved) ? "obsData" : "asimovData";

   // injection?
   if (m_injectionSample != "") {
      if (m_signals.find(m_injectionSample) == m_signals.end()) throw std::runtime_error("Injection sample does not exist");

      dset_to_use = "asimovData_inj_" + m_injectionSample;

      SignalInjector manip;
      manip.setModelConfig(m_workspaceBuilder->modelConfigName());
      manip.setMass(m_signals[m_injectionSample]);
      manip.setPOIValue(m_injectionStrength);
      manip.setDsetName(dset_to_use);

      manip.manipulate(coll);
   }

   // save manipulated collection
   coll.persist(wsDir + "/" + analysis() + "_actualWorkspaces_" + outputWSTag() + ".root");

   const TString nickname = analysis() + "_" + outputWSTag();

   ////
   // run limit
   ////
   // first %s is replaced in LimitRunner either with "hypotestinv" or "hypotest"
   // second %s is the placeholder for the mass value string (actual filename is formatted in LimitRunner)
   m_limitRunner->setOutputFileFormatString(getDiagnosticsDir() + "/%s_" + nickname + "_%s.root");
   m_limitRunner->limitTool().setDataName(dset_to_use);

   m_statResults = m_limitRunner->run(coll);
   m_statResults.persist(wsDir + "/results_" + nickname + ".root");


   if (m_doPull) m_pullRunner->run(coll);
}

TString RF::IAnalysisRunner::getWSDir()
{
   return releaseDir() + "/" + analysis() + "/ws";
}

TString RF::IAnalysisRunner::getDiagnosticsDir()
{
   return getWSDir() + "/diagnostics";
}

#endif
