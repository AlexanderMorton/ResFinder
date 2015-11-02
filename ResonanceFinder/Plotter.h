#ifndef __RF_Plotter_h__
#define __RF_Plotter_h__

///
/// \brief Plotter - a class to visualize a StatisticalResultsCollection
/// \author Valerio Ippolito - Harvard University
/// \date Apr 8, 2015
///
/// Plotter the class which plots p0s and limits given an user input
/// (cross sections, efficiencies) and a StatisticalResultsCollection.
///

#include <TString.h>
#include <map>

namespace RF {
   class StatisticalResultsCollection;

   class Plotter {
   public:
      class SampleSpec {
      public:
         Double_t branching_ratio;
         Double_t cross_section;
      };

      typedef std::map<Double_t, SampleSpec> SampleSpecMap_t;


      typedef enum {
         pdf,
         png,
         eps,
         root,
         C
      } OutputFormat;

   public:
      Plotter(TString name, TString dest);
      virtual ~Plotter();

      TString name() const;
      void setName(TString name);
      TString dest() const;
      void setDest(TString name);
      TString varName() const;
      void setVarName(TString name);
      TString varUnit() const;
      void setVarUnit(TString name);
      OutputFormat outputFormat() const;
      void setOutputFormat(OutputFormat format);

      SampleSpec &getSpec(Double_t point);
      void setSpec(Double_t point, Double_t br, Double_t xsec);

      virtual void process(StatisticalResultsCollection &data);

   protected:
      virtual void plot_p0(StatisticalResultsCollection &data, TString prefix);
      virtual void plot_limit(StatisticalResultsCollection &data, TString prefix);

      TString m_formatString;
      SampleSpecMap_t m_spec;
      TString m_varName;
      TString m_varUnit;

   private:
      TString m_name;
      TString m_dest;
      OutputFormat m_format;
   };
}

#endif
