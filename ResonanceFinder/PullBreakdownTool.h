// $Id$
/*
 * @file 
 * @author Stefan Gadatsch <Stefan.Gadatsch@cern.ch>
 * @author Haifeng Li <Haifeng.Li@cern.ch>
 * @date Sep, 2015
 * @brief 
 */

#ifndef __RF_PullBreakdownTool_h__
#define __RF_PullBreakdownTool_h__


#include "TString.h"

#include <string>
#include <vector>

#include "TFile.h"
#include "TH1D.h"
#include "Math/MinimizerOptions.h"
#include "TStopwatch.h"

#include "RooWorkspace.h"
#include "RooStats/ModelConfig.h"
#include "RooDataSet.h"
#include "RooArgSet.h"
#include "RooRealVar.h"
#include "RooNLLVar.h"
#include "RooFitResult.h"


#include "TXMLEngine.h"


#include "ResonanceFinder/parseString.h"
#include "ResonanceFinder/minimize.h"
#include "ResonanceFinder/findSigma.h"
#include "ResonanceFinder/log.h"




using namespace std;
using namespace RooFit;
using namespace RooStats;

class RooWorkspace;





namespace RF {
   class PullBreakdownTool{




   public:
      PullBreakdownTool(TString name);
      ~PullBreakdownTool();
      
     struct settings {
       string inFileName;
       string wsName;
       string modelConfigName;
       string dataName;
       string poiName;
       string xmlName;
       string technique;
       string catecory2eval;
       double precision;
       double corrCutoff;
       bool useMinos;
       string folder;
       string loglevel;
       RooWorkspace *ws ;
     };
     

      //virtual StatisticalResults run(RooWorkspace *ws);
     void run(RooWorkspace *ws,
		      string modelConfigName = "ModelConfig",
		      string dataName        = "obsData",
		      string poiName         = "mu",
		      string xmlName         = "config/HSG3_muhat_breakdown.xml",
		      string technique       = "add",
		      string catecory2eval   = "total",
		      double precision       = 0.005,
		      double corrCutoff      = 0.0,
		      string loglevel        = "DEBUG");

      void setParams(RooArgSet* nuis, list<string> tmp_list, string technique, RooFitResult* fitresult, RooRealVar* poi, double corrCutoff) ;
      list<string> addParams(settings* config, string catecory2eval) ;
      void writeTmpXML (string systName, settings* config) ;

      void setMass(Double_t mass) ; 

   protected:
      void init();
      
   private:
      Bool_t m_useCLs;
      Int_t m_nToys;
      Double_t m_mass;
   };
}

#endif
