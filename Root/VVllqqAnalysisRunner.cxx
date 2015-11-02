#include "ResonanceFinder/VVllqqAnalysisRunner.h"

RF::VVllqqAnalysisRunner::VVllqqAnalysisRunner(TString analysis) : IBinnedTreeAnalysisRunner(analysis)
{
   // note: we create the RF::BackgroundTemplateAdder although it's not really needed
   // destruction is, in any case, not our task by IBinnedAnalysisRunner's task
}

RF::VVllqqAnalysisRunner::~VVllqqAnalysisRunner()
{
}

void RF::VVllqqAnalysisRunner::manipulate()
{
   // do nothing (TODO: possibly implement something a la VVJJ?)
}
