#include "ResonanceFinder/BackgroundTemplateAdder.h"

#include <RooAbsPdf.h>
#include <RooRealVar.h>
#include <RooDataHist.h>
#include <TIterator.h>
#include <TMath.h>

RF::BackgroundTemplateAdder::BackgroundTemplateAdder() : m_doAllVariations(kFALSE), m_bkgName("background"), m_varName("fit")
{
   m_fitFunction.name = "pdf";
   m_fitFunction.obs = "x";
}

RF::BackgroundTemplateAdder::~BackgroundTemplateAdder()
{
}

TString RF::BackgroundTemplateAdder::channel() const
{
   return m_channel;
}

void RF::BackgroundTemplateAdder::setChannel(TString name)
{
   m_channel = name;
}

Bool_t RF::BackgroundTemplateAdder::doAllVariations() const
{
   return m_doAllVariations;
}

void RF::BackgroundTemplateAdder::setAllVariations(Bool_t val)
{
   m_doAllVariations = val;

   if (m_doAllVariations == kTRUE) {
      throw std::logic_error("NOT YET IMPLEMENTED (needs double loop on createIterator in manipulate())");
   }
}

TString RF::BackgroundTemplateAdder::bkgName() const
{
   return m_bkgName;
}

void RF::BackgroundTemplateAdder::setBkgName(TString name)
{
   m_bkgName = name;
}

TString RF::BackgroundTemplateAdder::varName() const
{
   return m_varName;
}

void RF::BackgroundTemplateAdder::setVarName(TString name)
{
   m_varName = name;
}

void RF::BackgroundTemplateAdder::defineFitFunction(TString factory, TString obs)
{
   RooAbsPdf *pdf = dynamic_cast<RooAbsPdf*>(m_ws.factory(factory));
   if (!pdf) throw std::runtime_error("pdf creation error");

   m_fitFunction.name = pdf->GetName();
   m_fitFunction.obs = obs;
}

void RF::BackgroundTemplateAdder::defineFitFunction(RooAbsPdf *pdf, RooRealVar *obs)
{
   const TString pdfname = pdf->GetName();
   const TString obsname = obs->GetName(); // to make sure it was imported with the pdf
   if (! m_ws.import(*pdf)) throw std::runtime_error("pdf import error");

   if (m_ws.var(obsname) == nullptr) {
      throw std::runtime_error("observable not in pdf");
   }

   m_fitFunction.name = pdfname;
   m_fitFunction.obs = obsname;
}

void RF::BackgroundTemplateAdder::manipulate(HistoCollection &hc)
{
   manipulate(hc, m_channel);
}

void RF::BackgroundTemplateAdder::manipulate(HistoCollection &hc, TString channel)
{
   RooAbsPdf *pdf = m_ws.pdf(m_fitFunction.name);
   if (!pdf) throw std::runtime_error("pdf not found");

   // import data histogram and use it as input for fitting
   Histo_t* h_bkg = dynamic_cast<Histo_t*>(hc.getData(channel)->Clone("bkgToFit"));
   const Double_t area = h_bkg->Integral();
   const Int_t nbins = h_bkg->GetNbinsX();
   const Double_t xmin = h_bkg->GetXaxis()->GetXmin();
   const Double_t xmax = h_bkg->GetXaxis()->GetXmax();

   RooDataHist dh_bkg("dh_bkg", "dh_bkg", *m_ws.var(m_fitFunction.obs), h_bkg);

   std::cout << "BackgroundTemplateAdder::manipulate fitting data" << std::endl;
   pdf->fitTo(dh_bkg, RooFit::Minos());

   const TString nominal_snap = "nominal";
   m_ws.saveSnapshot(nominal_snap, *pdf->getParameters(dh_bkg));
   std::cout << "BackgroundTemplateAdder::manipulate created nominal snapshot:" << std::endl;
   m_ws.getSnapshot(nominal_snap)->Print();
   Histo_t *h_nominal = dynamic_cast<Histo_t*>(
                           pdf->createHistogram("htmp_nominal",
                                                *m_ws.var(m_fitFunction.obs),
                                                RooFit::Binning(nbins, xmin, xmax)
                                               )
                        );
   if (!h_nominal) throw std::runtime_error("NOMINAL histogram not created");
   if (h_nominal->Integral()) h_nominal->Scale(area / h_nominal->Integral());

   // loop over fit parameters, vary each of them up/down by 1 sigma, and obtain histograms
   std::vector<Histo_t*> variations;

   TIter nextParam(pdf->getParameters(dh_bkg)->createIterator());
   RooAbsArg *arg = nullptr;

   while ((arg  = dynamic_cast<RooAbsArg*>(nextParam()))) {
      // vary only roorealvars...
      RooRealVar *rrv = dynamic_cast<RooRealVar*>(arg);
      if (rrv) {
         const TString hname_prefix = "htmp_" + TString(rrv->GetName());
         m_ws.loadSnapshot(nominal_snap);
         if (rrv->isConstant() == kFALSE) {
            m_ws.var(rrv->GetName())->setVal(rrv->getVal() + rrv->getErrorHi()); // up
            Histo_t *h_up = dynamic_cast<Histo_t*>(
                               pdf->createHistogram(hname_prefix + "_up",
                                                    *m_ws.var(m_fitFunction.obs),
                                                    RooFit::Binning(nbins, xmin, xmax)
                                                   )
                            );
            if (!h_up) throw std::runtime_error("UP histogram not created");
            if (h_up->Integral()) h_up->Scale(area / h_up->Integral());
            variations.push_back(h_up);

            m_ws.loadSnapshot(nominal_snap);
            m_ws.var(rrv->GetName())->setVal(rrv->getVal() + rrv->getErrorLo()); // down (already has sign)
            Histo_t *h_down = dynamic_cast<Histo_t*>(
                                 pdf->createHistogram(hname_prefix + "_down",
                                                      *m_ws.var(m_fitFunction.obs),
                                                      RooFit::Binning(nbins, xmin, xmax)
                                                     )
                              );
            if (!h_down) throw std::runtime_error("UP histogram not created");
            if (h_down->Integral()) h_down->Scale(area / h_down->Integral());
            variations.push_back(h_down);
         } // RRV is not constant
      } // is a RRV
   } // loop over pdf parameters (i.e. all but the observable)

   // obtain the nominal and +/- 1 sigma variations
   std::vector<Histo_t *> final_histos = createHistos(h_nominal, variations);
   if (final_histos.size() != 3) throw std::logic_error("Expecting 3 histograms");

   // save histograms
   hc.addHistogram(final_histos[0], channel, bkgName(), hc.tagNominal());
   hc.addHistogram(final_histos[1], channel, bkgName(), varName() + "_up");
   hc.addHistogram(final_histos[2], channel, bkgName(), varName() + "_down");

   // delete owned stuff
   delete h_bkg;
   delete h_nominal;
   for (UInt_t i = 0; i < variations.size(); i++) delete variations[i];
   for (UInt_t i = 0; i < final_histos.size(); i++) delete final_histos[i];
}

std::vector<RF::Histo_t*> RF::BackgroundTemplateAdder::createHistos(Histo_t *nominal, std::vector<Histo_t*> &variations)
{
   // NOTE: do NOT use input histograms after calling this function!
   //
   std::vector<RF::Histo_t*> result;

   // prepare outputs
   result.push_back(dynamic_cast<RF::Histo_t*>(nominal->Clone()));   // will remain untouched
   result.push_back(dynamic_cast<RF::Histo_t*>(nominal->Clone()));   // will become UP
   result.push_back(dynamic_cast<RF::Histo_t*>(nominal->Clone()));   // will become DOWN

   // bin-by-bin preparation of up and down variations
   for (Int_t i = 1; i <= result[0]->GetNbinsX(); i++) {
      const Double_t central = result[0]->GetBinContent(i);
      Double_t sum_sq_up(0), sum_sq_down(0);

      for (UInt_t j = 0; j < variations.size(); j++) {
         const Double_t diff = variations[j]->GetBinContent(i) - central;

         if (diff > 0) sum_sq_up += diff * diff;
         else          sum_sq_down += diff * diff;
      }

      result[1]->SetBinContent(i, central + TMath::Sqrt(sum_sq_up));
      result[2]->SetBinContent(i, central - TMath::Sqrt(sum_sq_down));
   } // loop over visible bins

   return result;
}
