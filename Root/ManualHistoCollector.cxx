#include "ResonanceFinder/ManualHistoCollector.h"

#include <TFile.h>
#include <TIterator.h>
#include <TKey.h>
#include <stdexcept>
#include <iostream>

RF::ManualHistoCollector::ManualHistoCollector()
{
}

RF::ManualHistoCollector::ManualHistoCollector(TString name)
{
   setName(name);
}

RF::ManualHistoCollector::~ManualHistoCollector()
{
}

void RF::ManualHistoCollector::addSample(TString channel, TString sample)
{
   // input from files containing histograms
   const Bool_t isData = (sample == getHistoCollection().tagData());
   const TString fname = source() + "/" + sample + "_" + channel + ".root";

   TFile *file = new TFile(fname);
   if (!file) throw std::runtime_error("File not found");

   TIter nextKey(file->GetListOfKeys());
   TKey* key = nullptr;

   while ((key = dynamic_cast<TKey*>(nextKey()))) {
      const TString variation = key->GetName();

      RF::Histo_t* h = dynamic_cast<RF::Histo_t*>(key->ReadObj());
      if (h) {

         if (isData == kFALSE) {
            // non-data histogram -> save always
            getHistoCollection().addHistogram(h, channel, sample, variation);
         } else {
            // data histogram: save only if nominal
            if (variation == getHistoCollection().tagNominal()) getHistoCollection().addData(h, channel);
            else throw std::logic_error("Data file contains input other than nominal");
         }
      } // we found a histogram
      // we didn't take ownership of the histogram, hence we don't delete it
   } // loop over file content (= variations, if it contains only histograms)

   delete file;
}

void RF::ManualHistoCollector::addSample(TString channel, TString sample, TString variation, TString hname, TString fname)
{
   const Bool_t isData = (sample == getHistoCollection().tagData());
   if (isData && variation != getHistoCollection().tagNominal()) throw std::logic_error("You are calling the manual version of addSample for data with a variation other than nominal");

   const TString full_fname = source() + "/" + fname;

   TFile *file = new TFile(full_fname);
   if (!file) throw std::runtime_error("File not found");

   RF::Histo_t* h = dynamic_cast<RF::Histo_t*>(file->Get(hname));
   if (!h) throw std::runtime_error("Histogram not found");

   if (isData == kFALSE)
      getHistoCollection().addHistogram(h, channel, sample, variation);
   else
      getHistoCollection().addData(h, channel);

   delete file;
}
