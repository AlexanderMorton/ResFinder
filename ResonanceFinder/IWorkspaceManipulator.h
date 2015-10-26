#ifndef __RF_IWorkspaceManipulator_h__
#define __RF_IWorkspaceManipulator_h__

///
/// \brief IWorkspaceManipulator - interface for classes modifying a WorkspaceCollection
/// \author Valerio Ippolito - Harvard University
/// \date Apr 5, 2015
///
/// IWorkspaceManipulator is the most primitive interface for classes which
/// manipulate WorkspaceCollection objects, for example adding a new observed dataset
/// containing an injected signal.
///
///      WorkspaceCollection --->  [BLACKBOX] ===> WorkspaceCollection (modified)
///                                ^
///                                |
///                                |
///                              config
///

#include "ResonanceFinder/WorkspaceCollection.h"

namespace RF {
   class IWorkspaceManipulator {
   public:
      inline virtual ~IWorkspaceManipulator() = 0;
      virtual void manipulate(WorkspaceCollection &coll) = 0;
   };
}

RF::IWorkspaceManipulator::~IWorkspaceManipulator()
{
}

#endif

