// This file's extension implies that it's C, but it's really -*- C++ -*-.
// $Id$
/*
 * @file ResonanceFinder/PullTool.h
 * @author Haifeng Li <Haifeng.Li@cern.ch>
 * @date Sep, 2015
 * @brief 
 */

#ifndef __RF_PullTool_h__
#define __RF_PullTool_h__


#include "TString.h"

#include <string>
#include <vector>

#include "TFile.h"
#include "TH1D.h"
#include "Math/MinimizerOptions.h"
#include "TStopwatch.h"
#include "TString.h"

#include "RooWorkspace.h"
#include "RooStats/ModelConfig.h"
#include "RooDataSet.h"
#include "RooArgSet.h"
#include "RooRealVar.h"
#include "RooNLLVar.h"
#include "RooFitResult.h"

#include "ResonanceFinder/parseString.h"
#include "ResonanceFinder/minimize.h"
#include "ResonanceFinder/findSigma.h"
#include "ResonanceFinder/log.h"


using namespace std;
using namespace RooFit;
using namespace RooStats;

class RooWorkspace;

namespace RF {
   class PullTool{
   public:
      PullTool(TString name);
      ~PullTool();
      


      virtual void run(RooWorkspace *ws);
      
      void setMass(Double_t mass) ; 

   protected:
      void init();
      
   private:
      TString m_name;
      Bool_t m_useCLs;
      Int_t m_nToys;
      Double_t m_mass;
   };
}

#endif
