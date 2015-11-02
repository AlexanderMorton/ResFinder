#include "ResonanceFinder/Sample.h"

#include <RooAbsPdf.h>

RF::Sample::Sample() : m_name(""), m_normByLumi(kFALSE), m_fitFunctionObs(""), m_fitFunctionFactoryCommand(""), m_fitFunctionPdf(0), m_useStatError(kFALSE)
{
}

RF::Sample::Sample(TString name) : m_name(name), m_normByLumi(kFALSE), m_fitFunctionObs(""), m_fitFunctionFactoryCommand(""), m_fitFunctionPdf(0), m_useStatError(kFALSE)
{
}

RF::Sample::~Sample()
{
}

TString RF::Sample::name() const
{
   return m_name;
}

void RF::Sample::setName(TString name)
{
   m_name = name;
}

Bool_t RF::Sample::normByLumi() const
{
   return m_normByLumi;
}

void RF::Sample::setNormByLumi(Bool_t value)
{
   m_normByLumi = value;
}

void RF::Sample::addVariation(TString name)
{
   // _up and _down are automatically appended by Variation(TString name)
   if (m_variations.find(name) != m_variations.end()) throw std::runtime_error("Variation already added");
   m_variations[name] = RF::Variation(name);
}

void RF::Sample::addVariation(TString name, TString nameUp, TString nameDown)
{
  // _up and _down are automatically appended by Variation(TString name)
  if (m_variations.find(name) != m_variations.end()) throw std::runtime_error("Variation already added");
  m_variations[name] = RF::Variation(name);
  m_variations[name].setNames(name, name+nameUp, name+nameDown);
}

RF::Variation &RF::Sample::variation(TString name)
{
   if (m_variations.find(name) == m_variations.end()) throw std::runtime_error("Unknown variation");
   return m_variations[name];
}

void RF::Sample::multiplyBy(TString name, Double_t value)
{
   if (m_factors.find(name) != m_factors.end()) throw std::runtime_error("Factor already added");
   m_factors[name] = RF::MultiplicativeFactor(name, value);
}

void RF::Sample::multiplyBy(TString name, Double_t value, Double_t min, Double_t max)
{
   if (m_factors.find(name) != m_factors.end()) throw std::runtime_error("Factor already added");
   m_factors[name] = RF::MultiplicativeFactor(name, value, min, max);
}

void RF::Sample::multiplyBy(TString name, Double_t value, Double_t min, Double_t max, RF::MultiplicativeFactor::ConstraintType constraint)
{
   if (m_factors.find(name) != m_factors.end()) throw std::runtime_error("Factor already added");
   m_factors[name] = RF::MultiplicativeFactor(name, value, min, max, constraint);
}

RF::MultiplicativeFactor &RF::Sample::factor(TString name)
{
   if (m_factors.find(name) == m_factors.end()) throw std::runtime_error("Unknown factor");
   return m_factors[name];
}

std::vector<TString> RF::Sample::variations()
{
   std::vector<TString> result;
   for (auto & i : m_variations) result.push_back(i.first);
   return result;
}

std::vector<TString> RF::Sample::factors()
{
   std::vector<TString> result;
   for (auto & i : m_factors) result.push_back(i.first);
   return result;
}

void RF::Sample::setFitFunction(TString factory_command, TString obs)
{
   m_fitFunctionFactoryCommand = factory_command;
   m_fitFunctionObs = obs;
   m_fitFunctionPdf = 0;
}

void RF::Sample::setFitFunction(RooAbsPdf *pdf, TString obs)
{
   m_fitFunctionFactoryCommand = "";
   m_fitFunctionObs = obs;
   m_fitFunctionPdf = pdf;
}

TString RF::Sample::getFitFunctionObservable()
{
   return m_fitFunctionObs;
}

RooAbsPdf *RF::Sample::getFitFunctionPdf()
{
   return m_fitFunctionPdf;
}

TString RF::Sample::getFitFunctionFactoryCommand()
{
   return m_fitFunctionFactoryCommand;
}

void RF::Sample::setUseStatError(Bool_t val)
{
   m_useStatError = val;
}

Bool_t RF::Sample::useStatError()
{
   return m_useStatError;
}
