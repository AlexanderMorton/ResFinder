#ifndef __RF_HypoTestInvTool_h__
#define __RF_HypoTestInvTool_h__

// adapted by Valerio Ippolito - Harvard University

/* -*- mode: c++ -*- */
// Standard tutorial macro for performing an inverted  hypothesis test for computing an interval
//
// This macro will perform a scan of the p-values for computing the interval or limit
//
//Author:  L. Moneta
//
// Usage:
//
// root>.L StandardHypoTestInvDemo.C
// root> StandardHypoTestInvDemo("fileName","workspace name","S+B modelconfig name","B model name","data set name",calculator type, test statistic type, use CLS,
//                                number of points, xmin, xmax, number of toys, use number counting)
//
//
// type = 0 Freq calculator
// type = 1 Hybrid calculator
// type = 2 Asymptotic calculator
// type = 3 Asymptotic calculator using nominal Asimov data sets (not using fitted parameter values but nominal ones)
//
// testStatType = 0 LEP
//              = 1 Tevatron
//              = 2 Profile Likelihood two sided
//              = 3 Profile Likelihood one sided (i.e. = 0 if mu < mu_hat)
//              = 4 Profile Likelihood signed ( pll = -pll if mu < mu_hat)
//              = 5 Max Likelihood Estimate as test statistic
//              = 6 Number of observed event as test statistic
//

#include "ResonanceFinder/RooStatsUtils.h"

#include <TString.h>
#include <string>

class RooWorkspace;
namespace RooStats {
   class HypoTestInverterResult;
}

namespace RooStats {

   class HypoTestInvTool {

   public:
      HypoTestInvTool();
      ~HypoTestInvTool() {};

      HypoTestInverterResult *
      RunInverter(RooWorkspace * w,
                  const char * modelSBName, const char * modelBName,
                  const char * dataName,
                  RF::CalculatorType type,  RF::TestStatType testStatType,
                  bool useCLs,
                  int npoints, double poimin, double poimax, int ntoys,
                  bool useNumberCounting = false,
                  const char * nuisPriorName = 0);



      void
      AnalyzeResult(HypoTestInverterResult * r,
                    RF::CalculatorType calculatorType,
                    RF::TestStatType testStatType,
                    bool useCLs,
                    int npoints,
                    const char * fileNameBase = 0);

      void SetParameter(const char * name, const char * value);
      void SetParameter(const char * name, bool value);
      void SetParameter(const char * name, int value);
      void SetParameter(const char * name, double value);

   private:

      bool mPlotHypoTestResult;
      bool mWriteResult;
      bool mOptimize;
      bool mUseVectorStore;
      bool mGenerateBinned;
      bool mUseProof;
      bool mRebuild;
      bool mReuseAltToys;
      bool mEnableDetOutput;
      bool mNoSystematics;
      int     mNWorkers;
      int     mNToyToRebuild;
      int     mRebuildParamValues;
      int     mPrintLevel;
      int     mInitialFit;
      int     mRandomSeed;
      double  mNToysRatio;
      double  mMaxPoi;
      double  mConfidenceLevel;
      int mAsimovBins;
      std::string mMassValue;
      std::string mMinimizerType;                  // minimizer type (default is what is in ROOT::Math::MinimizerOptions::DefaultMinimizerType()
      TString     mResultFileName;
      TString     mSBFitFileName;
   };

} // end namespace RooStats

#endif
