#include "ResonanceFinder/VVJJAnalysisRunner.h"

#include "ResonanceFinder/HistoLumiRescaler.h"
#include "ResonanceFinder/BackgroundTemplateAdder.h"
#include "ResonanceFinder/DownVariationAdder.h"

RF::VVJJAnalysisRunner::VVJJAnalysisRunner(TString analysis) : IBinnedTreeAnalysisRunner(analysis), m_bkgName("background"), m_pdf(nullptr), m_obs(nullptr), m_ptrName("")
{
   m_useStrings = kTRUE;
   m_fitFunction = "CEXPR::b('pow(1 - x/sqrts, p2-xi*p3) * pow(x/sqrts, -p3)', {x[1050,3550],sqrts[13000],xi[4.3],p2[5.28833e+01,-100,100],p3[-1.72355e+00,-100,100]})";
   m_fitObservables = "x";

   m_oldLumi = 1.;
   m_newLumi = m_oldLumi; 
}

RF::VVJJAnalysisRunner::~VVJJAnalysisRunner()
{
}

TString RF::VVJJAnalysisRunner::bkgName() const
{
   return m_bkgName;
}
void RF::VVJJAnalysisRunner::setBkgName(TString name)
{
   m_bkgName = name;
}

TString RF::VVJJAnalysisRunner::ptrName() const
{
   return m_ptrName;
}
void RF::VVJJAnalysisRunner::setPtrName(TString name)
{
   m_ptrName = name;
}

void RF::VVJJAnalysisRunner::setLumiRescale(Double_t oldLumi, Double_t newLumi) {
   m_oldLumi = oldLumi;
   m_newLumi = newLumi;
}

void RF::VVJJAnalysisRunner::setFitFunction(TString name, TString obs)
{
   m_fitFunction = name;
   m_fitObservables = obs;
   m_pdf = nullptr;
   m_obs = nullptr;
   m_useStrings = kTRUE;
}
void RF::VVJJAnalysisRunner::setFitFunction(RooAbsPdf *pdf, RooRealVar *obs)
{
   m_fitFunction = "";
   m_fitObservables = "";
   m_pdf = pdf;
   m_obs = obs;
   m_useStrings = kFALSE;
}

void RF::VVJJAnalysisRunner::manipulate()
{
   RF::HistoCollection &hc = histoCollection();

   // first, rescale to luminosity
   if (m_oldLumi != m_newLumi) {
     RF::HistoLumiRescaler hlr;
     hlr.setLumi(m_oldLumi, m_newLumi);
     hlr.manipulate(hc);
   }

   // add down variations
   if (m_ptrName != "") {
      RF::DownVariationAdder dva;
      dva.setVariation(m_ptrName + "_up", m_ptrName + "_down");
      dva.manipulate(hc);
   }

   // add background template
   RF::BackgroundTemplateAdder bta;

   bta.setChannel("SR");
   bta.setBkgName(m_bkgName);
   if (m_useStrings) {
      bta.defineFitFunction(m_fitFunction, m_fitObservables);
   } else {
      bta.defineFitFunction(m_pdf, m_obs);
   }
   bta.manipulate(hc);
}
