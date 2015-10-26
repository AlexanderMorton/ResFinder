#ifndef __RF_LimitRunner_h__
#define __RF_LimitRunner_h__

///
/// \brief LimitRunner - class to run limits
/// \author Valerio Ippolito - Harvard University
/// \date Apr 5, 2015
///
/// LimitRunner takes an input WorkspaceCollection, and uses a LimitTool
/// to produce a StatisticalResultsCollection
///

#include "ResonanceFinder/LimitTool.h"
#include "ResonanceFinder/StatisticalResultsCollection.h"

namespace RF {
   class WorkspaceCollection;
}

namespace RF {
   class LimitRunner {
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

      LimitRunner(TString name);
      ~LimitRunner();

      TString name() const;
      void setName(TString name);
      void setPOIScan(Int_t nsteps, Double_t min, Double_t max);
      void setPOIScan(Double_t mass, Int_t nsteps, Double_t min, Double_t max);
      TString outputFileFormatString() const;
      void setOutputFileFormatString(TString string);

      LimitTool &limitTool();
      StatisticalResultsCollection run(WorkspaceCollection &coll);

   private:
      TString m_name;
      ScanSetup m_standardSetup;
      std::map<Double_t, ScanSetup> m_detailedSetup;
      TString m_outputFileFormatString;
      LimitTool m_limitTool;
   };
}

#endif
