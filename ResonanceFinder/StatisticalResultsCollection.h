#ifndef __RF_StatisticalResultsCollection_h__
#define __RF_StatisticalResultsCollection_h__

///
/// \brief StatisticalResultsCollection - collection of statistical results
/// \author Valerio Ippolito - Harvard University
/// \date Apr 4, 2015
///
/// StatisticalResultsCollection is the class which holds StatisticalResults
/// objects associated to each mass point. It allows merging various objects
/// and I/O from a file containing a TTree.
///

#include "ResonanceFinder/StatisticalResults.h"

#include <TString.h>
#include <map>

namespace RF {
   typedef std::map<Double_t, StatisticalResults> StatisticalResultsMap_t;

   class StatisticalResultsCollection {
   public:
      StatisticalResultsCollection();
      StatisticalResultsCollection(TString name);
      ~StatisticalResultsCollection();

      TString name() const;
      void setName(TString name);
      TString treeName() const;
      void setTreeName(TString name);

      void addResult(StatisticalResults &res, Double_t point);
      void addResults(StatisticalResultsCollection &other);
      StatisticalResults &result(Double_t point);
      void persist(TDirectory *file);
      void persist(TString filename);
      void retrieve(TDirectory *file);
      void retrieve(TString filename);
      void reset();
      std::vector<Double_t> points();
      StatisticalResultsMap_t & results();

   private:
      TString m_name;
      TString m_treeName;
      StatisticalResultsMap_t  m_results;
   };
}

#endif

