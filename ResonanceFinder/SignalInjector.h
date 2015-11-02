#ifndef __RF_SignalInjector_h__
#define __RF_SignalInjector_h__

///
/// \brief SignalInjector - add a new dataset with injected signal to a WorkspaceCollection
/// \author Valerio Ippolito - Harvard University
/// \date Apr 5, 2015
///
/// SignalInjector is the class which, given an input set of workspaces (WorkspaceCollection),
/// modifies each of the workspaces by adding a new toy dataset with an injected signal.
/// Signal injection is determined by
///   - signal mass
///   - signal strength
///   - set of nuisance parameter values (either nominal, or from best fit to data for the
///     hypothesised mass)
/// A RooAbsData is generated with a standard naming (asimovData_inj), and saved into all workspaces.
///

#include "ResonanceFinder/IWorkspaceManipulator.h"

#include <RooWorkspace.h>

class RooAbsData;

namespace RF {
   class SignalInjector : public IWorkspaceManipulator {
   public:
      SignalInjector();
      ~SignalInjector();

      TString modelConfig() const;
      void setModelConfig(TString name);
      Double_t mass() const;
      void setMass(Double_t point);
      Double_t poiValue() const;
      void setPOIValue(Double_t value);
      TString dsetName() const;
      void setDsetName(TString name);

      virtual void manipulate(WorkspaceCollection &coll);

   protected:
      RooAbsData *getDataSet(RooWorkspace *ws);

      TString m_modelConfig;
      Double_t m_mass;
      Double_t m_poiValue;
      TString m_dsetName;
   };
}

#endif
