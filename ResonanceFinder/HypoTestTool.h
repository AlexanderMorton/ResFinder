#ifndef __RF_HypoTestTool_h__
#define __RF_HypoTestTool_h__

// adapted by Valerio Ippolito - Harvard University

// Standard tutorial macro for hypothesis test (for computing the discovery significance) using all
// RooStats hypotheiss tests calculators and test statistics
//
//
//Author:  L. Moneta
//
// Usage:
//
// root>.L StandardHypoTestDemo.C
// root> StandardHypoTestDemo("fileName","workspace name","S+B modelconfig name","B model name","data set name",calculator type, test statistic type, //                             number of toys)
//
//
// type = 0 Freq calculator
// type = 1 Hybrid calculator
// type = 2 Asymptotic calculator
// type = 3 Asymptotic calculator using nominal Asimov data sets (not using fitted parameter values but nominal ones)
//
// testStatType = 0 LEP
//              = 1 Tevatron
//              = 2 Profile Likelihood
//              = 3 Profile Likelihood one sided (i.e. = 0 if mu_hat < 0)
//

#include "ResonanceFinder/RooStatsUtils.h"

#include <TString.h>
#include <string>

class RooWorkspace;
namespace RooStats {
   class HypoTestResult;
}

namespace RooStats {

   class HypoTestTool {

   public:
      HypoTestTool();
      ~HypoTestTool() {};

      HypoTestResult *
      Run(RooWorkspace * w,
          const char * modelSBName, const char * modelBName,
          const char * dataName,
          RF::CalculatorType calcType,  RF::TestStatType testStatType,
          int ntoys,
          bool useNumberCounting = false,
          const char * nuisPriorName = 0);



      void
      AnalyzeResult(HypoTestResult * htr,
                    RF::CalculatorType calcType);

      void SetParameter(const char * name, const char * value);
      void SetParameter(const char * name, bool value);
      void SetParameter(const char * name, int value);
      void SetParameter(const char * name, double value);

   private:

      bool m_generateBinned;
      bool m_noSystematics;
      int     m_printLevel;
      double  m_nToysRatio;
      double m_poiValue;
      bool m_writeResults;
      TString m_resultFileName;
      bool m_usePROOF;
      int     m_nWorkers;
   };

} // end namespace RooStats

#endif
