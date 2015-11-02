#ifndef __RF_Sample_h__
#define __RF_Sample_h__

///
/// \brief Sample - class representing a sample
/// \author Valerio Ippolito - Harvard University
/// \date Mar 23, 2015
///
/// Sample is a class representing a physics sample.
/// Each sample itself contains many systematic variations,
/// and keeps track of multiplicative factors associated to
/// it.
///

#include "ResonanceFinder/Variation.h"
#include "ResonanceFinder/MultiplicativeFactor.h"

#include <TString.h>
#include <map>

class RooAbsPdf;

namespace RF {
   class Variation;
   class MultiplicativeFactor;
}

namespace RF {
   class Sample {
   public:
      Sample();
      Sample(TString name);
      ~Sample();

      TString name()const ;
      void setName(TString name);
      Bool_t normByLumi()const ;
      void setNormByLumi(Bool_t value);

      void addVariation(TString name);
      void addVariation(TString name, TString nameUp, TString nameDown);
      Variation &variation(TString name);
      void multiplyBy(TString name, Double_t value);
      void multiplyBy(TString name, Double_t value, Double_t min, Double_t max);
      void multiplyBy(TString name, Double_t value, Double_t min, Double_t max, MultiplicativeFactor::ConstraintType constraint);
      MultiplicativeFactor &factor(TString name);

      std::vector<TString> variations();
      std::vector<TString> factors();

      void setFitFunction(TString factory_command, TString obs); // cleaner
      void setFitFunction(RooAbsPdf *pdf, TString obs); // more error-prone; note: pdf is _not_ owned
      TString getFitFunctionObservable();
      RooAbsPdf *getFitFunctionPdf();
      TString getFitFunctionFactoryCommand();
      void setUseStatError(Bool_t val);
      Bool_t useStatError();

   private:
      TString m_name;
      Bool_t m_normByLumi;
      std::map<TString, Variation> m_variations;
      std::map<TString, MultiplicativeFactor> m_factors;
      // for explicit treatment (storage only)
      TString m_fitFunctionObs;
      TString m_fitFunctionFactoryCommand;
      RooAbsPdf *m_fitFunctionPdf;
      Bool_t m_useStatError;
   };
}

#endif
