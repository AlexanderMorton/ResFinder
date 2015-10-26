#ifndef __RF_IHistoCollector_h__
#define __RF_IHistoCollector_h__

///
/// \brief IHistoCollector - interface for classes collecting histograms
/// \author Valerio Ippolito - Harvard University
/// \date Mar 23, 2015
///
/// IHistoCollector is the most primitive interface for classes which
/// produce histograms out of an input file, whatever its content.
/// They basically take a set of config options (including input files) and
/// produce a collection of histograms:
///
///      (channel, samplename, file) x N
///                      +                     ---> [BLACKBOX] ===> HistoCollection
///                    source
///
/// The only assumptions are made on the input are that:
///   1. a rigid directory structure is present (in the sense that all files lie
///      under an unique source)
///   2. a single file is associated to a single sample, and contains input
///      (for example trees, or histograms) associated to many variations (e.g.
///      nominal, systematics) of the same sample
///   3. an additional tag, "channel" is added, to determine the channel each
///      individual input is associated to; for example, a region (SR/CR): the
///      general case is "a set of inputs statistically independent from each other"
///

#include <TString.h>
#include "ResonanceFinder/HistoCollection.h"

namespace RF {
   class IHistoCollector {
   public:
      inline virtual ~IHistoCollector() = 0;

      inline TString name() const;
      inline void setName(TString name);
      inline TString source() const;
      inline void setSource(TString source);

      virtual void addSample(TString channel, TString sample) = 0;
      inline HistoCollection& getHistoCollection();
      inline void reset();

   protected:
      HistoCollection m_hc;

   private:
      TString m_name;
      TString m_source;
   };
}

RF::IHistoCollector::~IHistoCollector()
{
}

TString RF::IHistoCollector::name() const
{
   return m_name;
}

void RF::IHistoCollector::setName(TString name)
{
   m_name = name;
}

TString RF::IHistoCollector::source() const
{
   return m_source;
}

void RF::IHistoCollector::setSource(TString source)
{
   m_source = source;
}

RF::HistoCollection& RF::IHistoCollector::getHistoCollection()
{
   return m_hc;
}

void RF::IHistoCollector::reset()
{
   getHistoCollection().reset();
}


#endif

