#include "ResonanceFinder/HistoCollection.h"

#include <TPRegexp.h>
#include <TIterator.h>
#include <TFile.h>
#include <TKey.h>
#include <stdexcept>

RF::HistoCollection::HistoCollection() : m_name(""), m_tagNominal("nominal"), m_tagData("data")
{
}

RF::HistoCollection::HistoCollection(TString name) : m_name(name), m_tagNominal("nominal"), m_tagData("data")
{
}

RF::HistoCollection::~HistoCollection()
{
}

TString RF::HistoCollection::name() const
{
   return m_name;
}

void RF::HistoCollection::setName(TString name)
{
   m_name = name;
}

TString RF::HistoCollection::tagNominal() const
{
   return m_tagNominal;
}

void RF::HistoCollection::setTagNominal(TString name)
{
   m_tagNominal = name;
}

TString RF::HistoCollection::tagData() const
{
   return m_tagData;
}

void RF::HistoCollection::setTagData(TString name)
{
   m_tagData = name;
}

void RF::HistoCollection::addHistogram(RF::Histo_t *h, TString channel, TString sample, TString variation)
{
   // copies input histogram, taking ownership of the copy
   const TString key = getKey(channel, sample, variation);

   if (m_histos.find(key) == m_histos.end()) {
      m_histos[key] = dynamic_cast<RF::Histo_t*>(h->Clone(getHistoName(channel, sample, variation)));
      m_histos[key]->SetDirectory(0);
      m_channels.insert(channel.Data());
      m_samples[channel].insert(sample.Data());
   } else {
      throw std::runtime_error("Histogram already read");
   }
}

void RF::HistoCollection::addData(RF::Histo_t *h, TString channel)
{
   // copies input histogram, taking ownership of the copy
   const TString key = getKeyData(channel);

   if (m_histos.find(key) == m_histos.end()) {
      m_histos[key] = dynamic_cast<RF::Histo_t*>(h->Clone(getDataName(channel)));
      m_histos[key]->SetDirectory(0);
      m_channels.insert(channel.Data());
      m_samples[channel].insert(m_tagData.Data());
   } else {
      throw std::runtime_error("Data histogram already read");
   }
}

TString RF::HistoCollection::getHistoName(TString channel, TString sample, TString variation) const
{
   const TString key = getKey(channel, sample, variation);
   return getHistoName(key);
}

TString RF::HistoCollection::getDataName(TString channel) const
{
   const TString key = getKeyData(channel);
   return getHistoName(key);
}

RF::Histo_t* RF::HistoCollection::getHistogram(TString channel, TString sample, TString variation)
{
   RF::Histo_t *result = nullptr;

   const TString key = getKey(channel, sample, variation);
   auto kv = m_histos.find(key);

   if (kv != m_histos.end()) {
      result = kv->second;
   }

   return result;
}

RF::Histo_t* RF::HistoCollection::getData(TString channel)
{
   RF::Histo_t *result = nullptr;

   const TString key = getKeyData(channel);
   auto kv = m_histos.find(key);

   if (kv != m_histos.end()) {
      result = kv->second;
   }

   return result;
}

void RF::HistoCollection::persist(TDirectory *file)
{
   file->cd();

   for (auto & kv : m_histos) {
      const TString name = getHistoName(kv.first);
      kv.second->SetName(name);
      kv.second->Write(); // keep ownership!
   }

   file->Write();
}

void RF::HistoCollection::persist(TString filename)
{
   TFile *file = new TFile(filename, "RECREATE");
   persist(file);
   delete file;
}

void RF::HistoCollection::retrieve(TDirectory *file)
{
   // takes also ownership of read histograms
   reset();

   TIter nextKey(file->GetListOfKeys());
   TKey* key = nullptr;
   TPMERegexp re("h_([a-zA-Z0-9]+)_([a-zA-Z]+)_([a-zA-Z0-9]+)");
   TPMERegexp re_data("h_([a-zA-Z0-9]+)");

   while ((key = dynamic_cast<TKey*>(nextKey()))) {
      const TString name = key->GetName();
      const Bool_t parsed = (re.Match(name) == 4);
      const Bool_t parsed_data = (re_data.Match(name) == 2);

      if (parsed || parsed_data) {
         RF::Histo_t* h = dynamic_cast<RF::Histo_t*>(key->ReadObj());
         if (h) {
            if (parsed)
               addHistogram(h, re[1], re[2], re[3]); // channel, sample, variation
            else if (parsed_data)
               addData(h, re[1]); // channel
         } else {
            throw std::runtime_error("Unable to access histogram");
         }
      } // naming like histograms we are able to identify
   }
}

void RF::HistoCollection::retrieve(TString filename)
{
   TFile *file = new TFile(filename);
   retrieve(file);
   delete file;
}

void RF::HistoCollection::reset()
{
   for (auto & kv : m_histos) {
      SafeDelete(kv.second);
   }

   m_histos.clear();
}

std::vector<TString> RF::HistoCollection::channels()
{
   std::vector<TString> result;
   for (auto x : m_channels) result.push_back(x);
   return result;
}

std::vector<TString> RF::HistoCollection::samples(TString channel)
{
   std::vector<TString> result;
   for (auto x : m_samples[channel]) result.push_back(x);
   return result;
}

RF::HistoMap_t& RF::HistoCollection::histos()
{
   return m_histos;
}

TString RF::HistoCollection::getHistoName(TString key) const
{
   // note this convention is in sync with ::retrieve
   return "h_" + key;
}

TString RF::HistoCollection::getKey(TString channel, TString sample, TString variation) const
{
   return channel + "_" + sample + "_" + variation;
}

TString RF::HistoCollection::getKeyData(TString channel) const
{
   return channel + "_data";
}
