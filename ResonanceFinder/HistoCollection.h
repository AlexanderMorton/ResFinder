#ifndef __RF_HistoCollection_h__
#define __RF_HistoCollection_h__

///
/// \brief HistoCollection - collection of histograms associated to channels,
///        samples and variations
/// \author Valerio Ippolito - Harvard University
/// \date Mar 23, 2015
///
/// HistoCollection is the collection class which maps (channel, sample,
/// variation) to histograms. Histograms are owned by the class.
///

#include <TString.h>
#include <TH1F.h>
#include <map>
#include <unordered_set>
#include <string>

class TDirectory;

namespace RF {
   typedef TH1F Histo_t;
   typedef std::map<TString, Histo_t*> HistoMap_t;

   class HistoCollection {
   public:
      HistoCollection();
      HistoCollection(TString name);
      ~HistoCollection();

      TString name() const;
      void setName(TString name);
      TString tagNominal() const;
      void setTagNominal(TString name);
      TString tagData() const;
      void setTagData(TString name);

      void addHistogram(Histo_t *h, TString channel, TString sample, TString variation);
      void addData(Histo_t *h, TString channel);
      TString getHistoName(TString channel, TString sample, TString variation) const;
      TString getDataName(TString channel) const;
      Histo_t* getHistogram(TString channel, TString sample, TString variation);
      Histo_t* getData(TString channel);
      void persist(TDirectory *file);
      void persist(TString filename);
      void retrieve(TDirectory *file);
      void retrieve(TString filename);
      void reset();
      std::vector<TString> channels();
      std::vector<TString> samples(TString channel);
      HistoMap_t& histos();

   protected:
      TString getHistoName(TString key) const;
      TString getKey(TString channel, TString sample, TString variation) const;
      TString getKeyData(TString channel) const;

   private:
      TString m_name;
      TString m_tagNominal;
      TString m_tagData;
      HistoMap_t m_histos;
      std::unordered_set<std::string> m_channels; // set allows uniqueness
      std::map<TString, std::unordered_set<std::string> > m_samples; // set allows uniqueness
   };
}

#endif

