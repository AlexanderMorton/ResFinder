#ifndef __RF_IHistoManipulator_h__
#define __RF_IHistoManipulator_h__

///
/// \brief IHistoManipulator - interface for classes modifying a HistoCollection
/// \author Valerio Ippolito - Harvard University
/// \date Mar 23, 2015
///
/// IHistoManipulator is the most primitive interface for classes which
/// manipulate HistoCollection objects, for example adding smearing/variations
/// which are produced due to explicit user input, and not on samples retrieved
/// from files. The idea is then
///
///      HistoCollection --->  [BLACKBOX] ===> HistoCollection (modified)
///                                ^
///                                |
///                                |
///                              config
///

#include "ResonanceFinder/HistoCollection.h"

namespace RF {
   class IHistoManipulator {
   public:
      inline virtual ~IHistoManipulator() = 0;
      virtual void manipulate(HistoCollection &hc) = 0;
   };
}

RF::IHistoManipulator::~IHistoManipulator()
{
}

#endif

