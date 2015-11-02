#ifndef __RF_IWorkspaceCollector_h__
#define __RF_IWorkspaceCollector_h__

///
/// \brief IWorkspaceCollector - interface for classes collecting workspaces
/// \author Valerio Ippolito - Harvard University
/// \date Apr 5, 2015
///
/// IWorkspaceCollector is the interface for WorkspaceCollector classes, which
/// deal with producing a WorkspaceCollection (i.e. a set of RooWorkspace objects
/// mapped to the corresponding resonance mass) out of user input.
///

#include "ResonanceFinder/WorkspaceCollection.h"
#include <TString.h>

namespace RF {
   class IWorkspaceCollector {
   public:
      inline virtual ~IWorkspaceCollector() = 0;

      inline TString name() const;
      inline void setName(TString name);
      inline TString source() const;
      inline void setSource(TString source);
      inline TString prefix() const;
      inline void setPrefix(TString prefix);
      inline TString suffix() const;
      inline void setSuffix(TString suffix);

      virtual void addMassPoint(Double_t mass, TString sampleName) = 0;
      inline WorkspaceCollection& getWorkspaceCollection();
      inline void reset();

   protected:
      WorkspaceCollection m_coll;

   private:
      TString m_name;
      TString m_source;
      TString m_prefix;
      TString m_suffix;
   };
}

RF::IWorkspaceCollector::~IWorkspaceCollector()
{
}

TString RF::IWorkspaceCollector::name() const
{
   return m_name;
}

void RF::IWorkspaceCollector::setName(TString name)
{
   m_name = name;
}

TString RF::IWorkspaceCollector::source() const
{
   return m_source;
}

void RF::IWorkspaceCollector::setSource(TString source)
{
   m_source = source;
}

TString RF::IWorkspaceCollector::prefix() const
{
   return m_prefix;
}

void RF::IWorkspaceCollector::setPrefix(TString prefix)
{
   m_prefix = prefix;
}

TString RF::IWorkspaceCollector::suffix() const
{
   return m_suffix;
}

void RF::IWorkspaceCollector::setSuffix(TString suffix)
{
   m_suffix = suffix;
}

RF::WorkspaceCollection& RF::IWorkspaceCollector::getWorkspaceCollection()
{
   return m_coll;
}

void RF::IWorkspaceCollector::reset()
{
   getWorkspaceCollection().reset();
}


#endif
