#ifndef __RF_ILimitTool_h__
#define __RF_ILimitTool_h__

///
/// \brief ILimitTool - interface class for LimitTool objects
/// \author Valerio Ippolito - Harvard University
/// \date Apr 4, 2015
///
/// ILimitTool is the interface for all LimitTool objects.
/// It takes as an input a RooWorkspace, and produces a StatisticalResults
/// object.
///

#include <TString.h>

#include "ResonanceFinder/StatisticalResults.h"

class RooWorkspace;


namespace RF {
   class ILimitTool {
   public:
      inline ILimitTool(TString name);
      inline virtual ~ILimitTool() = 0;

      inline TString name() const;
      inline void setName(TString name);
      inline TString modelConfig() const;
      inline void setModelConfig(TString name);
      inline TString dataName() const;
      inline void setDataName(TString name);
      inline Double_t poi_min() const;
      inline Double_t poi_max() const;
      inline Int_t poi_steps() const;
      inline void setPOISteps(Int_t nsteps, Double_t min, Double_t max);

      virtual StatisticalResults run(RooWorkspace *ws) = 0;

   private:
      TString m_name;
      TString m_modelConfig;
      TString m_dataName;
      Double_t m_poi_min;
      Double_t m_poi_max;
      Int_t m_poi_steps;
   };
}

RF::ILimitTool::ILimitTool(TString name) : m_name(name), m_modelConfig("ModelConfig"), m_dataName("obsData"), m_poi_min(0), m_poi_max(10), m_poi_steps(20)
{
}

RF::ILimitTool::~ILimitTool()
{
}

TString RF::ILimitTool::name() const
{
   return m_name;
}

void RF::ILimitTool::setName(TString name)
{
   m_name = name;
}

TString RF::ILimitTool::modelConfig() const
{
   return m_modelConfig;
}

void RF::ILimitTool::setModelConfig(TString name)
{
   m_modelConfig = name;
}

TString RF::ILimitTool::dataName() const
{
   return m_dataName;
}

void RF::ILimitTool::setDataName(TString name)
{
   m_dataName = name;
}

Double_t RF::ILimitTool::poi_min() const
{
   return m_poi_min;
}

Double_t RF::ILimitTool::poi_max() const
{
   return m_poi_max;
}

Int_t RF::ILimitTool::poi_steps() const
{
   return m_poi_steps;
}

void RF::ILimitTool::setPOISteps(Int_t nsteps, Double_t min, Double_t max)
{
   m_poi_steps = nsteps;
   m_poi_min = min;
   m_poi_max = max;
}

#endif
