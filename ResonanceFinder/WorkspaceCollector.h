#ifndef __RF_WorkspaceCollector_h__
#define __RF_WorkspaceCollector_h__

///
/// \brief WorkspaceCollector - class collecting workspaces for each mass point
/// \author Valerio Ippolito - Harvard University
/// \date Apr 5, 2015
///
/// WorkspaceCollector implements the IWorkspaceCollector interface. Its task is
/// to produce a WorkspaceCollection (i.e. a set of RooWorkspace objects
/// mapped to the corresponding resonance mass) out of standardised user input.
/// Files are looked for in the form:
///      source/prefix_MASS.root
/// and must contain a workspace with a given name (same for all files).
///

#include "ResonanceFinder/IWorkspaceCollector.h"

namespace RF {
   class WorkspaceCollector : public IWorkspaceCollector {
   public:
      WorkspaceCollector();
      WorkspaceCollector(TString name);
      ~WorkspaceCollector();

      TString wsName() const;
      void setWsName(TString name);

      void addMassPoint(Double_t point, TString sampleName);

   private:
      TString m_wsName;
   };
}

#endif
