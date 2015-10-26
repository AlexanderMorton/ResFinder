#ifndef __RF_WorkspaceCollection_h__
#define __RF_WorkspaceCollection_h__

///
/// \brief WorkspaceCollection - collection of workspaces associated to mass points
/// \author Valerio Ippolito - Harvard University
/// \date Apr 4, 2015
///
/// WorkspaceCollection is the collection class which maps a mass point to a
/// pointer to RooWorkspace. Workspaces are owned by the class.
///

#include <TString.h>
#include <RooWorkspace.h>
#include <map>
#include <unordered_set>
#include <string>

class TDirectory;

namespace RF {
   typedef std::map<Double_t, RooWorkspace*> WorkspaceMap_t;

   class WorkspaceCollection {
   public:
      WorkspaceCollection();
      WorkspaceCollection(TString name);
      ~WorkspaceCollection();

      TString name() const;
      void setName(TString name);
      TString massFormatString() const;
      void setMassFormatString(TString string);

      void addWorkspace(RooWorkspace *w, Double_t point);
      RooWorkspace* getWorkspace(Double_t point);
      void persist(TDirectory *file);
      void persist(TString filename);
      void retrieve(TDirectory *file);
      void retrieve(TString filename);
      void reset();
      std::vector<Double_t> points();
      WorkspaceMap_t& workspaces();

   protected:
      TString getWorkspaceName(Double_t point);

   private:
      TString m_name;
      TString m_massFormatString;
      WorkspaceMap_t m_workspaces;
      std::unordered_set<Double_t> m_points;
   };
}

#endif
