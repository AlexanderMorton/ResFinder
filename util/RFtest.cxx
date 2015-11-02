#include <iostream>
#include "ResonanceFinder/TreeHistoCollector.h"
#include "ResonanceFinder/BinnedWorkspaceBuilder.h"
#include "ResonanceFinder/BackgroundTemplateAdder.h"
#include "ResonanceFinder/VVJJAnalysisRunner.h"

#include <TTree.h>
#include <RooWorkspace.h>
#include <RooAbsPdf.h>
#include <RooDataSet.h>
#include <TFile.h>
#include <RooFormulaVar.h>
#include <RooRealVar.h>

using namespace RF;

void createFakeTrees(TString dir, Bool_t doBkg)
{
   // make it simple
   RooDataSet::setDefaultStorageType(RooAbsData::Tree);

   // create S/B pdfs and datasets
   RooWorkspace w("w", "w");
   w.factory("Gaussian::s(x[1050,3550], mu[2000], sigma[70])");
   w.factory("CEXPR::b('pow(1 - x/sqrts, p2-xi*p3) * pow(x/sqrts, -p3)', {x,sqrts[13000],xi[4.3],p2[5.28833e+01,-100,100],p3[-1.72355e+00,-100,100]})");
   w.defineSet("vars", "x");
   RooDataSet *sig = w.pdf("s")->generate(*w.set("vars"), 10000);
   RooDataSet *bkg = w.pdf("b")->generate(*w.set("vars"), 10000);

   // create up/down variations
   w.var("sigma")->setVal(80);
   RooDataSet *sig_ptr_up = w.pdf("s")->generate(*w.set("vars"), 10000);
   w.var("sigma")->setVal(60);
   RooDataSet *sig_ptr_down = w.pdf("s")->generate(*w.set("vars"), 10000);

   w.var("p2")->setVal(68);
   w.var("p3")->setVal(2);
   RooDataSet *bkg_both_up = w.pdf("b")->generate(*w.set("vars"), 10000);
   w.var("p2")->setVal(35);
   w.var("p3")->setVal(-6);
   RooDataSet *bkg_both_down = w.pdf("b")->generate(*w.set("vars"), 10000);

   // rescale them to lumi (10/fb, rough numbers)
   const Double_t sig_w = 17 / Double_t(sig->numEntries());
   const Double_t bkg_w = 2650 / Double_t(bkg->numEntries());
   RooFormulaVar w_s("weight", "event weight", TString::Format("%.4f", sig_w).Data(), RooArgList(*w.set("vars")));
   RooFormulaVar w_b("weight", "event weight", TString::Format("%.4f", bkg_w).Data(), RooArgList(*w.set("vars")));
   sig->addColumn(w_s);
   bkg->addColumn(w_b);
   sig_ptr_up->addColumn(w_s);
   sig_ptr_down->addColumn(w_s);
   bkg_both_up->addColumn(w_b);
   bkg_both_down->addColumn(w_b);

   TFile *f_sig  = new TFile(dir + "/signal_SR.root", "RECREATE");
   TFile *f_bkg  = new TFile(dir + "/background_SR.root", "RECREATE");
   TFile *f_data = new TFile(dir + "/data_SR.root", "RECREATE");

   TTree *t_sig = dynamic_cast<TTree*>(sig->tree()->Clone("nominal"));
   t_sig->SetDirectory(f_sig);
   TTree *t_bkg = dynamic_cast<TTree*>(bkg->tree()->Clone("nominal"));
   t_bkg->SetDirectory(f_bkg);
   TTree *t_data = dynamic_cast<TTree*>(bkg->tree()->Clone("nominal"));
   t_data->SetDirectory(f_data);

   TTree *t_sig_ptr_up = dynamic_cast<TTree*>(sig_ptr_up->tree()->Clone("ptr_up"));
   t_sig_ptr_up->SetDirectory(f_sig);
   TTree *t_sig_ptr_down = dynamic_cast<TTree*>(sig_ptr_down->tree()->Clone("ptr_down"));
   t_sig_ptr_down->SetDirectory(f_sig);
   TTree *t_bkg_both_up = dynamic_cast<TTree*>(bkg_both_up->tree()->Clone("both_up"));
   t_bkg_both_up->SetDirectory(f_bkg);
   TTree *t_bkg_both_down = dynamic_cast<TTree*>(bkg_both_down->tree()->Clone("both_down"));
   t_bkg_both_down->SetDirectory(f_bkg);

   f_sig->Write();
   if (doBkg) f_bkg->Write();
   f_data->Write();
   delete f_sig;
   delete f_bkg;
   delete f_data;
   delete sig;
   delete bkg;
   delete sig_ptr_up;
   delete sig_ptr_down;
   delete bkg_both_up;
   delete bkg_both_down;
}

int main(int argc, char **argv)
{
   Bool_t fit(kFALSE);
   Bool_t manual(kFALSE);
   if (argc > 2 &&
       (TString(argv[1]) == "gen" || TString(argv[1]) == "fit") &&
       (TString(argv[2]) == "manual" || TString(argv[2]) == "auto")
      ) {
      fit = (TString(argv[1]) == "fit");
      manual = (TString(argv[2]) == "manual");
   } else {
      std::cout << "Usage: " << argv[0] << " <gen|fit> <manual|auto>" << std::endl;
      return 0;
   }

   if (manual) {
      createFakeTrees("test_v1", !fit);

      TreeHistoCollector c("c");
      c.setSource("test_v1");
      c.setObservable("x");
      c.setWeight("weight");
      c.setNbins(25);
      c.setObsMin(1050);
      c.setObsMax(3550);

      // config part, file? script?
      c.addChannel("SR", "weight != 0");
      c.addSample("SR", "signal");
      if (!fit) c.addSample("SR", "background");

      // get histos
      HistoCollection &hc = c.getHistoCollection();

      if (fit) {
         BackgroundTemplateAdder bta;
         bta.setChannel("SR");
         bta.setBkgName("background");
         bta.defineFitFunction("CEXPR::b('pow(1 - x/sqrts, p2-xi*p3) * pow(x/sqrts, -p3)', {x[1050,3550],sqrts[13000],xi[4.3],p2[5.28833e+01,-100,100],p3[-1.72355e+00,-100,100]})", "x");
         bta.manipulate(hc);
      }

      hc.persist("test.root");

      // build workspace
      BinnedWorkspaceBuilder wb("wb");
      wb.setDest("ws");
      wb.setOutputFilePrefix("ws/test");

      wb.setHistoCollection(hc);

      wb.setPOI("mu");
      wb.addChannel("SR");
      wb.channel("SR").addSample("signal");
      wb.channel("SR").sample("signal").multiplyBy("mu", 1.0, 0, 20);
      wb.channel("SR").sample("signal").addVariation("ptr");
      wb.channel("SR").addSample("background");
      //wb.channel("SR").sample("background").multiplyBy("rescale", 2.0);
      if (!fit) wb.channel("SR").sample("background").addVariation("both");
      else wb.channel("SR").sample("background").addVariation("fit");

      wb.build();
   } else {
      createFakeTrees("test_v1/VVJJ_WPRIME/data/itest01", !fit);

      VVJJAnalysisRunner vvjj("VVJJ_WPRIME");
      vvjj.setTreeObs("x");
      vvjj.setTreeWeight("weight");
      vvjj.setNbins(25);
      vvjj.setObsMin(1050);
      vvjj.setObsMax(3550);
      vvjj.setReleaseDir("test_v1");
      vvjj.setInputListTag("itest01");
      vvjj.setOutputWSTag("wtest01");

      vvjj.addChannel("SR", ""); // no need for selection criteria
      vvjj.channel("SR").addSample("signal");
      vvjj.channel("SR").sample("signal").setNormByLumi(kTRUE);
      vvjj.channel("SR").sample("signal").multiplyBy("mu", 1.0, 0, 20);
      vvjj.channel("SR").sample("signal").addVariation("ptr");
      vvjj.channel("SR").addSample("background");
      vvjj.channel("SR").sample("background").setNormByLumi(kFALSE);
      if (!fit) vvjj.channel("SR").sample("background").addVariation("both");
      else vvjj.channel("SR").sample("background").addVariation("fit");

      vvjj.addSignal("signal", 2000.0); // mass tag not used at the moment
      vvjj.setPOI("mu");

//    vvjj.setInjectionSample("signal");
//    vvjj.setInjectionStrength(4.5);


      vvjj.run();
   }

   return 0;
}
