#include "ResonanceFinder/WorkspaceCollector.h"
#include <TFile.h>

RF::WorkspaceCollector::WorkspaceCollector()
{
}

RF::WorkspaceCollector::WorkspaceCollector(TString name) : m_wsName("combined")
{
   setName(name);
}

RF::WorkspaceCollector::~WorkspaceCollector()
{
}

TString RF::WorkspaceCollector::wsName() const
{
   return m_wsName;
}

void RF::WorkspaceCollector::setWsName(TString name)
{
   m_wsName = name;
}

void RF::WorkspaceCollector::addMassPoint(Double_t point, TString sampleName)
{
   const TString massString = sampleName;
   const TString fname = TString::Format("%s/%s_%s_%s.root", source().Data(), prefix().Data(), massString.Data(), suffix().Data());
   TFile *file = new TFile(fname);
   if (!file) throw std::runtime_error("Unable to open input WS file");

   RooWorkspace *ws = dynamic_cast<RooWorkspace*>(file->Get(wsName()));
   if (!ws) throw std::runtime_error("Workspace not retrieved");

   getWorkspaceCollection().addWorkspace(ws, point);

   delete file;
}

