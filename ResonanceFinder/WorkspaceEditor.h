#ifndef __RF_WorkspaceEditor_h__
#define __RF_WorkspaceEditor_h__

///
/// \brief WorkspaceEditor - Tools for editing existing workspaces
/// \author Ben Wynne - University of Edinburgh
/// \date May 29, 2015
///
/// Editing a workspace that already exists is tricky
/// Put helpful methods here
///

#include "RooWorkspace.h"

class WorkspaceEditor {
public:
   //Copy all iterable components from one workspace to another
   //This will preserve existing components in the target workspace
   //Probably the neatest way to replace a workspace component: put it in the target first
   static void CopyWorkspaceContents(RooWorkspace * InputWorkspace, RooWorkspace * OutputWorkspace);
};

#endif
