#ifndef __RF_Channel_h__
#define __RF_Channel_h__

///
/// \brief Channel - class representing a measurement channel
/// \author Valerio Ippolito - Harvard University
/// \date Mar 23, 2015
///
/// Channel is a class representing a measurement channel. A
/// Channel is composed by many Sample objects.
///

#include "ResonanceFinder/Sample.h"

#include <TString.h>
#include <map>

namespace RF {
   class Channel {
   public:
      Channel();
      Channel(TString name);
      ~Channel();

      TString name() const ;
      void setName(TString name);

      void addSample(TString name);
      void addSample(const Sample & sample);
      void removeSample(TString name);
      Sample &sample(TString name);
      void setStatErrorThreshold(Double_t threshold);
      Double_t statErrorThreshold();
      Bool_t hasStatError();

      std::vector<TString> samples();
   private:
      TString m_name;
      std::map<TString, Sample> m_samples;
      Double_t m_statErrorThreshold;
      Bool_t m_hasStatError;
   };
}

#endif
