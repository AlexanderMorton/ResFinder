#include <ResonanceFinder/HypoTestTool.h>

#include "TFile.h"
#include "RooWorkspace.h"
#include "RooAbsPdf.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooStats/ModelConfig.h"
#include "RooRandom.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TSystem.h"
#include "TROOT.h"

#include "RooStats/AsymptoticCalculator.h"
#include "RooStats/HybridCalculator.h"
#include "RooStats/FrequentistCalculator.h"
#include "RooStats/ToyMCSampler.h"
#include "RooStats/HypoTestPlot.h"

#include "RooStats/NumEventsTestStat.h"
#include "RooStats/ProfileLikelihoodTestStat.h"
#include "RooStats/SimpleLikelihoodRatioTestStat.h"
#include "RooStats/RatioOfProfiledLikelihoodsTestStat.h"
#include "RooStats/MaxLikelihoodEstimateTestStat.h"

#include "RooStats/HypoTestInverter.h"
#include "RooStats/HypoTestInverterResult.h"
#include "RooStats/HypoTestInverterPlot.h"

using namespace RooFit;
using namespace RooStats;
using namespace std;

RooStats::HypoTestTool::HypoTestTool() :
   m_generateBinned(false),
   m_noSystematics(false),
   m_printLevel(0),
   m_nToysRatio(4),
   m_poiValue(-1),
   m_writeResults(false),
   m_resultFileName(""),
   m_usePROOF(false),
   m_nWorkers(8)
{
}



void
RooStats::HypoTestTool::SetParameter(const char * name, bool value)
{
   //
   // set boolean parameters
   //

   std::string s_name(name);

   if (s_name.find("GenerateBinned") != std::string::npos) m_generateBinned = value;
   if (s_name.find("NoSystematics") != std::string::npos) m_noSystematics = value;
   if (s_name.find("WriteResult") != std::string::npos) m_writeResults = value;
   if (s_name.find("UsePROOF") != std::string::npos) m_usePROOF = value;

   return;
}



void
RooStats::HypoTestTool::SetParameter(const char * name, int value)
{
   //
   // set integer parameters
   //

   std::string s_name(name);

   if (s_name.find("PrintLevel") != std::string::npos) m_printLevel = value;
   if (s_name.find("NWorkers") != std::string::npos) m_nWorkers = value;

   return;
}



void
RooStats::HypoTestTool::SetParameter(const char * name, double value)
{
   //
   // set double precision parameters
   //

   std::string s_name(name);

   if (s_name.find("NToysRatio") != std::string::npos) m_nToysRatio = value;
   if (s_name.find("POIValue") != std::string::npos) m_poiValue = value;

   return;
}



void
RooStats::HypoTestTool::SetParameter(const char * name, const char * value)
{
   //
   // set string parameters
   //

   std::string s_name(name);

   if (s_name.find("ResultFileName") != std::string::npos) m_resultFileName = value;

   return;
}

void
RooStats::HypoTestTool::AnalyzeResult(HypoTestResult * htr,
                                      RF::CalculatorType calcType)
{
   if (calcType >= RF::CalculatorType::AsymptoticWithNominalAsimov) throw std::logic_error("Calculator not implemented");

   if (calcType != RF::CalculatorType::Asymptotic) {
      HypoTestPlot * plot = new HypoTestPlot(*htr, 100);
      plot->SetLogYaxis(true);
      plot->Draw();
   } else {
      std::cout << "Asymptotic results " << std::endl;

   }

   // look at expected significances
   // found median of S+B distribution
   if (calcType != RF::CalculatorType::Asymptotic) {

      SamplingDistribution * altDist = htr->GetAltDistribution();
      HypoTestResult htExp("Expected Result");
      htExp.Append(htr);
      // find quantiles in alt (S+B) distribution
      double p[5];
      double q[5];
      for (int i = 0; i < 5; ++i) {
         double sig = -2  + i;
         p[i] = ROOT::Math::normal_cdf(sig, 1);
      }
      std::vector<double> values = altDist->GetSamplingDistribution();
      TMath::Quantiles(values.size(), 5, &values[0], q, p, false);

      for (int i = 0; i < 5; ++i) {
         htExp.SetTestStatisticData(q[i]);
         double sig = -2  + i;
         std::cout << " Expected p -value and significance at " << sig << " sigma = "
                   << htExp.NullPValue() << " significance " << htExp.Significance() << " sigma " << std::endl;

      }
   } else {
      // case of asymptotic calculator
      for (int i = 0; i < 5; ++i) {
         double sig = -2  + i;
         // sigma is inverted here
         double pval = AsymptoticCalculator::GetExpectedPValues(htr->NullPValue(), htr->AlternatePValue(), -sig, false);
         std::cout << " Expected p -value and significance at " << sig << " sigma = "
                   << pval << " significance " << ROOT::Math::normal_quantile_c(pval, 1) << " sigma " << std::endl;

      }
   }


   if (m_writeResults) {
      // write to a file the results
      const char *  calcTypeName = (calcType == RF::CalculatorType::Frequentist) ? "Freq" : (calcType == RF::CalculatorType::Hybrid) ? "Hybr" : "Asym";
      if (m_resultFileName.IsNull()) {
         m_resultFileName = TString::Format("%s", calcTypeName);

         m_resultFileName += ".root";
      }

      TFile * fileOut = new TFile(m_resultFileName, "RECREATE");
      htr->Write();
      Info("StandardHypoTestDemo", "HypoTestResult has been written in the file %s", m_resultFileName.Data());

      fileOut->Close();
   }
}



// internal routine to run the hypothesis test
HypoTestResult *
RooStats::HypoTestTool::Run(RooWorkspace * w,
                            const char * modelSBName, const char * modelBName,
                            const char * dataName, RF::CalculatorType calcType,  RF::TestStatType testStatType,
                            int ntoys,
                            bool useNumberCounting,
                            const char * nuisPriorName)
{

   /*

     Other Parameter to pass in tutorial
     apart from standard for filename, ws, modelconfig and data

     type = 0 Freq calculator
     type = 1 Hybrid calculator
     type = 2 Asymptotic calculator

     testStatType = 0 LEP
     = 1 Tevatron
     = 2 Profile Likelihood
     = 3 Profile Likelihood one sided (i.e. = 0 if mu < mu_hat)

     ntoys:         number of toys to use

     useNumberCounting:  set to true when using number counting events

     nuisPriorName:   name of prior for the nnuisance. This is often expressed as constraint term in the global model
     It is needed only when using the HybridCalculator (type=1)
     If not given by default the prior pdf from ModelConfig is used.

     extra options are available as global paramwters of the macro. They major ones are:

     m_generateBinned       generate binned data sets for toys (default is false) - be careful not to activate with
     a too large (>=3) number of observables
     nToyRatio            ratio of S+B/B toys (default is 2)
     printLevel

   */

   // disable - can cause some problems
   //ToyMCSampler::SetAlwaysUseMultiGen(true);

   SimpleLikelihoodRatioTestStat::SetAlwaysReuseNLL(true);
   ProfileLikelihoodTestStat::SetAlwaysReuseNLL(true);
   RatioOfProfiledLikelihoodsTestStat::SetAlwaysReuseNLL(true);

   //RooRandom::randomGenerator()->SetSeed(0);

   // to change minimizers
   // ROOT::Math::MinimizerOptions::SetDefaultStrategy(0);
   // ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2");
   // ROOT::Math::MinimizerOptions::SetDefaultTolerance(1);

   if (!w) {
      cout << "workspace not found" << endl;
      return nullptr;
   }
   w->Print();

   // get the modelConfig out of the file
   ModelConfig* sbModel = (ModelConfig*) w->obj(modelSBName);


   // get the modelConfig out of the file
   RooAbsData* data = w->data(dataName);

   // make sure ingredients are found
   if (!data || !sbModel) {
      w->Print();
      cout << "data or ModelConfig was not found" << endl;
      return nullptr;
   }
   // make b model
   ModelConfig* bModel = (ModelConfig*) w->obj(modelBName);


   // case of no systematics
   // remove nuisance parameters from model
   if (m_noSystematics) {
      const RooArgSet * nuisPar = sbModel->GetNuisanceParameters();
      if (nuisPar && nuisPar->getSize() > 0) {
         std::cout << "StandardHypoTestInvDemo" << "  -  Switch off all systematics by setting them constant to their initial values" << std::endl;
         RooStats::SetAllConstant(*nuisPar);
      }
      if (bModel) {
         const RooArgSet * bnuisPar = bModel->GetNuisanceParameters();
         if (bnuisPar)
            RooStats::SetAllConstant(*bnuisPar);
      }
   }


   if (!bModel) {
      Info("StandardHypoTestInvDemo", "The background model %s does not exist", modelBName);
      Info("StandardHypoTestInvDemo", "Copy it from ModelConfig %s and set POI to zero", modelSBName);
      bModel = (ModelConfig*) sbModel->Clone();
      bModel->SetName(TString(modelSBName) + TString("B_only"));
      RooRealVar * var = dynamic_cast<RooRealVar*>(bModel->GetParametersOfInterest()->first());
      if (!var) return nullptr;
      double oldval = var->getVal();
      var->setVal(0);
      //bModel->SetSnapshot( RooArgSet(*var, *w->var("lumi"))  );
      bModel->SetSnapshot(RooArgSet(*var));
      var->setVal(oldval);
   }

   if (!sbModel->GetSnapshot() || m_poiValue > 0) {
      Info("StandardHypoTestDemo", "Model %s has no snapshot  - make one using model poi", modelSBName);
      RooRealVar * var = dynamic_cast<RooRealVar*>(sbModel->GetParametersOfInterest()->first());
      if (!var) return nullptr;
      double oldval = var->getVal();
      if (m_poiValue > 0)  var->setVal(m_poiValue);
      //sbModel->SetSnapshot( RooArgSet(*var, *w->var("lumi") ) );
      sbModel->SetSnapshot(RooArgSet(*var));
      if (m_poiValue > 0) var->setVal(oldval);
      //sbModel->SetSnapshot( *sbModel->GetParametersOfInterest() );
   }





   // part 1, hypothesis testing
   SimpleLikelihoodRatioTestStat * slrts = new SimpleLikelihoodRatioTestStat(*bModel->GetPdf(), *sbModel->GetPdf());
   // null parameters must includes snapshot of poi plus the nuisance values
   RooArgSet nullParams(*bModel->GetSnapshot());
   if (bModel->GetNuisanceParameters()) nullParams.add(*bModel->GetNuisanceParameters());

   slrts->SetNullParameters(nullParams);
   RooArgSet altParams(*sbModel->GetSnapshot());
   if (sbModel->GetNuisanceParameters()) altParams.add(*sbModel->GetNuisanceParameters());
   slrts->SetAltParameters(altParams);


   ProfileLikelihoodTestStat * profll = new ProfileLikelihoodTestStat(*bModel->GetPdf());


   RatioOfProfiledLikelihoodsTestStat *
   ropl = new RatioOfProfiledLikelihoodsTestStat(*bModel->GetPdf(), *sbModel->GetPdf(), sbModel->GetSnapshot());
   ropl->SetSubtractMLE(false);

   if (testStatType == RF::TestStatType::PL1sided) profll->SetOneSidedDiscovery(1);
   profll->SetPrintLevel(m_printLevel);

   // profll.SetReuseNLL(mOptimize);
   // slrts.SetReuseNLL(mOptimize);
   // ropl.SetReuseNLL(mOptimize);

   AsymptoticCalculator::SetPrintLevel(m_printLevel);

   HypoTestCalculatorGeneric *  hypoCalc = 0;
   // note here Null is B and Alt is S+B
   if (calcType == RF::CalculatorType::Frequentist) hypoCalc = new  FrequentistCalculator(*data, *sbModel, *bModel);
   else if (calcType == RF::CalculatorType::Hybrid) hypoCalc = new  HybridCalculator(*data, *sbModel, *bModel);
   else if (calcType == RF::CalculatorType::Asymptotic) hypoCalc = new  AsymptoticCalculator(*data, *sbModel, *bModel);

   if (calcType == RF::CalculatorType::Frequentist)
      ((FrequentistCalculator*)hypoCalc)->SetToys(ntoys, ntoys / m_nToysRatio);
   if (calcType == RF::CalculatorType::Hybrid)
      ((HybridCalculator*)hypoCalc)->SetToys(ntoys, ntoys / m_nToysRatio);
   if (calcType == RF::CalculatorType::Asymptotic) {
      if (testStatType == RF::TestStatType::PL1sided)((AsymptoticCalculator*) hypoCalc)->SetOneSidedDiscovery(true);
      if (testStatType != RF::TestStatType::PL2sided && testStatType != RF::TestStatType::PL1sided)
         Warning("StandardHypoTestDemo", "Only the PL test statistic can be used with AsymptoticCalculator - use by default a two-sided PL");


   }


   // check for nuisance prior pdf in case of nuisance parameters
   if (calcType == RF::CalculatorType::Hybrid && (bModel->GetNuisanceParameters() || sbModel->GetNuisanceParameters())) {
      RooAbsPdf * nuisPdf = 0;
      if (nuisPriorName) nuisPdf = w->pdf(nuisPriorName);
      // use prior defined first in bModel (then in SbModel)
      if (!nuisPdf)  {
         Info("StandardHypoTestDemo", "No nuisance pdf given for the HybridCalculator - try to deduce  pdf from the   model");
         if (bModel->GetPdf() && bModel->GetObservables())
            nuisPdf = RooStats::MakeNuisancePdf(*bModel, "nuisancePdf_bmodel");
         else
            nuisPdf = RooStats::MakeNuisancePdf(*sbModel, "nuisancePdf_sbmodel");
      }
      if (!nuisPdf) {
         if (bModel->GetPriorPdf())  {
            nuisPdf = bModel->GetPriorPdf();
            Info("StandardHypoTestDemo", "No nuisance pdf given - try to use %s that is defined as a prior pdf in the B model", nuisPdf->GetName());
         } else {
            Error("StandardHypoTestDemo", "Cannnot run Hybrid calculator because no prior on the nuisance parameter is specified or can be derived");
            return nullptr;
         }
      }
      assert(nuisPdf);
      Info("StandardHypoTestDemo", "Using as nuisance Pdf ... ");
      nuisPdf->Print();

      const RooArgSet * nuisParams = (bModel->GetNuisanceParameters()) ? bModel->GetNuisanceParameters() : sbModel->GetNuisanceParameters();
      RooArgSet * np = nuisPdf->getObservables(*nuisParams);
      if (np->getSize() == 0) {
         Warning("StandardHypoTestDemo", "Prior nuisance does not depend on nuisance parameters. They will be smeared in their full range");
      }
      delete np;

      ((HybridCalculator*)hypoCalc)->ForcePriorNuisanceAlt(*nuisPdf);
      ((HybridCalculator*)hypoCalc)->ForcePriorNuisanceNull(*nuisPdf);
   }

   // hypoCalc->ForcePriorNuisanceAlt(*sbModel->GetPriorPdf());
   // hypoCalc->ForcePriorNuisanceNull(*bModel->GetPriorPdf());

   ToyMCSampler * sampler = (ToyMCSampler *)hypoCalc->GetTestStatSampler();

   if (sampler && (calcType == RF::CalculatorType::Frequentist || calcType == RF::CalculatorType::Hybrid)) {

      // can speed up using proof-lite
      if (m_usePROOF) {
         ProofConfig pc(*w, m_nWorkers, "", kFALSE);
         sampler->SetProofConfig(&pc);    // enable proof
      }

      // look if pdf is number counting or extended
      if (sbModel->GetPdf()->canBeExtended()) {
         if (useNumberCounting)   Warning("StandardHypoTestDemo", "Pdf is extended: but number counting flag is set: ignore it ");
      } else {
         // for not extended pdf
         if (!useNumberCounting)  {
            int nEvents = data->numEntries();
            Info("StandardHypoTestDemo", "Pdf is not extended: number of events to generate taken  from observed data set is %d", nEvents);
            sampler->SetNEventsPerToy(nEvents);
         } else {
            Info("StandardHypoTestDemo", "using a number counting pdf");
            sampler->SetNEventsPerToy(1);
         }
      }

      if (data->isWeighted() && !m_generateBinned) {
         Info("StandardHypoTestDemo", "Data set is weighted, nentries = %d and sum of weights = %8.1f but toy generation is unbinned - it would be faster to set m_generateBinned to true\n", data->numEntries(), data->sumEntries());
      }
      if (m_generateBinned)  sampler->SetGenerateBinned(m_generateBinned);


      // set the test statistic
      if (testStatType == RF::TestStatType::LEP) sampler->SetTestStatistic(slrts);
      if (testStatType == RF::TestStatType::Tevatron) sampler->SetTestStatistic(ropl);
      if (testStatType == RF::TestStatType::PL2sided || testStatType == RF::TestStatType::PL1sided) sampler->SetTestStatistic(profll);

   }

   HypoTestResult *  htr = hypoCalc->GetHypoTest();
   htr->SetPValueIsRightTail(true);
   htr->SetBackgroundAsAlt(false);
   htr->Print(); // how to get meaningfull CLs at this point?

   delete sampler;
   delete slrts;
   delete ropl;
   delete profll;

   return htr;
}
