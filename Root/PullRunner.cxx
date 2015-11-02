#include "ResonanceFinder/PullRunner.h"

#include "ResonanceFinder/WorkspaceCollection.h"
#include <stdexcept>

RF::PullRunner::ScanSetup::ScanSetup() : poi_steps(20), poi_min(0), poi_max(20)
{
}

RF::PullRunner::ScanSetup::ScanSetup(Int_t poiSteps, Double_t poiMin, Double_t poiMax) : poi_steps(poiSteps), poi_min(poiMin), poi_max(poiMax)
{
}

RF::PullRunner::ScanSetup::~ScanSetup()
{
}

RF::PullRunner::PullRunner(TString name) : m_name(name), m_outputFileFormatString(""), m_pullTool(name), m_pullBreakdownTool(name)
{
}

RF::PullRunner::~PullRunner()
{
}

TString RF::PullRunner::name() const
{
   return m_name;
}

void RF::PullRunner::setName(TString name)
{
   m_name = name;
}

TString RF::PullRunner::outputFileFormatString() const
{
   return m_outputFileFormatString;
}

void RF::PullRunner::setOutputFileFormatString(TString string)
{
   if (string.Contains("%s") == kFALSE) throw std::runtime_error("Output file format string must contain the %s specifier");

   m_outputFileFormatString = string;
}

void RF::PullRunner::setPOIScan(Int_t nsteps, Double_t min, Double_t max)
{
   m_standardSetup.poi_steps = nsteps;
   m_standardSetup.poi_min = min;
   m_standardSetup.poi_max = max;
}

void RF::PullRunner::setPOIScan(Double_t mass, Int_t nsteps, Double_t min, Double_t max)
{
   m_detailedSetup[mass] = RF::PullRunner::ScanSetup(nsteps, min, max);
}

RF::PullTool& RF::PullRunner::pullTool()
{
   return m_pullTool;
}

RF::StatisticalResultsCollection RF::PullRunner::run(RF::WorkspaceCollection &coll)
{

  std::cout << "DEBUG : in PullRunner::run() " << std::endl;

  RF::StatisticalResultsCollection result(name());


  for (auto &kv : coll.workspaces()) {

    m_pullTool.setMass(kv.first);
    m_pullTool.run(kv.second); 


    m_pullBreakdownTool.setMass(kv.first);
    m_pullBreakdownTool.run(kv.second);
  }


   return result;
}
