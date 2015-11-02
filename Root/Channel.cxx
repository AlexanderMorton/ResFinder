#include "ResonanceFinder/Channel.h"

RF::Channel::Channel() : m_name(""), m_statErrorThreshold(-1), m_hasStatError(kFALSE)
{
}

RF::Channel::Channel(TString name) : m_name(name), m_statErrorThreshold(-1), m_hasStatError(kFALSE)
{
}

RF::Channel::~Channel()
{
}

TString RF::Channel::name() const
{
   return m_name;
}

void RF::Channel::setName(TString name)
{
   m_name = name;
}

void RF::Channel::addSample(TString name)
{
   if (m_samples.find(name) != m_samples.end()) throw std::runtime_error("Sample already defined");
   m_samples[name] = RF::Sample(name);
}

void RF::Channel::addSample(const RF::Sample &sample)
{
   if (m_samples.find(sample.name()) != m_samples.end()) throw std::runtime_error("Sample already defined");
   m_samples[sample.name()] = sample;
}

void RF::Channel::removeSample(TString name)
{
   auto itr = m_samples.find(name);
   if (itr != m_samples.end()) {
      m_samples.erase(itr);
   } else {
      throw std::runtime_error("Attempt at removing a sample which does not exist");
   }
}

RF::Sample &RF::Channel::sample(TString name)
{
   if (m_samples.find(name) == m_samples.end()) throw std::runtime_error("Unknown sample");
   return m_samples[name];
}

std::vector<TString> RF::Channel::samples()
{
   std::vector<TString> result;
   for (auto & i : m_samples) result.push_back(i.first);
   return result;
}

void RF::Channel::setStatErrorThreshold(Double_t threshold)
{
   m_statErrorThreshold = threshold;

   m_hasStatError = (m_statErrorThreshold > 0);
}

Double_t RF::Channel::statErrorThreshold()
{
   return m_statErrorThreshold;
}

Bool_t RF::Channel::hasStatError()
{
   return m_hasStatError;
}
