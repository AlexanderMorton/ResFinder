// $Id$
/*
 * @file 
 * @author Stefan Gadatsch <Stefan.Gadatsch@cern.ch>
 * @author Haifeng Li <Haifeng.Li@cern.ch>
 * @date Sep, 2015
 * @brief 
 */

#ifndef __PullPlotter_h__
#define __PullPlotter_h__



#include <TString.h>
#include <map>
#include "TCanvas.h"
#include "TH2F.h"
#include "TGraphAsymmErrors.h"
#include "TLatex.h"
#include "TLine.h"
#include "TFile.h"
#include "TGaxis.h"
#include "TSystemDirectory.h"
#include "TList.h"
#include "TSystemFile.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TButton.h"

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>


#include "ResonanceFinder/parseString.h"
#include "ResonanceFinder/fileHolder.h"
#include "ResonanceFinder/AtlasLabels.h"

#include "ResonanceFinder/StatisticalResultsCollection.h"

using namespace RF;

//class StatisticalResultsCollection;
class fileHolder;
class PullPlotter {
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
      PullPlotter(TString name, TString dest);
      virtual ~PullPlotter();

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

      void addOneMassPoint(double m);


      // global style options
bool doHorizontal              ; // produce a horizontal plot
bool drawInset                 ; // will cover legend but show the normalisation factors which are a priori unconstrained
bool drawErrorBars             ; // draw bars visualising the total, stat and syst uncertainty
bool drawParamNames            ; // show the nuisance parameter labels
bool drawPostfitImpactBand     ; // && (mode != error); // draw a band with the variation of muhat due to each theta (postfit uncertainty)
bool drawPrefitImpactBand      ; // && (mode != error); // draw a band with the variation of muhat due to each theta (prefit uncertainty)
bool useRelativeImpact         ; // switch to delta muhat / delta muhat tot for the top axis
int useBreakdown               ; // 0 = add, 1 = sub
double scale_poi               ; // zoom the impact axis
double scale_theta             ; // zoom the pull axis
bool removeHbb                 ; // remove Hbb parameters from the plot
bool removeHtt                 ; // remove Htt parameters from the plot
int showTopParameters          ; // -1 to show all parameters
double showHighImpact          ; // sigma_comp / sigma_tot threshold
Color_t color_standardband     ;
Color_t color_standardband_ol  ;
Color_t color_totalerror       ;
Color_t color_staterror        ;
Color_t color_systerror        ;
Color_t color_pulls            ;
Color_t color_prefit           ;
Color_t color_prefit_ol        ;
Color_t color_postfit          ;
Color_t color_postfit_ol       ;
bool rankNuis                  ;// sort the nuisance parameters by impact on POI




TCanvas* c_saved     ;
string saveName_saved;
string labelTxt      ;

bool doInj           ;
bool doLogx          ;
bool doSave          ;
bool forApproval     ;
bool showLabel       ;
double labelPosX     ;
double labelPosY     ;
double textSize      ;
double txtPosX       ;
double txtPosY       ;
double channelPosX   ;
double channelPosY   ;

string lumi_2011     ;
string lumi_2012     ;
string lumi          ;
string energy        ;
string year          ;
string channel_label ;

string overlay       ;
string overlay2      ;
string overlay3      ;
double xmin_leg      ;
double xdiff_leg     ;
double ymax_leg      ;
double ydiff_leg     ;
double markerSize    ;

int minMass          ;
int maxMass          ;

bool overridden      ;
bool overrideMass    ;

bool dozoom          ;
bool do2011          ;
bool do2012          ;

bool dogg_4l_lvlv    ;
bool dogg_4l         ;
bool dogg            ;
bool dollll          ;
bool dollqq          ;
bool dollvv          ;
bool dolvqq          ;
bool dolvlv          ;
bool dott            ;
bool doll            ;
bool dolh            ;
bool dowh            ;
bool dozh            ;
bool dovh            ;
bool dozz            ;
bool doww            ;
bool docb            ;
bool dolog           ;
bool drawBands       ;






 void drawPlot_pulls2(std::string cardName, std::string mass, TCanvas* c1, TPad* pad1, TPad* pad2);
 void ROOT2Ascii(std::string folder);
 void loadFile(const char* fileName, int cols, fileHolder file);
 std::vector<std::string> getLabel(const char* fileName, int nrPars);


 void applyOverlay(string in, string& out, string suf) ; 

 void applyOverlay(string in, string& out) ;

 void computeFlags(string cardName) ;

 void drawPlot(string cardName, int nrCols, fileHolder& numbers) ;

 double* getAry(vector<double> numbers) ;
 TGraph* makeGraph(string title, int n, double* x_ary, double* y_ary) ;

 TGraphAsymmErrors* makeGraphErr(string title, int n, double* x_ary, double* central, double* errlo, double* errhi) ;

 TGraphAsymmErrors* makeGraphErr(string title, int n, double* x_ary, double* central, double* cenlo, double* cenhi, double* errlo, double* errhi) ;

 TLegend* makeLeg() ;

 TLegend* makeLeg2();

 void save(string baseName, string type, TCanvas* c1) ;
 void save();
 void addButton() ;

 void drawInsert() ;

 void drawTopRight(string s);

   protected:
     
 void plot_pull(TString prefix, double xmass);
      
      TString m_formatString;
      SampleSpecMap_t m_spec;
      TString m_varName;
      TString m_varUnit;

   private:
      TString m_name;
      TString m_dest;
      OutputFormat m_format;

      vector<double> m_massPoints ; 

};


#endif
