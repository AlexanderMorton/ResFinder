#ifndef __RF_ITreeRunner_h__
#define __RF_ITreeRunner_h__

///
/// \brief ITreeRunner - base class for AnalysisRunner objects reading from TTree
/// \author Valerio Ippolito - Harvard University
/// \date Mar 23, 2015
///
/// ITreeRunner provides basic info on what to read from a TTree
///

namespace RF {
   class ITreeRunner {
   public:
      inline ITreeRunner();
      inline virtual ~ITreeRunner() = 0;

      inline TString treeObs() const;
      inline void setTreeObs(TString name);
      inline TString treeWeight() const;
      inline void setTreeWeight(TString name);

   private:
      TString m_treeObs;
      TString m_treeWeight;
   };
}

RF::ITreeRunner::ITreeRunner() : m_treeObs(""), m_treeWeight("")
{
}

RF::ITreeRunner::~ITreeRunner()
{
}

TString RF::ITreeRunner::treeObs() const
{
   return m_treeObs;
}

void RF::ITreeRunner::setTreeObs(TString name)
{
   m_treeObs = name;
}

TString RF::ITreeRunner::treeWeight() const
{
   return m_treeWeight;
}

void RF::ITreeRunner::setTreeWeight(TString name)
{
   m_treeWeight = name;
}

#endif
