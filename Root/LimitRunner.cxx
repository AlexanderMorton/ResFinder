#include "ResonanceFinder/LimitRunner.h"

#include "ResonanceFinder/WorkspaceCollection.h"
#include <stdexcept>

RF::LimitRunner::ScanSetup::ScanSetup() : poi_steps(20), poi_min(0), poi_max(20)
{
}

RF::LimitRunner::ScanSetup::ScanSetup(Int_t poiSteps, Double_t poiMin, Double_t poiMax) : poi_steps(poiSteps), poi_min(poiMin), poi_max(poiMax)
{
}

RF::LimitRunner::ScanSetup::~ScanSetup()
{
}

RF::LimitRunner::LimitRunner(TString name) : m_name(name), m_outputFileFormatString(""), m_limitTool(name)
{
}

RF::LimitRunner::~LimitRunner()
{
}

TString RF::LimitRunner::name() const
{
   return m_name;
}

void RF::LimitRunner::setName(TString name)
{
   m_name = name;
}

TString RF::LimitRunner::outputFileFormatString() const
{
   return m_outputFileFormatString;
}

void RF::LimitRunner::setOutputFileFormatString(TString string)
{
   if (string.Contains("%s") == kFALSE) throw std::runtime_error("Output file format string must contain the %s specifier");

   m_outputFileFormatString = string;
}

void RF::LimitRunner::setPOIScan(Int_t nsteps, Double_t min, Double_t max)
{
   m_standardSetup.poi_steps = nsteps;
   m_standardSetup.poi_min = min;
   m_standardSetup.poi_max = max;
}

void RF::LimitRunner::setPOIScan(Double_t mass, Int_t nsteps, Double_t min, Double_t max)
{
   m_detailedSetup[mass] = RF::LimitRunner::ScanSetup(nsteps, min, max);
}

RF::LimitTool& RF::LimitRunner::limitTool()
{
   return m_limitTool;
}

RF::StatisticalResultsCollection RF::LimitRunner::run(RF::WorkspaceCollection &coll)
{
   RF::StatisticalResultsCollection result(name());

   for (auto &kv : coll.workspaces()) {
      // prepare suffix in case it is needed (it shouldn't, though)
      TString massString = TString::Format(coll.massFormatString(), kv.first).ReplaceAll(".", "p");
      m_limitTool.hypoTestInvTool().SetParameter("MassValue", massString);

      // prepare filename (replace second %s in m_outputFileFormatString with mass value string)
      m_limitTool.hypoTestInvTool().SetParameter("ResultFileName", TString::Format(m_outputFileFormatString, "hypotestinv", massString.Data()));
      m_limitTool.hypoTestInvTool().SetParameter("SBFitFileName", TString::Format(m_outputFileFormatString, "sbfit", massString.Data()));
      m_limitTool.hypoTestTool().SetParameter("ResultFileName", TString::Format(m_outputFileFormatString, "hypotest", massString.Data()));

      // setup scan
      if (m_detailedSetup.find(kv.first) != m_detailedSetup.end()) {
         // use personalised scan setup
         m_limitTool.setPOISteps(m_detailedSetup[kv.first].poi_steps, m_detailedSetup[kv.first].poi_min, m_detailedSetup[kv.first].poi_max);
      } else {
         // use standard scan setup
         m_limitTool.setPOISteps(m_standardSetup.poi_steps, m_standardSetup.poi_min, m_standardSetup.poi_max);
      }

      // run and save results
      RF::StatisticalResults thisRes = m_limitTool.run(kv.second);
      result.addResult(thisRes, kv.first);
   }

   return result;
}
