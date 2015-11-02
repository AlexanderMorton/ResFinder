#include "ResonanceFinder/LimitTool.h"

#include "ResonanceFinder/StatisticalResults.h"
#include <RooWorkspace.h>
#include <RooStats/RooStatsUtils.h>
#include <RooStats/HypoTestInverterResult.h>
#include <stdexcept>

RF::LimitTool::LimitTool(TString name) : RF::ILimitTool(name), m_calcType(RF::CalculatorType::Asymptotic), m_testStatType(RF::TestStatType::PL1sided), m_useCLs(kTRUE), m_nToys(1000), m_doLimit(true), m_doPValue(true)
{
   init();
}

RF::LimitTool::~LimitTool()
{
}

RF::CalculatorType RF::LimitTool::calcType() const
{
   return m_calcType;
}
void RF::LimitTool::setCalcType(RF::CalculatorType type)
{
   m_calcType = type;
}
RF::TestStatType RF::LimitTool::testStatType() const
{
   return m_testStatType;
}
void RF::LimitTool::setTestStatType(RF::TestStatType type)
{
   m_testStatType = type;
}
Bool_t RF::LimitTool::useCLs() const
{
   return m_useCLs;
}
void RF::LimitTool::setUseCLs(Bool_t value)
{
   m_useCLs = value;
}
Bool_t RF::LimitTool::doLimit() const
{
   return m_doLimit;
}
void RF::LimitTool::setDoLimit(Bool_t value)
{
   m_doLimit = value;
}
Bool_t RF::LimitTool::doPValue() const
{
   return m_doPValue;
}
void RF::LimitTool::setDoPValue(Bool_t value)
{
   m_doPValue = value;
}
Int_t RF::LimitTool::nToys() const
{
   return m_nToys;
}
void RF::LimitTool::setNToys(Int_t n)
{
   m_nToys = n;
}

RooStats::HypoTestInvTool& RF::LimitTool::hypoTestInvTool()
{
   return m_hypoTestInvTool;
}

RooStats::HypoTestTool& RF::LimitTool::hypoTestTool()
{
   return m_hypoTestTool;
}

void RF::LimitTool::init()
{
   ///////////
   //// initialize hypothesis test inversion tool
   ///////////

   // standards from $ROOTSYS/tutorials/roostats/StandardHypoTestInvDemo.C
   const Bool_t plotHypoTestResult = true;          // plot test statistic result at each point
   const Bool_t writeResult = true;                 // write HypoTestInverterResult in a file
   const TString resultFileNameForLimit = name() + "_lim.root"; // file with results (by default is built automatically using the workspace input file name)
   const TString resultFileNameForP0 = name() + "_p0.root"; // file with results (by default is built automatically using the workspace input file name)
   const Bool_t optimize = true;                    // optmize evaluation of test statistic
   const Bool_t useVectorStore = true;              // convert data to use new roofit data store
   const Bool_t generateBinned = false;             // generate binned data sets
   const Bool_t noSystematics = false;              // force all systematics to be off (i.e. set all nuisance parameters as constat
   // to their nominal values)
   const double nToysRatioForLimit = 2;             // ratio Ntoys S+b/ntoysB
   const double maxPOI = -1;                        // max value used of POI (in case of auto scan)
   const Bool_t useProof = false;                   // use Proof Lite when using toys (for freq or hybrid)
   const Int_t nworkers = 0;                        // number of worker for ProofLite (default use all available cores)
   const Bool_t enableDetailedOutput = false;       // enable detailed output with all fit information for each toys (output will be written in result file)
   const Bool_t rebuild = false;                    // re-do extra toys for computing expected limits and rebuild test stat
   // distributions (N.B this requires much more CPU (factor is equivalent to nToyToRebuild)
   const Int_t nToyToRebuild = 100;                 // number of toys used to rebuild
   const Int_t rebuildParamValues = 0;              // = 0   do a profile of all the parameters on the B (alt snapshot) before performing a rebuild operation (default)
   // = 1   use initial workspace parameters with B snapshot values
   // = 2   use all initial workspace parameters with B
   // Otherwise the rebuild will be performed using
   const Int_t initialFit = -1;                     // do a first  fit to the model (-1 : default, 0 skip fit, 1 do always fit)
   const Int_t randomSeed = -1;                     // random seed (if = -1: use default value, if = 0 always random )
   // NOTE: Proof uses automatically a random seed

   const Int_t nAsimovBins = 0;                     // number of bins in observables used for Asimov data sets (0 is the default and it is given by workspace, typically is 100)

   const Bool_t reuseAltToys = false;               // reuse same toys for alternate hypothesis (if set one gets more stable bands)
   const double confidenceLevel = 0.95;             // confidence level value


   const std::string massValue = "";                // extra string to tag output file of result
   const std::string  minimizerType = "";           // minimizer type (default is what is in ROOT::Math::MinimizerOptions::DefaultMinimizerType()
   const Int_t   printLevel = 0;                    // print level for debugging PL test statistics and calculators

   const bool useNLLOffset = false;                 // use NLL offset when fitting (this increase stability of fits)


   m_hypoTestInvTool.SetParameter("PlotHypoTestResult", plotHypoTestResult);
   m_hypoTestInvTool.SetParameter("WriteResult", writeResult);
   m_hypoTestInvTool.SetParameter("Optimize", optimize);
   m_hypoTestInvTool.SetParameter("UseVectorStore", useVectorStore);
   m_hypoTestInvTool.SetParameter("GenerateBinned", generateBinned);
   m_hypoTestInvTool.SetParameter("NToysRatio", nToysRatioForLimit);
   m_hypoTestInvTool.SetParameter("MaxPOI", maxPOI);
   m_hypoTestInvTool.SetParameter("UseProof", useProof);
   m_hypoTestInvTool.SetParameter("EnableDetailedOutput", enableDetailedOutput);
   m_hypoTestInvTool.SetParameter("NWorkers", nworkers);
   m_hypoTestInvTool.SetParameter("Rebuild", rebuild);
   m_hypoTestInvTool.SetParameter("ReuseAltToys", reuseAltToys);
   m_hypoTestInvTool.SetParameter("NToyToRebuild", nToyToRebuild);
   m_hypoTestInvTool.SetParameter("RebuildParamValues", rebuildParamValues);
   m_hypoTestInvTool.SetParameter("MassValue", massValue.c_str());
   m_hypoTestInvTool.SetParameter("MinimizerType", minimizerType.c_str());
   m_hypoTestInvTool.SetParameter("PrintLevel", printLevel);
   m_hypoTestInvTool.SetParameter("InitialFit", initialFit);
   m_hypoTestInvTool.SetParameter("ResultFileName", resultFileNameForLimit);
   m_hypoTestInvTool.SetParameter("RandomSeed", randomSeed);
   m_hypoTestInvTool.SetParameter("AsimovBins", nAsimovBins);
   m_hypoTestInvTool.SetParameter("NoSystematics", noSystematics);
   m_hypoTestInvTool.SetParameter("ConfidenceLevel", confidenceLevel);

   // enable offset for all roostats
   if (useNLLOffset) RooStats::UseNLLOffset(true);

   setCalcType(RF::CalculatorType::Asymptotic);
   setTestStatType(RF::TestStatType::PL1sided);


   ///////////
   //// initialize hypothesis test tool
   ///////////

   const double nToysRatioForP0 = 4;                   // ratio Ntoys Null/ntoys ALT
   const double poiValue = 1;                    // change poi snapshot value for S+B model (needed for expected p0 values)

   m_hypoTestTool.SetParameter("NToysRatio", nToysRatioForP0);
   m_hypoTestTool.SetParameter("POIValue", poiValue);
   m_hypoTestTool.SetParameter("WriteResult", writeResult);
   m_hypoTestTool.SetParameter("ResultFileName", resultFileNameForP0);
   m_hypoTestTool.SetParameter("UseProof", useProof);
   m_hypoTestTool.SetParameter("NWorkers", nworkers);
}

RF::StatisticalResults RF::LimitTool::run(RooWorkspace *ws)
{
   if (!ws) throw std::runtime_error("Void input workspace");

   const char *modelBName = "";
   const Bool_t useNumberCounting = kFALSE;
   const char *nuisPriorName = 0;

   RF::StatisticalResults result;
   
   ///////////
   //// if requested, compute limits
   ///////////
   if(m_doLimit){
     RooStats::HypoTestInverterResult * r_limit = m_hypoTestInvTool.RunInverter(
										ws,
										modelConfig(),
										modelBName,
										dataName(),
										calcType(),
										testStatType(),
										useCLs(),
										poi_steps(),
										poi_min(),
										poi_max(),
										nToys(),
										useNumberCounting,
										nuisPriorName
										);
     if (!r_limit) throw std::runtime_error("Void HypoTestInverterResult");
     
     const char *infile = 0;
     m_hypoTestInvTool.AnalyzeResult(r_limit, calcType(), testStatType(), useCLs(), poi_steps(), infile); // not strictly needed
     
     result.readHypoTestInv(r_limit);
   }
   
   ///////////
   //// if requested, compute p0s
   ///////////     
   if(m_doPValue){
     RooStats::HypoTestResult * r_p0 = m_hypoTestTool.Run(
							  ws,
							  modelConfig(),
							  modelBName,
							  dataName(),
							  calcType(),
							  testStatType(),
							  nToys(),
							  useNumberCounting,
							  nuisPriorName
							  );
     if (!r_p0) throw std::runtime_error("Void HypoTestInverterResult");
     
     result.readHypoTest(r_p0);
     
     m_hypoTestTool.AnalyzeResult(r_p0, calcType()); // not strictly needed
   }
   
   return result;
}
