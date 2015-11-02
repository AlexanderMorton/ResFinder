#include "ResonanceFinder/SignalInjector.h"

#include <RooStats/AsymptoticCalculator.h>
#include <RooStats/ModelConfig.h>
#include <RooDataSet.h>
#include <RooRealVar.h>
#include <stdexcept>

RF::SignalInjector::SignalInjector() : m_modelConfig("ModelConfig"), m_mass(0), m_poiValue(0), m_dsetName("asimovData_inj")
{
}

RF::SignalInjector::~SignalInjector()
{
}

TString RF::SignalInjector::modelConfig() const
{
   return m_modelConfig;
}
void RF::SignalInjector::setModelConfig(TString name)
{
   m_modelConfig = name;
}
Double_t RF::SignalInjector::mass() const
{
   return m_mass;
}
void RF::SignalInjector::setMass(Double_t point)
{
   m_mass = point;
}
Double_t RF::SignalInjector::poiValue() const
{
   return m_poiValue;
}
void RF::SignalInjector::setPOIValue(Double_t value)
{
   m_poiValue = value;
}
TString RF::SignalInjector::dsetName() const
{
   return m_dsetName;
}
void RF::SignalInjector::setDsetName(TString name)
{
   m_dsetName = name;
}

RooAbsData *RF::SignalInjector::getDataSet(RooWorkspace *ws)
{
   RooStats::ModelConfig *mc = dynamic_cast<RooStats::ModelConfig*>(ws->obj(m_modelConfig));
   if (!mc) throw std::runtime_error("ModelConfig object not found");

   // take note of the original (first) POI value
   RooRealVar *poi = ws->var(mc->GetParametersOfInterest()->first()->GetName());
   const Double_t poi_old = poi->getVal();

   // set it to desired value and generate
   poi->setVal(m_poiValue);
   RooArgSet tmp;

   // this version profiles NP values on the observed dataset; it is probably not what we want to do for injection,
   // as mu is fixed to the injection value but other signal normalisation parameters are profiled on data
   //RooAbsData *dset = RooStats::AsymptoticCalculator::MakeAsimovData(*ws->data("obsData"), *mc, RooArgSet(*poi), tmp);

   // this version uses nominal NP values -> will inject the "average" mu*S+B sample
   RooArgSet allVars = *(mc->GetPdf()->getVariables());
   RooAbsData *dset = RooStats::AsymptoticCalculator::MakeAsimovData(*mc, allVars, tmp);

   // revert the POI value to its original value
   poi->setVal(poi_old);

   // return
   return dset;
}

void RF::SignalInjector::manipulate(WorkspaceCollection &coll)
{
   ////
   // generate dataset
   ////

   RooWorkspace *w = coll.getWorkspace(m_mass);
   if (!w) throw std::runtime_error("Workspace pointer is void");

   RooDataSet *dset = dynamic_cast<RooDataSet*>(getDataSet(w));
   if (!dset) throw std::runtime_error("Dataset not available");


   ////
   // import it
   ////
   for (auto &kv : coll.workspaces()) {
      if (kv.second->import(*dset, RooFit::Rename(m_dsetName))) throw std::runtime_error("Error importing dataset");
   }
}
