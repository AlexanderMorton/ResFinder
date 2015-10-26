// $Id$
/*
 * @file 
 * @author Haifeng Li <Haifeng.Li@cern.ch>
 * @date Sep, 2015
 * @brief 
 */

#ifndef __RF_PullRunner_h__
#define __RF_PullRunner_h__





#include "ResonanceFinder/PullTool.h"
#include "ResonanceFinder/PullBreakdownTool.h"
#include "ResonanceFinder/StatisticalResultsCollection.h"

namespace RF {
   class WorkspaceCollection;
}

namespace RF {
   class PullRunner {
   public:
      class ScanSetup {
      public:
         Int_t poi_steps;
         Double_t poi_min;
         Double_t poi_max;

         ScanSetup();
         ScanSetup(Int_t poiSteps, Double_t poiMin, Double_t poiMax);
         ~ScanSetup();
      };

      PullRunner(TString name);
      ~PullRunner();

      TString name() const;
      void setName(TString name);
      void setPOIScan(Int_t nsteps, Double_t min, Double_t max);
      void setPOIScan(Double_t mass, Int_t nsteps, Double_t min, Double_t max);
      TString outputFileFormatString() const;
      void setOutputFileFormatString(TString string);

      PullTool          &pullTool();
      PullBreakdownTool &pullBreakdownTool();
      StatisticalResultsCollection run(WorkspaceCollection &coll);

   private:
      TString m_name;
      ScanSetup m_standardSetup;
      std::map<Double_t, ScanSetup> m_detailedSetup;
      TString m_outputFileFormatString;
      PullTool m_pullTool;
      PullBreakdownTool m_pullBreakdownTool;

   };
}

#endif
