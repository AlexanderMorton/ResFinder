#ifndef __RF_LimitTool_h__
#define __RF_LimitTool_h__

///
/// \brief LimitTool - class to compute limits
/// \author Valerio Ippolito - Harvard University
/// \date Apr 4, 2015
///
/// LimitTool takes an input RooWorkspace, allows configuration of the
/// underlying RooStats' HypoTestTool and HypoTestInvTool, runs them
/// and produces a StatisticalResults object.
///
/// TODO: this is effectively a wrapper class, but HypoTestInvTool and
/// HypoTestTool can be easily unified.
///

#include "ResonanceFinder/ILimitTool.h"

#include "ResonanceFinder/HypoTestInvTool.h"
#include "ResonanceFinder/HypoTestTool.h"

namespace RF {
   class LimitTool : public ILimitTool {
   public:
      LimitTool(TString name);
      ~LimitTool();

      CalculatorType calcType() const;
      void setCalcType(CalculatorType type);

      TestStatType testStatType() const;
      void setTestStatType(TestStatType type);

      Bool_t useCLs() const;
      void setUseCLs(Bool_t value);

      Int_t nToys() const;
      void setNToys(Int_t n);

      Bool_t doLimit() const;
      void setDoLimit(Bool_t value);

      Bool_t doPValue() const;
      void setDoPValue(Bool_t value);

      RooStats::HypoTestInvTool &hypoTestInvTool();
      RooStats::HypoTestTool &hypoTestTool();
      virtual StatisticalResults run(RooWorkspace *ws);

   protected:
      void init();
      RooStats::HypoTestInvTool m_hypoTestInvTool;
      RooStats::HypoTestTool m_hypoTestTool;

   private:
      CalculatorType m_calcType;
      TestStatType m_testStatType;
     
      Bool_t m_useCLs;
      Int_t m_nToys;

      Bool_t m_doLimit;
      Bool_t m_doPValue;

   };
}

#endif
