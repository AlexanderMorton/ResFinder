#include "ResonanceFinder/WorkspaceCollection.h"

#include <TPRegexp.h>
#include <TIterator.h>
#include <TFile.h>
#include <TKey.h>
#include <RooStats/ModelConfig.h>
#include <stdexcept>

RF::WorkspaceCollection::WorkspaceCollection() : m_name(""), m_massFormatString("%.3f")
{
}

RF::WorkspaceCollection::WorkspaceCollection(TString name) : m_name(name), m_massFormatString("%.3f")
{
}

RF::WorkspaceCollection::~WorkspaceCollection()
{
}

TString RF::WorkspaceCollection::name() const
{
   return m_name;
}

void RF::WorkspaceCollection::setName(TString name)
{
   m_name = name;
}

TString RF::WorkspaceCollection::massFormatString() const
{
   return m_massFormatString;
}

void RF::WorkspaceCollection::setMassFormatString(TString string)
{
   m_massFormatString = string;
}

void RF::WorkspaceCollection::addWorkspace(RooWorkspace *w, Double_t point)
{
   // copies input workspace, taking ownership of the copy

   const Double_t key = point;

   if (m_workspaces.find(key) == m_workspaces.end()) {
      //m_workspaces[key] = new RooWorkspace(*w);
      m_workspaces[key] = dynamic_cast<RooWorkspace*>(w->Clone());
      m_workspaces[key]->SetTitle(getWorkspaceName(point));

      // hack: replace ModelConfig objects with copies, to make sure links between
      // them and the workspace preserved
      // see https://sft.its.cern.ch/jira/browse/ROOT-7237

      for (auto &itr : w->allGenericObjects()) {
         RooStats::ModelConfig *mc = dynamic_cast<RooStats::ModelConfig*>(itr);

         if (mc) {
            // first, create an empty ModelConfig linked to this workspace
            const TString mcName = mc->GetName();
            RooStats::ModelConfig tmp(mcName, m_workspaces[key]);

            // import it, replacing the original one
            m_workspaces[key]->import(tmp, kTRUE);

            // retrieve a pointer to the new and the old ModelConfigs
            RooStats::ModelConfig *hook = dynamic_cast<RooStats::ModelConfig*>(m_workspaces[key]->obj(mcName));

            // fill manually this ModelConfig
            if (mc->GetConditionalObservables()) hook->SetConditionalObservables(*mc->GetConditionalObservables());
            if (mc->GetConstraintParameters()) hook->SetConstraintParameters(*mc->GetConstraintParameters());
            if (mc->GetGlobalObservables()) hook->SetGlobalObservables(*mc->GetGlobalObservables());
            if (mc->GetNuisanceParameters()) hook->SetNuisanceParameters(*mc->GetNuisanceParameters());
            if (mc->GetObservables()) hook->SetObservables(*mc->GetObservables());
            if (mc->GetParametersOfInterest()) hook->SetParametersOfInterest(*mc->GetParametersOfInterest());
            if (mc->GetPdf()) hook->SetPdf(*mc->GetPdf());
            if (mc->GetPriorPdf()) hook->SetPriorPdf(*mc->GetPriorPdf());
            if (mc->GetProtoData()) hook->SetProtoData(*mc->GetProtoData());
            if (mc->GetSnapshot()) hook->SetSnapshot(*mc->GetSnapshot());
            if (hook->GetWS() != m_workspaces[key]) throw std::logic_error("Fix to ModelConfig copying does not work");
         } // ModelConfig found
      }

      m_points.insert(point);
   } else {
      throw std::runtime_error("Workspace already read");
   }
}

TString RF::WorkspaceCollection::getWorkspaceName(Double_t point)
{
   const TString result = TString::Format("ws_" + m_massFormatString, point).ReplaceAll(".", "p");
   return result;
}

RooWorkspace* RF::WorkspaceCollection::getWorkspace(Double_t point)
{
   RooWorkspace *result = nullptr;

   const Double_t key = point;
   auto kv = m_workspaces.find(key);

   if (kv != m_workspaces.end()) {
      result = kv->second;
   }

   return result;
}

void RF::WorkspaceCollection::persist(TDirectory *file)
{
   file->cd();

   for (auto & kv : m_workspaces) {
      const TString name = getWorkspaceName(kv.first);
      kv.second->SetName(name);
      kv.second->Write(); // keep ownership!
   }

   file->Write();
}

void RF::WorkspaceCollection::persist(TString filename)
{
   TFile *file = new TFile(filename, "RECREATE");
   persist(file);
   delete file;
}

void RF::WorkspaceCollection::retrieve(TDirectory *file)
{
   // takes also ownership of read workspaces
   reset();

   TIter nextKey(file->GetListOfKeys());
   TKey* key = nullptr;
   TPMERegexp re("ws_([0-9p]+)");
   TPMERegexp re2("[Cc]ombined");

   while ((key = dynamic_cast<TKey*>(nextKey()))) {
      const TString name = key->GetName();
      const Bool_t parsed1 = (re.Match(name) == 2);
      const Bool_t parsed2 = (re2.Match(name) == 1);

      if (parsed1 || parsed2) {
	RooWorkspace* w = dynamic_cast<RooWorkspace*>(key->ReadObj());
	if (w) {
	  if(parsed1) addWorkspace(w, TString(re[1]).ReplaceAll("p", ".").Atof());
	  else if(parsed2) addWorkspace(w,1000);
	} else {
	  throw std::runtime_error("Unable to access histogram");
	}
      } // naming like histograms we are able to identify
   }
}

void RF::WorkspaceCollection::retrieve(TString filename)
{
   TFile *file = new TFile(filename);
   retrieve(file);
   delete file;
}

void RF::WorkspaceCollection::reset()
{
   for (auto & kv : m_workspaces) {
      SafeDelete(kv.second);
   }

   m_workspaces.clear();
}

std::vector<Double_t> RF::WorkspaceCollection::points()
{
   std::vector<Double_t> result;
   for (auto x : m_points) result.push_back(x);
   return result;
}

RF::WorkspaceMap_t& RF::WorkspaceCollection::workspaces()
{
   return m_workspaces;
}
