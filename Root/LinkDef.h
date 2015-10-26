// ws building
#include "ResonanceFinder/BackgroundTemplateAdder.h"
#include "ResonanceFinder/BinnedWorkspaceBuilder.h"
#include "ResonanceFinder/Channel.h"
#include "ResonanceFinder/DownVariationAdder.h"
#include "ResonanceFinder/HistoCollection.h"
#include "ResonanceFinder/HistoLumiRescaler.h"
#include "ResonanceFinder/IAnalysisRunner.h"
#include "ResonanceFinder/IBinnedAnalysisRunner.h"
#include "ResonanceFinder/IBinnedTreeAnalysisRunner.h"
#include "ResonanceFinder/IHistoCollector.h"
#include "ResonanceFinder/IHistoManipulator.h"
#include "ResonanceFinder/ITreeRunner.h"
#include "ResonanceFinder/IWorkspaceBuilder.h"
#include "ResonanceFinder/ManualHistoCollector.h"
#include "ResonanceFinder/MultiplicativeFactor.h"
#include "ResonanceFinder/Sample.h"
#include "ResonanceFinder/TreeHistoCollector.h"
#include "ResonanceFinder/VVJJAnalysisRunner.h"
#include "ResonanceFinder/VVJJValidationRunner.h"
#include "ResonanceFinder/VVlvqqAnalysisRunner.h"
#include "ResonanceFinder/VVlvqqValidationRunner.h"
#include "ResonanceFinder/VVllqqAnalysisRunner.h"
#include "ResonanceFinder/VVllqqValidationRunner.h"
#include "ResonanceFinder/VHAnalysisRunner.h"
#include "ResonanceFinder/Variation.h"

// ws interpretation
#include "ResonanceFinder/HypoTestInvTool.h"
#include "ResonanceFinder/ILimitTool.h"
#include "ResonanceFinder/IWorkspaceCollector.h"
#include "ResonanceFinder/IWorkspaceManipulator.h"
#include "ResonanceFinder/LimitRunner.h"
#include "ResonanceFinder/LimitTool.h"
#include "ResonanceFinder/Plotter.h"
#include "ResonanceFinder/SignalInjector.h"
#include "ResonanceFinder/StatisticalResults.h"
#include "ResonanceFinder/StatisticalResultsCollection.h"
#include "ResonanceFinder/WorkspaceCollection.h"
#include "ResonanceFinder/WorkspaceCollector.h"
#include "ResonanceFinder/PullPlotter.h"


#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ class RF::BackgroundTemplateAdder;
#pragma link C++ class RF::BinnedWorkspaceBuilder;
#pragma link C++ class RF::Channel;
#pragma link C++ class RF::DownVariationAdder;
#pragma link C++ class RF::HistoCollection;
#pragma link C++ class RF::HistoLumiRescaler;
#pragma link C++ class RF::IAnalysisRunner;
#pragma link C++ class RF::IBinnedAnalysisRunner;
#pragma link C++ class RF::IBinnedTreeAnalysisRunner;
#pragma link C++ class RF::IHistoCollector;
#pragma link C++ class RF::IHistoManipulator;
#pragma link C++ class RF::ITreeRunner;
#pragma link C++ class RF::IWorkspaceBuilder;
#pragma link C++ class RF::ManualHistoCollector;
#pragma link C++ class RF::MultiplicativeFactor;
#pragma link C++ class RF::Sample;
#pragma link C++ class RF::TreeHistoCollector;
#pragma link C++ class RF::VVJJAnalysisRunner;
#pragma link C++ class RF::VVJJValidationRunner;
#pragma link C++ class RF::VVlvqqAnalysisRunner;
#pragma link C++ class RF::VVlvqqValidationRunner;
#pragma link C++ class RF::VVllqqAnalysisRunner;
#pragma link C++ class RF::VVllqqValidationRunner;
#pragma link C++ class RF::VHAnalysisRunner;
#pragma link C++ class RF::Variation;

#pragma link C++ class RooStats::HypoTestInvTool;
#pragma link C++ class RF::ILimitTool;
#pragma link C++ class RF::IWorkspaceCollector;
#pragma link C++ class RF::IWorkspaceManipulator;
#pragma link C++ class RF::LimitRunner;
#pragma link C++ class RF::LimitTool;
#pragma link C++ class RF::Plotter;
#pragma link C++ class RF::SignalInjector;
#pragma link C++ class RF::StatisticalResults;
#pragma link C++ class RF::StatisticalResultsCollection;
#pragma link C++ class RF::WorkspaceCollection;
#pragma link C++ class RF::WorkspaceCollector;

#pragma link C++ class PullPlotter;

#endif
