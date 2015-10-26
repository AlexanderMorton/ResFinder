#include "ResonanceFinder/TreeHistoCollector.h"

#include <TTreeFormula.h>
#include <TTree.h>
#include <TFile.h>
#include <TIterator.h>
#include <TKey.h>
#include <stdexcept>
#include <iostream>

RF::TreeHistoCollector::TreeHistoCollector() : m_obs(""), m_weight(""), m_nbins(0), m_obs_min(-1), m_obs_max(-1), m_bins(nullptr), m_use_bins(kFALSE)
{
}

RF::TreeHistoCollector::TreeHistoCollector(TString name) : m_obs(""), m_weight(""), m_nbins(0), m_obs_min(-1), m_obs_max(-1), m_bins(nullptr), m_use_bins(kFALSE)
{
   setName(name);
}

RF::TreeHistoCollector::~TreeHistoCollector()
{
}

TString RF::TreeHistoCollector::observable() const
{
   return m_obs;
}

void RF::TreeHistoCollector::setObservable(TString var)
{
   m_obs = var;
}

TString RF::TreeHistoCollector::weight() const
{
   return m_weight;
}

void RF::TreeHistoCollector::setWeight(TString var)
{
   m_weight = var;
}

Int_t RF::TreeHistoCollector::nbins() const
{
   return m_nbins;
}

void RF::TreeHistoCollector::setNbins(Int_t nbins)
{
   m_nbins = nbins;
}

Double_t RF::TreeHistoCollector::obsMin() const
{
   return m_obs_min;
}

void RF::TreeHistoCollector::setObsMin(Double_t min)
{
   m_obs_min = min;
}

Double_t RF::TreeHistoCollector::obsMax() const
{
   return m_obs_max;
}

void RF::TreeHistoCollector::setObsMax(Double_t max)
{
   m_obs_max = max;
}

void RF::TreeHistoCollector::setBins(Double_t* bins)
{
   m_bins = bins;
   m_use_bins =  kTRUE;
}

void RF::TreeHistoCollector::addChannel(TString channel, TString formula)
{
   m_selection[channel] = formula;
   addData(channel);
}

std::vector<TString> RF::TreeHistoCollector::getChannels()
{
   std::vector<TString> result;

   for (auto & kv : m_selection) {
      result.push_back(kv.first);
   }

   return result;
}

void RF::TreeHistoCollector::addSample(TString channel, TString sample)
{
   addSample(channel, sample, kFALSE);
}

void RF::TreeHistoCollector::addData(TString channel)
{
   addSample(channel, getHistoCollection().tagData(), kTRUE);
}

void RF::TreeHistoCollector::addSample(TString channel, TString sample, Bool_t isData)
{
   // here is the relevant part
   const TString filename = source() + "/" + sample + "_" + channel + ".root";

   TFile* file = new TFile(filename);
   if (!file) throw std::runtime_error("File not found");

   TIter nextKey(file->GetListOfKeys());
   TKey* key = nullptr;

   while ((key = dynamic_cast<TKey*>(nextKey()))) {
      const TString variation = key->GetName();

      if (TString(key->GetClassName()) == "TTree") {
         TTree *tree = dynamic_cast<TTree*>(key->ReadObj());
         RF::Histo_t *h = processTree(tree, channel);

         // protection against empty input
         if (h->Integral() == 0) {
            const TString msg = "Histogram integral for channel " + channel + " sample " + sample + " in file " + filename + " is empty, please check input tree " + variation;
            throw std::runtime_error(msg.Data());
         }

         if (isData == kFALSE) {
            // non-data histogram -> save always
            getHistoCollection().addHistogram(h, channel, sample, variation);
         } else {
            // data histogram: save only if nominal
            if (variation == getHistoCollection().tagNominal()) getHistoCollection().addData(h, channel);
            else throw std::logic_error("Data file contains input other than nominal");
         }

         delete h; // we took ownership
      }
   } // loop over variations

   delete file;
}

RF::Histo_t *RF::TreeHistoCollector::processTree(TTree *tree, TString channel)
{
   // this function returns also histogram ownership
   // TODO: use shared_ptr?
   // five decimal digits for min, max
   const TString hname = "tmpXjjDD"; // TODO: make unique?
   RF::Histo_t *h = nullptr; // this is the filled histogram

   const Bool_t useVariableBins = (m_use_bins);
   if (!useVariableBins) {
      h = new RF::Histo_t(hname, hname, nbins(), obsMin(), obsMax());
   } else {
      h = new RF::Histo_t(hname, hname, nbins() , m_bins) ;
      // we could also use sizeof(m_bins)/sizeof(Double_t), but this is cleaner
   }


// const TString what = observable() + ">>+" + hname;

   const TString sel = m_selection[channel];
// TString how = weight(); // try only weight
// if (how == "") how = sel; // if no weight specified, try only selection
// else if (sel != "") how = "(" + how + ")*(" + sel + ")"; // if weight specified and also selection, take product

// tree->Draw(what, how, "goff");
   Float_t tree_obs(0), tree_weight(0);
   Double_t tree_obs_d(0), tree_weight_d(0);
   Bool_t isFloat(kFALSE);
   if (tree->SetBranchAddress(observable(), &tree_obs) != TTree::kMatch) {
      if (tree->SetBranchAddress(observable(), &tree_obs_d) != TTree::kMatch) {
         throw std::runtime_error("Unable to attach to observable branch");
      } else {
         isFloat = kTRUE;
      }
   }
   if (tree->SetBranchAddress(weight(), &tree_weight) != TTree::kMatch) {
      if (tree->SetBranchAddress(weight(), &tree_weight_d) != TTree::kMatch) {
         throw std::runtime_error("Unable to attach to weight branch");
      } else {
         isFloat = kTRUE;
      }
   }
   TTreeFormula *tree_sel(nullptr);
   if (sel != "") {
      tree_sel = new TTreeFormula("tree_sel", sel, tree);
      tree_sel->GetNdata();
   }

   for (Long64_t entry = 0; entry < tree->GetEntries(); entry++) {
      if (tree->GetEntry(entry) == -1) throw std::runtime_error("Error reading tree");

      Bool_t fill = (sel == "" || tree_sel->EvalInstance());
      if (fill) {
         if (isFloat) {
            Float_t thisWeight = tree_weight_d;
            h->Fill(tree_obs_d, thisWeight);
         } else {
            Float_t thisWeight = tree_weight;
            h->Fill(tree_obs, thisWeight);
         }
      }
   }

   delete tree_sel;

   if (!useVariableBins) {

      h->SetDirectory(0);
      return h;

   } else {
      // remap

      const Int_t nb = h->GetNbinsX();

      TString newtitle = (TString)h->GetTitle() + "_remap";
      RF::Histo_t* hh = new RF::Histo_t(newtitle, newtitle, nb, 0., 1.); // using [0,1] as range (instead of m_obs_min,max) is safer to make sure people realise this is a remap

      for (int b = 0; b <= nb + 1; b++) { // do both under and overflow

         hh->SetBinContent(b, h->GetBinContent(b));
         hh->SetBinError(b, h->GetBinError(b));

      }
      SafeDelete(h);

      hh->SetDirectory(0);
      return hh;
   }

}
