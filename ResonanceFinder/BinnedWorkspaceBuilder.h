#ifndef __RF_BinnedWorkspaceBuilder_h__
#define __RF_BinnedWorkspaceBuilder_h__

///
/// \brief BinnedWorkspaceBuilder - build HistFactory-based workspace
/// \author Valerio Ippolito - Harvard University
/// \date Mar 23, 2015
///
/// BinnedWorkspaceBuilder is the binned workspace builder implementation
/// of IWorkspaceBuilder
///

#include "ResonanceFinder/IWorkspaceBuilder.h"
#include "ResonanceFinder/HistoCollection.h"

namespace RF {
   class BinnedWorkspaceBuilder : public IWorkspaceBuilder {
   public:
      BinnedWorkspaceBuilder(TString name);
      ~BinnedWorkspaceBuilder();

      virtual void build();
      virtual const char *wsName();
      virtual const char *modelConfigName();

      TString outputFilePrefix() const;
      void setOutputFilePrefix(TString outputFilePrefix);

      void setHistoCollection(HistoCollection &hc);
      HistoCollection &histoCollection();
   private:
      TString m_outputFilePrefix;
      HistoCollection m_hc;
   };
}

#endif
