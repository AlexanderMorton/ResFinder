#include "ResonanceFinder/PullPlotter.h"

#include "ResonanceFinder/StatisticalResultsCollection.h"
#include <TCanvas.h>
#include <TGraphAsymmErrors.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TLatex.h>
#include <TLine.h>
#include <TMath.h>
#include <Math/DistFunc.h>






PullPlotter::PullPlotter(TString name, TString dest) : m_varName("mass"), m_varUnit("GeV"), m_name(name), m_dest(dest)
{
   setOutputFormat(PullPlotter::OutputFormat::png);


   // global style options
   doHorizontal          = false; // produce a horizontal plot
   drawInset             = false; // will cover legend but show the normalisation factors which are a priori unconstrained
   drawErrorBars         = false; // draw bars visualising the total, stat and syst uncertainty
   drawParamNames        = true;  // show the nuisance parameter labels
   drawPostfitImpactBand = true;  // && (mode != error); // draw a band with the variation of muhat due to each theta (postfit uncertainty)
   drawPrefitImpactBand  = true;  // && (mode != error); // draw a band with the variation of muhat due to each theta (prefit uncertainty)
   useRelativeImpact     = false; // switch to delta muhat / delta muhat tot for the top axis
   useBreakdown           = 0;     // 0 = add, 1 = sub
   scale_poi           = 2.0;   // zoom the impact axis
   scale_theta         = 1.0;   // zoom the pull axis
   removeHbb             = false; // remove Hbb parameters from the plot
   removeHtt             = false; // remove Htt parameters from the plot
   showTopParameters      = -1;    // -1 to show all parameters
   showHighImpact      = 0.0;   // sigma_comp / sigma_tot threshold
   color_standardband    = kRed-4;
   color_standardband_ol = kBlue-4;
   color_totalerror   = kBlue-4;
   color_staterror    = kGreen+1;
   color_systerror    = kMagenta-4;
   color_pulls        = kGray+2;
   color_prefit       = kYellow-7;
   color_prefit_ol    = kGreen-8;
   color_postfit      = kBlue-4;
   color_postfit_ol   = kYellow-7;
   rankNuis              = true; // sort the nuisance parameters by impact on POI



labelTxt    = "Internal"  ;

doInj           = 0;
doLogx          = 0;
doSave          = 1;
forApproval     = 0;
showLabel       = 1;
labelPosX     = 0.2;
labelPosY     = 0.87;
textSize      = 0.04;
txtPosX       = 0.6;
txtPosY       = 0.3;
channelPosX   = 0.15;
channelPosY   = 0.87;

lumi_2011     = "4.8";
lumi_2012     = "3.2";
lumi          = "1.0-5.0";
energy        = "8";
year          = "2012";
channel_label = "";

overlay       = "";
overlay2      = "";
overlay3      = "";
xmin_leg      = 0.6;
xdiff_leg     = 0.22;
ymax_leg      = 0.85;
ydiff_leg     = 0.13;
markerSize    = 0.8;

minMass          = 90;
maxMass          = 600;

overridden      = 0;
overrideMass    = 0;

dozoom          = 0;
do2011          = 0;
do2012          = 0;

dogg_4l_lvlv    = 0;
dogg_4l         = 0;
dogg            = 0;
dollll          = 0;
dollqq          = 0;
dollvv          = 0;
dolvqq          = 0;
dolvlv          = 0;
dott            = 0;
doll            = 0;
dolh            = 0;
dowh            = 0;
dozh            = 0;
dovh            = 0;
dozz            = 0;
doww            = 0;
docb            = 0;
dolog           = 0;
drawBands       = 0;


}

PullPlotter::~PullPlotter()
{
}

TString PullPlotter::name() const
{
   return m_name;
}

void PullPlotter::setName(TString name)
{
   m_name = name;
}

TString PullPlotter::dest() const
{
   return m_dest;
}

void PullPlotter::setDest(TString name)
{
   m_dest = name;
}

TString PullPlotter::varName() const
{
   return m_varName;
}

void PullPlotter::setVarName(TString name)
{
   m_varName = name;
}

TString PullPlotter::varUnit() const
{
   return m_varUnit;
}

void PullPlotter::setVarUnit(TString name)
{
   m_varUnit = name;
}

PullPlotter::OutputFormat PullPlotter::outputFormat() const
{
   return m_format;
}

void PullPlotter::setOutputFormat(PullPlotter::OutputFormat format)
{
   m_format = format;

   if (format == PullPlotter::OutputFormat::pdf) {
      m_formatString = ".pdf";
   } else if (format == PullPlotter::OutputFormat::png) {
      m_formatString = ".png";
   } else if (format == PullPlotter::OutputFormat::eps) {
      m_formatString = ".eps";
   } else if (format == PullPlotter::OutputFormat::root) {
      m_formatString = ".root";
   } else if (format == PullPlotter::OutputFormat::C) {
      m_formatString = ".C";
   } else throw std::logic_error("Format not implemented");
}

PullPlotter::SampleSpec& PullPlotter::getSpec(Double_t point)
{
   if (m_spec.find(point) == m_spec.end()) throw std::runtime_error("Requesting specifications for an unknow mass point");

   return m_spec[point];
}

void PullPlotter::setSpec(Double_t point, Double_t br, Double_t xsec)
{
   m_spec[point].branching_ratio = br;
   m_spec[point].cross_section = xsec;
}


void PullPlotter::addOneMassPoint(double m)
{
  m_massPoints.push_back(m);
}


void PullPlotter::process(StatisticalResultsCollection &data)
{
   // determine file path prefix
   const TString prefix = m_dest + "/" + m_name;

   for (int i=0; i < m_massPoints.size() ; i++)  { 
     cout  << " DEBUG mass " << m_massPoints[i] << endl; 
     plot_pull(prefix, m_massPoints[i]);
   }
}


void PullPlotter::plot_pull( TString prefix, double xmass)
{
  //std::string mass = "125";
  std::string mass = Form("%.0f", xmass);
  std::string cardName = "test";
  bool remakeAscii = 1;

  std::string overlayCard="";

  cout << "DEBUG plot_pull :: " << mass << endl;

  cardName  = cardName + "_" + mass ;

  std::vector<std::string> parsed = parseString(cardName, ":");
    std::string cardOpts;
    if (parsed.size() > 1) {
        cardOpts = parsed[1];
    }
    cardName = parsed[0];
    computeFlags(cardName);

    applyOverlay(overlayCard , overlay , "");

    if (remakeAscii) {
        ROOT2Ascii(parsed[0]+"_pulls");
        ROOT2Ascii(parsed[0]+"_breakdown_add");

        if (overlay != "") {
            ROOT2Ascii(overlay+"_pulls");
            ROOT2Ascii(overlay+"_breakdown_add");
        }
    }

    showLabel = 1;

    TCanvas* c1 = new TCanvas("c1","c1",1024,1448);

    TPad *pad1 = new TPad("pad1", "pad1", 0.0  , 0.0  , 1.0 , 1.0  , 0);
    TPad *pad2 = new TPad("pad2", "pad2", 0.63, 0.1, 0.94, 0.22, 0);

    if (drawParamNames) pad1->SetLeftMargin(0.25);
    else pad1->SetLeftMargin(0.05);
    pad1->SetRightMargin(0.05);
    pad1->SetBottomMargin(0.09);
    if (drawErrorBars) pad1->SetTopMargin(0.10);
    else pad1->SetTopMargin(0.09);
    
    pad2->SetLeftMargin(0.325);
    pad2->SetRightMargin(0.01);

    pad1->Draw();
    if (drawInset) pad2->Draw();

    ydiff_leg = 0.15;

    labelPosX = 0.06;
    channelPosX = 0.28;
    channelPosY = 0.19;

    if (dolvlv) {
        markerSize = 0.8;

        minMass = 0;
        maxMass = 500;

        drawPlot_pulls2(cardName, mass, c1, pad1, pad2);

        pad1->cd();

        TLatex t;
        t.SetTextSize(0.03);
        t.SetNDC();
    
        if (cardName.find("_ee_") != std::string::npos)          t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu");
        else if (cardName.find("_em_")     != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu");
        else if (cardName.find("_me_")     != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrow#mu#nue#nu");
        else if (cardName.find("_mm_")     != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrow#mu#nu#mu#nu");
        else if (cardName.find("_0j_")     != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu (0 jets)");
        else if (cardName.find("_1j_")     != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu (1 jet)");
        else if (cardName.find("_2j_")     != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu (2 jets)");
        else if (cardName.find("_BK_")     != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu B-K");
        else if (cardName.find("_LM_")     != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu L-M");
        else if (cardName.find("_ee0j_")   != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu (0 jets)");
        else if (cardName.find("_em0j_")   != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu (0 jets)");
        else if (cardName.find("_mm0j_")   != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrow#mu#nu#mu#nu (0 jets)");
        else if (cardName.find("_me0j_")   != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrow#mu#nue#nu (0 jets)");
        else if (cardName.find("_ee1j_")   != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu (1 jet)");
        else if (cardName.find("_em1j_")   != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu (1 jet)");
        else if (cardName.find("_me1j_")   != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrow#mu#nue#nu (1 jet)");
        else if (cardName.find("_mm1j_")   != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrow#mu#nu#mu#nu (1 jet)");
        else if (cardName.find("_ee01j_")  != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu (0/1 jets)");
        else if (cardName.find("_em01j_")  != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu (0/1 jets)");
        else if (cardName.find("_mm01j_")  != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrow#mu#nu#mu#nu (0/1 jets)");
        else if (cardName.find("_me01j_")  != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrow#mu#nue#nu (0/1jets)");
        else if (cardName.find("_ee2j_")   != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu (2 jets)");
        else if (cardName.find("_em2j_")   != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu (2 jets)");
        else if (cardName.find("_me2j_")   != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrow#mu#nue#nu (2 jets)");
        else if (cardName.find("_mm2j_")   != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrow#mu#nu#mu#nu (2 jets)");
        else if (cardName.find("_em0jBK_") != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu BK (0 jets)");
        else if (cardName.find("_em0jLM_") != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu LM (0 jets)");
        else if (cardName.find("_em1jBK_") != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu BK (1 jet)");
        else if (cardName.find("_em1jLM_") != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu LM (1 jet)");
        else if (cardName.find("_01j_")    != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu (0/1 jets)");
        else if (cardName.find("_SF_")     != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu/#mu#nu#mu#nu");
        else if (cardName.find("_SF0j_")   != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu/#mu#nu#mu#nu (0 jets)");
        else if (cardName.find("_SF1j_")   != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu/#mu#nu#mu#nu (1 jet)");
        else if (cardName.find("_SF01j_")  != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu/#mu#nu#mu#nu (0/1 jets)");
        else if (cardName.find("_SF2j_")   != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu/#mu#nu#mu#nu (2 jets)");
        else if (cardName.find("_OF_")     != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu/#mu#nue#nu");
        else if (cardName.find("_OF0j_")   != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu/#mu#nue#nu (0 jets)");
        else if (cardName.find("_OF1j_")   != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu/#mu#nue#nu (1 jet)");
        else if (cardName.find("_OF01j_")  != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu/#mu#nue#nu (0/1 jets)");
        else if (cardName.find("_OF2j_")   != std::string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu/#mu#nue#nu (2 jets)");
        else if (cardName.find("_cuts_") != std::string::npos) {
            t.DrawLatex(channelPosX + 0.4, channelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu");
            t.DrawLatex(0.67, 0.2, "m_{T} Cut");
        }
        else if (cardName.find("_lpt_") != std::string::npos) {
            t.DrawLatex(channelPosX + 0.4, channelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu");
            t.DrawLatex(0.67, 0.2, "Low pT alone");
        }
        else if (cardName.find("_hpt_") != std::string::npos) {
            t.DrawLatex(channelPosX + 0.4, channelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu");
            t.DrawLatex(0.67, 0.2, "High pT alone");
        } else {
            t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu");
        }
    } else {
        markerSize = 0.8;

        minMass = 0;
        maxMass = 500;

        drawPlot_pulls2(cardName, mass, c1, pad1, pad2);

        pad1->cd();

        TLatex t;
        t.SetTextSize(0.03);
        t.SetNDC();

        t.DrawLatex(channelPosX, channelPosY, cardName.c_str());
        if (overlay != "") t.DrawLatex(channelPosX, channelPosY-0.025, overlay.c_str());
    }

    labelPosY = channelPosY-0.02;
    ATLASLabel(labelPosX,labelPosY,"",1);

    TLatex p;
    p.SetNDC();
    p.SetTextFont(42);
    p.DrawLatex(labelPosX,labelPosY-0.04,labelTxt.c_str());

    TLatex t3;
    t3.SetNDC();
    t3.SetTextSize(0.03);
    std::stringstream lumiLatex;
    lumiLatex << "#sqrt{s} = " << energy << " TeV: #lower[-0.2]{#scale[0.6]{#int}}Ldt = " << lumi << " fb^{-1}";
    // t3.DrawLatex(channelPosX,channelPosY-0.035,lumiLatex.str().c_str());

    TLatex t2;
    t2.SetTextSize(0.03);
    t2.SetNDC();
    t2.DrawLatex(channelPosX, channelPosY-0.07, ("m_{H}="+mass+" GeV").c_str());

    std::stringstream saveName;
    saveName << cardName << "_pulls_" << mass;
    save(saveName.str(), "eps", c1);
    save(saveName.str(), "pdf", c1);
    save(saveName.str(), "C", c1);
  
}

// ____________________________________________________________________________|__________
// The actual plotting goes on here
void PullPlotter::drawPlot_pulls2(string cardName, string mass, TCanvas* c1, TPad* pad1, TPad* pad2) {
    cout << "INFO::Drawing pulls: " << cardName << " for mH = " << mass << " GeV";

    // load and initialize ascii files
    ifstream testFile(("ascii/"+cardName+"_pulls.txt").c_str());
    if (testFile.fail()) {
        cout << "ERROR::file " << ("ascii/"+cardName+"_pulls.txt").c_str() << "does not exist.";
        exit(1);
    }
    fileHolder pulls;
    drawPlot("ascii/"+cardName+"_pulls.txt", 8, pulls);

    // overlay
    if (overlay != "") {
        ifstream testFile_ol(("ascii/"+overlay+"_pulls.txt").c_str());
        if (testFile_ol.fail()) {
            cout << "ERROR::file " << ("ascii/"+overlay+"_pulls.txt").c_str() << "does not exist.";
            exit(1);
        }
    }
    fileHolder pulls_ol;
    if (overlay != "") drawPlot("ascii/"+overlay+"_pulls.txt", 8, pulls_ol);

    // load and initialize the normalization factor ascii file
    ifstream testFile2(("ascii/"+cardName+"_pulls_nf.txt").c_str());
    if (testFile2.fail()) {
        cout << "ERROR::file " << ("ascii/"+cardName+"_pulls_nf.txt").c_str() << "does not exist.";
        exit(1);
    }
    fileHolder nfs;
    drawPlot("ascii/"+cardName+"_pulls_nf.txt", 8, nfs);
    
    // overlay
    if (overlay != "") {
        ifstream testFile2_ol(("ascii/"+overlay+"_pulls_nf.txt").c_str());
        if (testFile2_ol.fail()) {
            cout << "ERROR::file " << ("ascii/"+overlay+"_pulls_nf.txt").c_str() << "does not exist.";
            exit(1);
        }
    }
    fileHolder nfs_ol;
    if (overlay != "") drawPlot("ascii/"+overlay+"_pulls_nf.txt", 8, nfs_ol);

    // load and initialize the category uncertainties
    ifstream testFile3(("ascii/"+cardName+"_breakdown_add.txt").c_str());
    if (testFile3.fail()) {
        cout << "ERROR::file " << ("ascii/"+cardName+"_breakdown_add.txt").c_str() << "does not exist.";
        exit(1);
    }
    fileHolder cats;
    drawPlot("ascii/"+cardName+"_breakdown_add.txt", 3, cats);
    
    // overlay
    if (overlay != "") {
        ifstream testFile3_ol(("ascii/"+overlay+"_breakdown_add.txt").c_str());
        if (testFile3_ol.fail()) {
            cout << "ERROR::file " << ("ascii/"+overlay+"_breakdown_add.txt").c_str() << "does not exist.";
            exit(1);
        }
    }
    fileHolder cats_ol;
    if (overlay != "") drawPlot("ascii/"+overlay+"_breakdown_add.txt", 3, cats_ol);

    // get the values from the ascii files
    int nrNuis = pulls.massPoints.size();
    int nrNFs = nfs.massPoints.size();
    int nrCats = cats.massPoints.size();

    //int nrNuis_ol = 30;
    int nrNuis_ol = pulls_ol.massPoints.size();
    int nrNFs_ol = nfs_ol.massPoints.size();
    int nrCats_ol = cats_ol.massPoints.size();
    
    vector<double> points_nuis = pulls.massPoints;
    vector<double> points_nf = nfs.massPoints;
    vector<double> points_cats = cats.massPoints;

    vector<double> points_nuis_ol = pulls_ol.massPoints;
    vector<double> points_nf_ol = nfs_ol.massPoints;
    vector<double> points_cats_ol = cats_ol.massPoints;

    for (int i = 0; i < nrNuis; i++) points_nuis[i]   = i + 0.5;
    for (int i = 0; i < nrNFs; i++) points_nf[i] = i + 0.5;

    for (int i = 0; i < nrNuis_ol; i++) points_nuis_ol[i]   = i + 0.25;
    for (int i = 0; i < nrNFs_ol; i++) points_nf_ol[i] = i + 0.25;

    if (overlay != ""){
        for (int i = 0; i < nrNuis; i++) points_nuis[i]   = i + 0.75;
        // TODO: nfs not needed at the moment
    }

    vector<double> val          = pulls.getCol(0);
    vector<double> up           = pulls.getCol(1);
    vector<double> down         = pulls.getCol(2);
    vector<double> poi_hat      = pulls.getCol(3);
    vector<double> poi_up       = pulls.getCol(4);
    vector<double> poi_down     = pulls.getCol(5);
    vector<double> poi_nom_up   = pulls.getCol(6);
    vector<double> poi_nom_down = pulls.getCol(7);

    vector<double> val_ol          = pulls_ol.getCol(0);
    vector<double> up_ol           = pulls_ol.getCol(1);
    vector<double> down_ol         = pulls_ol.getCol(2);
    vector<double> poi_hat_ol      = pulls_ol.getCol(3);
    vector<double> poi_up_ol       = pulls_ol.getCol(4);
    vector<double> poi_down_ol     = pulls_ol.getCol(5);
    vector<double> poi_nom_up_ol   = pulls_ol.getCol(6);
    vector<double> poi_nom_down_ol = pulls_ol.getCol(7);

    vector<double> nf_val          = nfs.getCol(0);
    vector<double> nf_up           = nfs.getCol(1);
    vector<double> nf_down         = nfs.getCol(2);
    vector<double> nf_poi_hat      = nfs.getCol(3);
    vector<double> nf_poi_up       = nfs.getCol(4);
    vector<double> nf_poi_down     = nfs.getCol(5);
    vector<double> nf_poi_nom_up   = nfs.getCol(6);
    vector<double> nf_poi_nom_down = nfs.getCol(7);

    vector<double> nf_val_ol          = nfs_ol.getCol(0);
    vector<double> nf_up_ol           = nfs_ol.getCol(1);
    vector<double> nf_down_ol         = nfs_ol.getCol(2);
    vector<double> nf_poi_hat_ol      = nfs_ol.getCol(3);
    vector<double> nf_poi_up_ol       = nfs_ol.getCol(4);
    vector<double> nf_poi_down_ol     = nfs_ol.getCol(5);
    vector<double> nf_poi_nom_up_ol   = nfs_ol.getCol(6);
    vector<double> nf_poi_nom_down_ol = nfs_ol.getCol(7);

    vector<double> cats_val  = cats.getCol(0);
    vector<double> cats_up   = cats.getCol(1);
    vector<double> cats_down = cats.getCol(2);

    vector<double> cats_val_ol  = cats_ol.getCol(0);
    vector<double> cats_up_ol   = cats_ol.getCol(1);
    vector<double> cats_down_ol = cats_ol.getCol(2);

    // set correct values for the poi
    for (int i = 0; i < nrNuis; i++) {
        val[i] *= scale_theta;

        poi_up[i]   = poi_up[i] - poi_hat[i];
        poi_down[i] = poi_down[i] - poi_hat[i];

        poi_nom_up[i]   = poi_nom_up[i] - poi_hat[i];
        poi_nom_down[i] = poi_nom_down[i] - poi_hat[i];
        
        if (poi_up[i] < 0) swap(poi_up[i], poi_down[i]);
        if (poi_nom_up[i] < 0) swap(poi_nom_up[i], poi_nom_down[i]);
        
        poi_up[i]   = fabs(poi_up[i]);
        poi_down[i] = fabs(poi_down[i]);

        poi_nom_up[i]   = fabs(poi_nom_up[i]);
        poi_nom_down[i] = fabs(poi_nom_down[i]);
        
        poi_hat[i] = 0;
    }

    for (int i = 0; i < nrNuis_ol; i++) {
        val_ol[i] *= scale_theta;

        poi_up_ol[i]   = poi_up_ol[i] - poi_hat_ol[i];
        poi_down_ol[i] = poi_down_ol[i] - poi_hat_ol[i];

        poi_nom_up_ol[i]   = poi_nom_up_ol[i] - poi_hat_ol[i];
        poi_nom_down_ol[i] = poi_nom_down_ol[i] - poi_hat_ol[i];
        
        if (poi_up_ol[i] < 0) swap(poi_up_ol[i], poi_down_ol[i]);
        if (poi_nom_up_ol[i] < 0) swap(poi_nom_up_ol[i], poi_nom_down_ol[i]);
        
        poi_up_ol[i]   = fabs(poi_up_ol[i]);
        poi_down_ol[i] = fabs(poi_down_ol[i]);

        poi_nom_up_ol[i]   = fabs(poi_nom_up_ol[i]);
        poi_nom_down_ol[i] = fabs(poi_nom_down_ol[i]);
        
        poi_hat_ol[i] = 0;
    }

    // find maximal error due to a single nuisance parameter
    double max_poi = 0.;
    for (int i = 0; i < nrNuis; ++i) {
        if (poi_up[i] > max_poi) max_poi = poi_up[i];
        if (poi_down[i] > max_poi) max_poi = poi_down[i];
    }
    // TODO: for overlay as well? maybe get rid of this...

    // get labels
    vector<std::string> labels;
    Int_t nlines = 0;
    ifstream idFile(("ascii/"+cardName+"_pulls_id.txt").c_str());
    while (1) {
        if (!idFile.good() || nlines > nrNuis-1) break;
        string label;
        idFile >> label;
        labels.push_back(label);
        cout << "added: " << label << endl;
        nlines++;
    }

    vector<string> labels_ol;
    if (overlay != "") {
        Int_t nlines_ol = 0;
        ifstream idFile_ol(("ascii/"+overlay+"_pulls_id.txt").c_str());
        while (1) {
            if (!idFile_ol.good() || nlines_ol > nrNuis_ol-1) break;
            string label;
            idFile_ol >> label;
            labels_ol.push_back(label);
            nlines_ol++;
        }
    }

    vector<string> nf_labels;
    Int_t nf_nlines = 0;
    ifstream idFile2(("ascii/"+cardName+"_pulls_nf_id.txt").c_str());
    while (1) {
        if (!idFile2.good() || nf_nlines > nrNFs-1) break;
        string nf_label;
        idFile2 >> nf_label;
        nf_labels.push_back(nf_label);
        nf_nlines++;
    }

    vector<string> nf_labels_ol;
    if (overlay != "") {
        Int_t nf_nlines_ol = 0;
        ifstream idFile2_ol(("ascii/"+overlay+"_pulls_nf_id.txt").c_str());
        while (1) {
            if (!idFile2_ol.good() || nf_nlines_ol > nrNFs_ol-1) break;
            string nf_label;
            idFile2_ol >> nf_label;
            nf_labels_ol.push_back(nf_label);
            nf_nlines_ol++;
        }
    }

    vector<string> cats_labels;
    Int_t cats_nlines = 0;
    ifstream idFile3(("ascii/"+cardName+"_breakdown_add_id.txt").c_str());
    while (1) {
        if (!idFile3.good() || cats_nlines > nrCats-1) break;
        string cat_label;
        idFile3 >> cat_label;
        cats_labels.push_back(cat_label);
        cats_nlines++;
    }

    vector<string> cats_labels_ol;
    if (overlay != "") {
        Int_t cats_nlines_ol = 0;
        ifstream idFile3_ol(("ascii/"+overlay+"_breakdown_add_id.txt").c_str());
        while (1) {
            if (!idFile3_ol.good() || cats_nlines_ol > nrCats_ol-1) break;
            string cat_label;
            idFile3_ol >> cat_label;
            cats_labels_ol.push_back(cat_label);
            cats_nlines_ol++;
        }
    }

    // map of category uncertainties
    map<string, vector<double> > cat_uncerts;
    for (int i = 0; i < nrCats; i++) {
        string index = cats_labels[i];
        cat_uncerts[index].push_back(cats_val[i]);
        cat_uncerts[index].push_back(cats_up[i]);
        cat_uncerts[index].push_back(cats_down[i]);
    }

    map<string, vector<double> > cat_uncerts_ol;
    if (overlay != "") {
        for (int i = 0; i < nrCats_ol; i++) {
            string index = cats_labels_ol[i];
            cat_uncerts_ol[index].push_back(cats_val_ol[i]);
            cat_uncerts_ol[index].push_back(cats_up_ol[i]);
            cat_uncerts_ol[index].push_back(cats_down_ol[i]);
        }
    }

    double sigma_tot_hi    = cat_uncerts["total"][1];
    double sigma_tot_lo    = cat_uncerts["total"][2];
    double sigma_tot_ol_hi;
    double sigma_tot_ol_lo;
    if (overlay != "") sigma_tot_ol_hi = cat_uncerts_ol["total"][1];
    if (overlay != "") sigma_tot_ol_lo = cat_uncerts_ol["total"][2];

    // hardcoded for now
    double sigma_stat_hi = 0.;
    double sigma_stat_lo = 0.;
    double sigma_syst_hi = 0.;
    double sigma_syst_lo = 0.;
    // TODO: can probably drop this

    // dump everything in maps
    map<string, vector<double> > nuis_map;
    for (int i = 0; i < nrNuis; i++) {
        string index = labels[i];
        nuis_map[index].push_back(val[i]);
        nuis_map[index].push_back(up[i]);
        nuis_map[index].push_back(down[i]);
        nuis_map[index].push_back(poi_hat[i]);
        nuis_map[index].push_back(poi_up[i]);
        nuis_map[index].push_back(poi_down[i]);
        nuis_map[index].push_back(poi_nom_up[i]);
        nuis_map[index].push_back(poi_nom_down[i]);
    }

    map<string, vector<double> > nuis_map_ol;
    if (overlay != "") {
        for (int i = 0; i < nrNuis_ol; i++) {
            string index = labels_ol[i];
            nuis_map_ol[index].push_back(val_ol[i]);
            nuis_map_ol[index].push_back(up_ol[i]);
            nuis_map_ol[index].push_back(down_ol[i]);
            nuis_map_ol[index].push_back(poi_hat_ol[i]);
            nuis_map_ol[index].push_back(poi_up_ol[i]);
            nuis_map_ol[index].push_back(poi_down_ol[i]);
            nuis_map_ol[index].push_back(poi_nom_up_ol[i]);
            nuis_map_ol[index].push_back(poi_nom_down_ol[i]);
        }
    }

    // check that we have in both maps the same keys
    if (overlay != "") {

        std::vector<string> tmpstring;

        int counter = nrNuis;
        int counter_ol = nrNuis_ol;

        for (int i = 0; i < nrNuis; i++) {
            bool found = 0;
            for (int ii = 0; ii < nrNuis_ol; ii++) {
                if (labels[i] == labels_ol[ii]) found = 1;
            }
            if (!found) {
                nuis_map_ol[labels[i]].push_back(-999); val_ol.push_back(-999);
                nuis_map_ol[labels[i]].push_back(0); up_ol.push_back(0);
                nuis_map_ol[labels[i]].push_back(0); down_ol.push_back(0);
                nuis_map_ol[labels[i]].push_back(0); poi_hat_ol.push_back(0);
                nuis_map_ol[labels[i]].push_back(0); poi_up_ol.push_back(0);
                nuis_map_ol[labels[i]].push_back(0); poi_down_ol.push_back(0);
                nuis_map_ol[labels[i]].push_back(0); poi_nom_up_ol.push_back(0);
                nuis_map_ol[labels[i]].push_back(0); poi_nom_down_ol.push_back(0);
                labels_ol.push_back(labels[i]);
                points_nuis_ol.push_back(counter_ol + 0.25); counter_ol++;
            }
        }

        for (int i = 0; i < nrNuis_ol; i++) {
            bool found = 0;
            for (int ii = 0; ii < nrNuis; ii++) {
                if (labels_ol[i] == labels[ii]) found = 1;
            }
            if (!found) {
                nuis_map[labels_ol[i]].push_back(-999); val.push_back(-999);
                nuis_map[labels_ol[i]].push_back(0); up.push_back(0);
                nuis_map[labels_ol[i]].push_back(0); down.push_back(0);
                nuis_map[labels_ol[i]].push_back(0); poi_hat.push_back(0);
                nuis_map[labels_ol[i]].push_back(0); poi_up.push_back(0);
                nuis_map[labels_ol[i]].push_back(0); poi_down.push_back(0);
                nuis_map[labels_ol[i]].push_back(0); poi_nom_up.push_back(0);
                nuis_map[labels_ol[i]].push_back(0); poi_nom_down.push_back(0);
                labels.push_back(labels_ol[i]);
                points_nuis.push_back(counter + 0.75); counter++;
            }
        }
    }

    // Getting the vectors back
    nrNuis    = labels.size();
    if (overlay != "") nrNuis_ol = labels_ol.size();

    for (int i = 0; i < nrNuis-1; i++) {
        for (int j = 0; j < nrNuis-1-i; j++) {
            if (strcmp(labels[i].c_str(),labels[i+1].c_str())) {
                swap(labels[j], labels[j+1]);
                if (overlay != "") labels_ol[j+1] = labels[j+1];
                if (overlay != "") labels_ol[j] = labels[j];
            }
        }
    }

    for (int i = 0; i < nrNuis; i++) {
        val[i] = nuis_map[labels[i]][0];
        up[i] = nuis_map[labels[i]][1];
        down[i] = nuis_map[labels[i]][2];
        poi_hat[i] = nuis_map[labels[i]][3];
        poi_up[i] = nuis_map[labels[i]][4];
        poi_down[i] = nuis_map[labels[i]][5];
        poi_nom_up[i] = nuis_map[labels[i]][6];
        poi_nom_down[i] = nuis_map[labels[i]][7];
    }

    for (int i = 0; i < nrNuis_ol; i++) {
        val_ol[i] = nuis_map_ol[labels_ol[i]][0];
        up_ol[i] = nuis_map_ol[labels_ol[i]][1];
        down_ol[i] = nuis_map_ol[labels_ol[i]][2];
        poi_hat_ol[i] = nuis_map_ol[labels_ol[i]][3];
        poi_up_ol[i] = nuis_map_ol[labels_ol[i]][4];
        poi_down_ol[i] = nuis_map_ol[labels_ol[i]][5];
        poi_nom_up_ol[i] = nuis_map_ol[labels_ol[i]][6];
        poi_nom_down_ol[i] = nuis_map_ol[labels_ol[i]][7];
    }

    if (overlay != "") {
        for (int i = 0; i < nrNuis; i++) {
            cout << "Matches: " << labels[i] << " " << labels_ol[i] << endl;
        }
    }

    // sort poi values by variation size
    if (rankNuis) {
        for (int i = 0; i < nrNuis-1; i++) {
            for (int j = 0; j < nrNuis-1-i; j++) {
                if (poi_up[j]+poi_down[j] > poi_up[j+1]+poi_down[j+1]) {
                    // swap postfit poi
                    swap(poi_up[j], poi_up[j+1]);
                    swap(poi_down[j], poi_down[j+1]);
                    if (overlay != "") {
                        swap(poi_up_ol[j], poi_up_ol[j+1]);
                        swap(poi_down_ol[j], poi_down_ol[j+1]);
                    }

                    // swap prefit poi
                    swap(poi_nom_up[j], poi_nom_up[j+1]);
                    swap(poi_nom_down[j], poi_nom_down[j+1]);
                    if (overlay != "") {
                        swap(poi_nom_up_ol[j], poi_nom_up_ol[j+1]);
                        swap(poi_nom_down_ol[j], poi_nom_down_ol[j+1]);
                    }

                    // swap pulls
                    swap(up[j], up[j+1]);
                    swap(down[j], down[j+1]);
                    swap(val[j], val[j+1]);
                    if (overlay != "") {
                        swap(up_ol[j], up_ol[j+1]);
                        swap(down_ol[j], down_ol[j+1]);
                        swap(val_ol[j], val_ol[j+1]);
                    }

                    // swap names
                    swap(labels[j], labels[j+1]);
                    if (overlay != "") {
                        swap(labels_ol[j], labels_ol[j+1]);
                    }
                }
            }
        }
    }

    // make the 1 sigma boxes
    vector<double> boxup;
    vector<double> boxdown;
    vector<double> cenup;
    vector<double> cendown;

    vector<double> boxup_ol;
    vector<double> boxdown_ol;
    vector<double> cenup_ol;
    vector<double> cendown_ol;

    for (int i = 0; i < nrNuis; i++) {
        boxup.push_back(1.*scale_theta);
        boxdown.push_back(1.*scale_theta);
        double height = 0.5;
        if (overlay != "") height = 0.25;
        cenup.push_back(height);
        cendown.push_back(height);
    }

    for (int i = 0; i < nrNuis_ol; i++) {
        boxup_ol.push_back(1.*scale_theta);
        boxdown_ol.push_back(1.*scale_theta);
        cenup_ol.push_back(0.25);
        cendown_ol.push_back(0.25);
    }

    // make the 1 sigma boxes
    double* statboxup   = new double[nrNuis];
    double* statboxdown = new double[nrNuis];
    double* systboxup   = new double[nrNuis];
    double* systboxdown = new double[nrNuis];

    for (int i = 0; i < nrNuis; i++) {
        statboxup[i]   = sigma_stat_hi * scale_poi / max_poi;
        statboxdown[i] = sigma_stat_lo * scale_poi / max_poi;

        systboxup[i]   = sigma_syst_hi * scale_poi / max_poi;
        systboxdown[i] = sigma_syst_lo * scale_poi / max_poi;
    }
    // TODO: same for overlay, not really needed though

    // find boundaries for NF box
    double max = 1.;
    double min = 1.;
    for (int i = 0; i < nrNFs; ++i) {
        if (nf_val[i] - nf_down[i] < min) min = nf_val[i] - nf_down[i];
        if (nf_val[i] + nf_up[i] > max) max = nf_val[i] + nf_up[i];
    }
    // TODO: same for overlay, not really needed though

    // make the final arrays for plotting, in particular remove parameters
    int nrNuis2remove = 0;
    for (int i = 0; i < nrNuis; i++) {
        cout << "DEBUG::Checking " << labels[i] << " " <<  fabs(poi_down[i]-poi_hat[i]) << " " << fabs(poi_up[i]-poi_hat[i]) << endl;

        if ((fabs(poi_down[i]) + fabs(poi_up[i])) / (sigma_tot_lo + sigma_tot_hi) < showHighImpact) {
            cout << "WARNING::Removing " << labels[i] << ". Below threshold." << endl;
            nrNuis2remove++;
        }        
    }

    if (showTopParameters != -1) nrNuis2remove = nrNuis - showTopParameters;

    labels.erase(labels.begin(), labels.begin() + nrNuis2remove);
    points_nuis.erase(points_nuis.end() - nrNuis2remove, points_nuis.end());

    if (overlay != "") {
        labels_ol.erase(labels_ol.begin(), labels_ol.begin() + nrNuis2remove);
        points_nuis_ol.erase(points_nuis_ol.end() - nrNuis2remove, points_nuis_ol.end());
    }

    val.erase(val.begin(), val.begin() + nrNuis2remove);
    down.erase(down.begin(), down.begin() + nrNuis2remove);
    up.erase(up.begin(), up.begin() + nrNuis2remove);

    if (overlay != "") {
        val_ol.erase(val_ol.begin(), val_ol.begin() + nrNuis2remove);
        down_ol.erase(down_ol.begin(), down_ol.begin() + nrNuis2remove);
        up_ol.erase(up_ol.begin(), up_ol.begin() + nrNuis2remove);
    }

    poi_hat.erase(poi_hat.begin(), poi_hat.begin() + nrNuis2remove);
    poi_down.erase(poi_down.begin(), poi_down.begin() + nrNuis2remove);
    poi_up.erase(poi_up.begin(), poi_up.begin() + nrNuis2remove);

    if (overlay != "") {
        poi_hat_ol.erase(poi_hat_ol.begin(), poi_hat_ol.begin() + nrNuis2remove);
        poi_down_ol.erase(poi_down_ol.begin(), poi_down_ol.begin() + nrNuis2remove);
        poi_up_ol.erase(poi_up_ol.begin(), poi_up_ol.begin() + nrNuis2remove);
    }

    poi_nom_down.erase(poi_nom_down.begin(), poi_nom_down.begin() + nrNuis2remove);
    poi_nom_up.erase(poi_nom_up.begin(), poi_nom_up.begin() + nrNuis2remove);

    if (overlay != "") {
        poi_nom_down_ol.erase(poi_nom_down_ol.begin(), poi_nom_down_ol.begin() + nrNuis2remove);
        poi_nom_up_ol.erase(poi_nom_up_ol.begin(), poi_nom_up_ol.begin() + nrNuis2remove);        
    }

    boxdown.erase(boxdown.begin(), boxdown.begin() + nrNuis2remove);
    boxup.erase(boxup.begin(), boxup.begin() + nrNuis2remove);
    cendown.erase(cendown.begin(), cendown.begin() + nrNuis2remove);
    cenup.erase(cenup.begin(), cenup.begin() + nrNuis2remove);

    if (overlay != "") {
        boxdown_ol.erase(boxdown_ol.begin(), boxdown_ol.begin() + nrNuis2remove);
        boxup_ol.erase(boxup_ol.begin(), boxup_ol.begin() + nrNuis2remove);
        cendown_ol.erase(cendown_ol.begin(), cendown_ol.begin() + nrNuis2remove);
        cenup_ol.erase(cenup_ol.begin(), cenup_ol.begin() + nrNuis2remove);        
    }

    nrNuis -= nrNuis2remove;
    nrNuis_ol -= nrNuis2remove;
    cout << "INFO::" << nrNuis << " " << nrNuis_ol << " nuisance paramters remaining." << endl;

    int offset = ceil(2 * nrNuis / 10); // used for space to plot the labels and legend

    for (int i = 0; i < nrNuis; i++) {
        poi_up[i] = fabs(poi_up[i]) * scale_poi / max_poi;
        poi_down[i] = fabs(poi_down[i]) * scale_poi / max_poi;

        if (overlay != "") {
            poi_up_ol[i] = fabs(poi_up_ol[i]) * scale_poi / max_poi;
            poi_down_ol[i] = fabs(poi_down_ol[i]) * scale_poi / max_poi;
        }

        poi_nom_up[i] = fabs(poi_nom_up[i]) * scale_poi / max_poi;
        poi_nom_down[i] = fabs(poi_nom_down[i]) * scale_poi / max_poi;

        if (overlay != "") {
            poi_nom_up_ol[i] = fabs(poi_nom_up_ol[i]) * scale_poi / max_poi;
            poi_nom_down_ol[i] = fabs(poi_nom_down_ol[i]) * scale_poi / max_poi;
        }

        if (useRelativeImpact) {
            poi_up[i] /= sigma_tot_hi;
            poi_down[i] /= sigma_tot_lo;

            if (overlay != "") {
                poi_up_ol[i] /= sigma_tot_ol_hi;
                poi_down_ol[i] /= sigma_tot_ol_lo;
            }

            poi_nom_up[i] /= sigma_tot_hi;
            poi_nom_down[i] /= sigma_tot_lo;

            if (overlay != "") {
                poi_nom_up_ol[i] /= sigma_tot_ol_hi;
                poi_nom_down_ol[i] /= sigma_tot_ol_lo;
            }
        }

        up[i] = fabs(up[i]) * scale_theta;
        down[i] = fabs(down[i]) * scale_theta;

        if (overlay != "") {
            up_ol[i] = fabs(up_ol[i]) * scale_theta;
            down_ol[i] = fabs(down_ol[i]) * scale_theta;
        }
    }

    // change to the right pad
    pad1->cd();

    // make plot of pulls for nuisance parameters
    markerSize = 1;
    TGraphAsymmErrors* gr = makeGraphErr("", nrNuis, getAry(val), getAry(points_nuis), getAry(down), getAry(up), NULL, NULL);
    gr->SetLineColor(kBlack);
    gr->SetMarkerColor(kBlack);
    gr->SetMarkerStyle(20);
    gr->SetLineStyle(1);
    gr->SetLineWidth(2);
    gr->SetMarkerSize(markerSize);
    gr->GetXaxis()->SetTitleOffset(1.2);

    TGraphAsymmErrors* gr_ol;
    if (overlay != "") {
        gr_ol = makeGraphErr("", nrNuis_ol, getAry(val_ol), getAry(points_nuis_ol), getAry(down_ol), getAry(up_ol), NULL, NULL);
        gr_ol->SetLineColor(kBlack);
        gr_ol->SetMarkerColor(kBlack);
        gr_ol->SetMarkerStyle(24);
        gr_ol->SetLineStyle(1);
        gr_ol->SetLineWidth(2);
        gr_ol->SetMarkerSize(markerSize);
        gr_ol->GetXaxis()->SetTitleOffset(1.2);
    }

    // make plot of 1 sigma boxes
    TGraphAsymmErrors* gr1s = makeGraphErr("", nrNuis, getAry(val), getAry(points_nuis), getAry(boxdown), getAry(boxup), NULL, NULL);
    gr1s->SetLineColor(color_standardband);
    gr1s->SetMarkerColor(color_standardband);
    gr1s->SetLineStyle(1);
    gr1s->SetLineWidth(3);
    gr1s->SetMarkerSize(markerSize*1.25);
    gr1s->GetXaxis()->SetTitleOffset(1.2);


    TGraphAsymmErrors* gr1s_ol;
    if (overlay != "") {
        gr1s_ol = makeGraphErr("", nrNuis_ol, getAry(val_ol), getAry(points_nuis_ol), getAry(boxdown_ol), getAry(boxup_ol), NULL, NULL);
        gr1s_ol->SetLineColor(color_standardband_ol);
        gr1s_ol->SetMarkerColor(color_standardband_ol);
        gr1s_ol->SetLineStyle(1);
        gr1s_ol->SetLineWidth(3);
        gr1s_ol->SetMarkerSize(markerSize*1.25);
        gr1s_ol->GetXaxis()->SetTitleOffset(1.2);
    }

    // make plot of normalization parameters
    TGraphAsymmErrors* gr_nf = makeGraphErr("", nrNFs, getAry(nf_val), getAry(points_nf), getAry(nf_down), getAry(nf_up), NULL, NULL);
    gr_nf->SetLineColor(1);
    gr_nf->SetMarkerColor(1);
    gr_nf->SetMarkerStyle(20);
    gr_nf->SetLineStyle(1);
    gr_nf->SetLineWidth(1);
    gr_nf->SetMarkerSize(markerSize);
    gr_nf->GetXaxis()->SetTitleOffset(1.2);

    if (overlay != "") {
        // TODO: not needed at the moment
    }

    // make plot for the POI change for postfit uncertainties
    TGraphAsymmErrors* gr_poi = makeGraphErr("", nrNuis, getAry(poi_hat), getAry(points_nuis), getAry(poi_down), getAry(poi_up), getAry(cenup), getAry(cendown));
    gr_poi->SetLineColor(color_postfit);
    gr_poi->SetFillColor(color_postfit);
    gr_poi->SetFillStyle(3354);
    gr_poi->SetLineWidth(0);
    gr_poi->SetMarkerSize(0);
    
    TGraphAsymmErrors* gr_poi_ol;
    if (overlay != "") {
        gr_poi_ol = makeGraphErr("", nrNuis_ol, getAry(poi_hat_ol), getAry(points_nuis_ol), getAry(poi_down_ol), getAry(poi_up_ol), getAry(cenup_ol), getAry(cendown_ol));
        gr_poi_ol->SetLineColor(color_postfit_ol);
        gr_poi_ol->SetFillColor(color_postfit_ol);
        gr_poi_ol->SetFillStyle(3345);
        gr_poi_ol->SetLineWidth(0);
        gr_poi_ol->SetMarkerSize(0);
    }

    // make plot for the POI change for prefit uncertainties
    TGraphAsymmErrors* gr_poi_nom = makeGraphErr("", nrNuis, getAry(poi_hat), getAry(points_nuis), getAry(poi_nom_down), getAry(poi_nom_up), getAry(cenup), getAry(cendown));
    gr_poi_nom->SetLineColor(color_prefit);
    gr_poi_nom->SetFillColor(color_prefit);
    gr_poi_nom->SetLineWidth(1);
    gr_poi_nom->SetMarkerSize(0);

    TGraphAsymmErrors* gr_poi_nom_ol;
    if (overlay != "") {
        gr_poi_nom_ol = makeGraphErr("", nrNuis_ol, getAry(poi_hat_ol), getAry(points_nuis_ol), getAry(poi_nom_down_ol), getAry(poi_nom_up_ol), getAry(cenup_ol), getAry(cendown_ol));
        gr_poi_nom_ol->SetLineColor(color_prefit_ol);
        gr_poi_nom_ol->SetFillColor(color_prefit_ol);
        gr_poi_nom_ol->SetLineWidth(1);
        gr_poi_nom_ol->SetMarkerSize(0);
    }


    double border_lo = -sigma_tot_lo / max_poi;
    double border_hi = sigma_tot_hi / max_poi;

    // different shades for better readability
    int nrShades = ceil((nrNuis+1)/2);
    std::vector<double> shadeCenter;
    std::vector<double>  shadePoints;
    std::vector<double>  shadeWidth;
    std::vector<double>  shadeHeight;
    for (int ishade = 0; ishade < nrShades; ishade++) {
        shadeCenter.push_back(0.0);
        shadePoints.push_back(2.0*ishade+0.5);
        shadeWidth.push_back(10.); // TODO: should not be hardcoded
        shadeHeight.push_back(0.5);
    }

    TGraphAsymmErrors* gr_shades = makeGraphErr("", nrShades, getAry(shadeCenter), getAry(shadePoints), getAry(shadeWidth), getAry(shadeWidth), getAry(shadeHeight), getAry(shadeHeight));
    gr_shades->SetLineColor(18);
    gr_shades->SetFillColor(18);
    gr_shades->SetFillStyle(3001);
    gr_shades->SetLineWidth(1);
    gr_shades->SetMarkerSize(0);

    // histogram to get the nuisance parameter labels correct
    TH2F *h = new TH2F("h", "", 1, border_lo, border_hi, nrNuis+offset+1, -offset, nrNuis+1);
    for (int i = offset; i < nrNuis+offset; i++) h->GetYaxis()->SetBinLabel(i+1, drawParamNames?labels[i-offset].c_str():"");
    h->LabelsOption("h");
    double labelSize = 1./nrNuis;
    h->SetLabelSize(labelSize>0.02?0.02:labelSize,"Y");
    h->GetXaxis()->SetLabelColor(kWhite);
    h->GetXaxis()->SetAxisColor(kWhite);
    h->GetYaxis()->SetLabelColor(kBlack);
    h->GetYaxis()->SetAxisColor(kBlack);
    h->GetYaxis()->SetTickLength(0.);
    h->SetStats(0);
    // h->LabelsDeflate();
    h->Draw("h");
    // TODO: order should be the same for overlay, so just do it once

    // histogram to get the normalization parameters labels correct
    TH2F *h2 = new TH2F("h2", "", 1, min-0.05, max+0.05, nrNFs, 0, nrNFs);
    for (int i = 0; i < nrNFs; i++) h2->GetYaxis()->SetBinLabel(i+1, drawParamNames?nf_labels[i].c_str():"");
    h2->SetStats(0);
    h2->SetLabelSize(0.1, "X");
    h2->SetLabelSize(0.1, "Y");
    // TODO: not needed at the moment

    // axis for the POI correlation
    TGaxis *axis_poi = new TGaxis(border_lo, nrNuis+1, border_hi, nrNuis+1, (-sigma_tot_lo) / scale_poi, (sigma_tot_hi) / scale_poi, 510, "-L");
    axis_poi->ImportAxisAttributes(h->GetXaxis());
    axis_poi->SetName("axis_poi");
    if (useRelativeImpact) axis_poi->SetTitle("#Delta#hat{#mu}/#Delta#hat{#mu}_{tot}");
    else axis_poi->SetTitle("#Delta#hat{#mu}");
    axis_poi->SetTitleOffset(1.1);
    axis_poi->SetLineColor(kBlack);
    axis_poi->SetLabelColor(kBlack);
    axis_poi->SetTitleColor(kBlack);
    axis_poi->SetLabelSize(0.034);
    axis_poi->SetTitleSize(0.034);

    // axis for the nuisance parameter pull
    TGaxis *axis_theta = new TGaxis(border_lo, -offset, border_hi, -offset, (-sigma_tot_lo / max_poi) / scale_theta, (sigma_tot_hi / max_poi) / scale_theta, 510, "+R");
    axis_theta->ImportAxisAttributes(h->GetXaxis());
    axis_theta->SetName("axis_theta");
    axis_theta->SetTitle("(#hat{#theta} - #theta_{0})/#Delta#theta");
    axis_theta->SetTitleOffset(1.1);
    axis_theta->SetLineColor(kBlack);
    axis_theta->SetLabelColor(kBlack);
    axis_theta->SetTitleColor(kBlack);
    axis_theta->SetLabelSize(0.034);
    axis_theta->SetTitleSize(0.034);

    // axis for the nuisance parameter labels
    TGaxis *axis_label = new TGaxis(border_lo, 0, border_lo, nrNuis+1, 0, nrNuis+1, 0, "-R");
    axis_label->SetLineColor(kBlack);
    axis_label->SetTitleColor(kWhite);
    axis_label->SetLabelSize(0);
    axis_label->SetNdivisions(0);

    // some line definitions
    TLine l;
    l.SetLineWidth(2);
    l.SetLineColor(color_pulls);
    l.SetLineStyle(2);
  
    TLine l_stat;
    l_stat.SetLineWidth(2);
    l_stat.SetLineColor(color_staterror);
    l_stat.SetLineStyle(2);

    TLine l_syst;
    l_syst.SetLineWidth(2);
    l_syst.SetLineColor(color_systerror);
    l_syst.SetLineStyle(2);

    TLine l_tot;
    l_tot.SetLineWidth(2);
    l_tot.SetLineColor(color_totalerror);
    l_tot.SetLineStyle(2);

    // draw the nuisance parameter pulls including error bands and impact on poi
    gr_shades->Draw("p2");

    if (drawPrefitImpactBand) {
        gr_poi_nom->Draw("p2");
        if (overlay != "") gr_poi_nom_ol->Draw("p2");
    }
    if (drawPostfitImpactBand) {
        gr_poi->Draw("p2");
        if (overlay != "") gr_poi_ol->Draw("p2");
    }

    // draw axes
    if (drawPrefitImpactBand || drawPostfitImpactBand || drawErrorBars) axis_poi->Draw();
    axis_theta->Draw();
    axis_label->Draw();

    // draw +-1 and 0 sigma lines for pulls
    l.DrawLine( 0.              , 0.,  0.              , nrNuis);
    l.DrawLine( 1. * scale_theta, 0.,  1. * scale_theta, nrNuis);
    l.DrawLine(-1. * scale_theta, 0., -1. * scale_theta, nrNuis);

    gStyle->SetEndErrorSize(5.0);
    gr1s->Draw("p");
    if (overlay != "") gr1s_ol->Draw("p");
    
    
    
    

    

    // draw syst and stat errors
    
    if (drawErrorBars) {
        l_stat.SetLineStyle(1);
        l_syst.SetLineStyle(1);
        l_tot.SetLineStyle(1);

        l_stat.DrawLine(-sigma_stat_lo * scale_poi / max_poi, 1.07*nrNuis,  sigma_stat_hi * scale_poi / max_poi, 1.07*nrNuis);
        l_stat.DrawLine( sigma_stat_hi * scale_poi / max_poi, 1.06*nrNuis,  sigma_stat_hi * scale_poi / max_poi, 1.08*nrNuis);
        l_stat.DrawLine(-sigma_stat_lo * scale_poi / max_poi, 1.06*nrNuis, -sigma_stat_lo * scale_poi / max_poi, 1.08*nrNuis);

        l_syst.DrawLine(-sigma_syst_lo * scale_poi / max_poi, 1.10*nrNuis,  sigma_syst_hi * scale_poi / max_poi, 1.10*nrNuis);
        l_syst.DrawLine( sigma_syst_hi * scale_poi / max_poi, 1.09*nrNuis,  sigma_syst_hi * scale_poi / max_poi, 1.11*nrNuis);
        l_syst.DrawLine(-sigma_syst_lo * scale_poi / max_poi, 1.09*nrNuis, -sigma_syst_lo * scale_poi / max_poi, 1.11*nrNuis);
    
        l_tot.DrawLine(-sigma_tot_lo * scale_poi / max_poi, 1.13*nrNuis,  sigma_tot_hi * scale_poi / max_poi, 1.13*nrNuis);
        l_tot.DrawLine( sigma_tot_hi * scale_poi / max_poi, 1.12*nrNuis,  sigma_tot_hi * scale_poi / max_poi, 1.14*nrNuis);
        l_tot.DrawLine(-sigma_tot_lo * scale_poi / max_poi, 1.12*nrNuis, -sigma_tot_lo * scale_poi / max_poi, 1.14*nrNuis);

        TLatex t_stat;
        TLatex t_syst;
        TLatex t_tot;
    
        t_stat.SetTextSize(0.03);
        t_stat.SetTextAlign(32);
        t_stat.SetTextColor(color_staterror);
        t_stat.DrawLatex((-sigma_stat_lo-0.025) * scale_poi / max_poi, 1.07*nrNuis, "statistics");
    
        t_syst.SetTextSize(0.03);
        t_syst.SetTextAlign(32);
        t_syst.SetTextColor(color_systerror);
        t_syst.DrawLatex((-sigma_syst_lo-0.025) * scale_poi / max_poi, 1.10*nrNuis, "systematics");
    
        t_tot.SetTextSize(0.03);
        t_tot.SetTextAlign(32);
        t_tot.SetTextColor(color_totalerror);
        t_tot.DrawLatex((-sigma_tot_lo-0.025) * scale_poi / max_poi, 1.13*nrNuis, "total");
    
        t_stat.Draw();
        t_syst.Draw();
        t_tot.Draw();
    }

    gr->Draw("p");
    if (overlay != "") gr_ol->Draw("p");

    pad1->SetTicks(0, 0);

    c1->SetLogy(0);

    TLegend* leg = makeLeg();
    leg->SetX1(channelPosX+0.34);
    leg->SetY1(channelPosY-0.0775);
    leg->SetX2(channelPosX + 0.8);
    leg->SetY2(channelPosY+0.02);
    leg->SetTextSize(0.0225);

    leg->AddEntry(gr, "Pull","lp");
    if (nrNuis_ol > 0) leg->AddEntry(gr_ol, "Alt pull","lp");
    leg->AddEntry(gr1s, "1 standard deviation","l");
    if (nrNuis_ol > 0) leg->AddEntry(gr1s_ol, "Alt 1 standard deviation","l");
    if (drawPrefitImpactBand) {
        leg->AddEntry(gr_poi_nom, "Prefit Impact on #hat#mu", "f");
        if (nrNuis_ol > 0) leg->AddEntry(gr_poi_nom_ol, "Alt Prefit Impact on #hat#mu", "f");
    }
    if (drawPostfitImpactBand) {
        leg->AddEntry(gr_poi, "Postfit Impact on #hat#mu", "f");
        if (nrNuis_ol > 0) leg->AddEntry(gr_poi_ol, "Alt Postfit Impact on #hat#mu", "f");
    }

    leg->Draw();

    // draw the normalizations
    if (drawInset) {
        pad2->cd();

        TLine l2;
        l2.SetLineWidth(2);
        l2.SetLineColor(13);
        l2.SetLineStyle(2);

        h2->Draw();
        l2.DrawLine(1., 0., 1., nrNFs);
        gr_nf->Draw("p");
    }
}

// ____________________________________________________________________________|__________
// dump ROOT file to ascii table
void PullPlotter::ROOT2Ascii(string folder) {
    system("mkdir -vp ascii");

    enum ConvertMode {pulls, breakdown};

    ConvertMode mode;
    if (folder.find("pulls") != string::npos) {
        mode = pulls;
    } else if (folder.find("breakdown") != string::npos) {
        mode = pulls;
    } else {
        cout << "ERROR::Something went wrong." << endl;
        exit(1);
    }

    bool ignoreInfNan = 1;

    std::vector<TString> list;

    TSystemDirectory  dire(("root-files/"+folder).c_str(), ("root-files/"+folder).c_str());

    TList *files = dire.GetListOfFiles();
    TIter next(files);
    TSystemFile *file;
    TString fname;
    while((file = (TSystemFile*)next())) {
        fname = file->GetName();
        if(file->IsDirectory()) continue;

        if (removeHbb && fname.Contains("Hbb")) continue;
        if (removeHtt && (fname.Contains("Htt") || fname.Contains("ATLAS_TAU"))) continue;

        list.push_back(fname.Data());
    }
    
    stringstream outFileName;
    outFileName << "ascii/" << folder;
    
    ofstream outFile((outFileName.str()+".txt").c_str());
    ofstream outFile_id((outFileName.str()+"_id.txt").c_str());
    ofstream outFile_nf((outFileName.str()+"_nf.txt").c_str());
    ofstream outFile_nf_id((outFileName.str()+"_nf_id.txt").c_str());

    int nrNPs = 0;
    int nrNFs = 0;

    for(vector<TString>::iterator it = list.begin(); it != list.end(); it++) {
        stringstream fileName;
        fileName << "root-files/" << folder << "/" << *it;

        bool isNorm = false;
        bool success = true;
        bool isInfNan = false;
    
        TFile* f = NULL;
        f = new TFile(fileName.str().c_str());
        if (f && !f->IsOpen()) success = false;

        if (success) cout << "O";
        else cout << "X";
        cout << " File: " << fileName.str() << endl;
        if (!success) continue;

        TString histoName = it->ReplaceAll(".root", "");

        // if (histoName.Contains("atlas_nbkg_")) isNorm = true;
        // if (histoName.Contains("slope_")) isNorm = true;
        // if (histoName.Contains("p0_")) isNorm = true;
        // if (histoName.Contains("p1_")) isNorm = true;
        // if (histoName.Contains("p2_")) isNorm = true;
        // if (histoName.Contains("p3_")) isNorm = true;
        if (histoName.Contains("ATLAS_norm") /*&& histoName.Contains("lvlv")*/) isNorm = true;
        // if (histoName.Contains("ATLAS_Hbb_norm_")) isNorm = true;
        // if (histoName.Contains("ATLAS_PM_EFF_")) isNorm = true;
        // if (histoName.Contains("scale_norm")) isNorm = true;
        if (histoName.Contains("scale_") && !histoName.Contains("QCDscale_")) isNorm = true;
        if (histoName.Contains("mu_BR")) isNorm = true;

        TH1D* hist = (TH1D*)f->Get(histoName);

        int nrBins = hist->GetNbinsX();

        for (int bin = 1; bin <= nrBins; bin++) {
            double number = hist->GetBinContent(bin);
            
            // check inf
            if (number > 10e9) isInfNan = true;

            // check nan
            if (number != number) isInfNan = true;
        }
    
        if (ignoreInfNan && isInfNan) {
            cout << "WARNING::Skipping " << *it << " because of inf/nan" << endl;
            continue;
        }

        (isNorm?outFile_nf:outFile) << (isNorm?nrNPs++:nrNFs++) << " ";

        for (int bin = 1; bin <= nrBins; bin++) {
            double number = hist->GetBinContent(bin);
            (isNorm?outFile_nf:outFile) << number << ((bin < nrBins)?" ":"\n");
        }

        if (isNorm) outFile_nf_id << * it << "\n";
        else outFile_id << * it << "\n";

        f->Close();
    }

    cout << "Writing to file: " << outFileName.str() << "*.txt" << endl;

    outFile.close();
    outFile_id.close();
    
    if (mode == pulls) {
        outFile_nf.close();
        outFile_nf_id.close();
    }
}

// ____________________________________________________________________________|__________
// initialize file
void PullPlotter::loadFile(const char* fileName, int cols, fileHolder file) {
    ifstream testFile(fileName);
    if (testFile.fail()) {
        cout << "ERROR::file " << fileName << "does not exist.";
        exit(1);
    }
    drawPlot(fileName, cols, file);
}

// ____________________________________________________________________________|__________
// Return vector of strings from textfile
vector<string> PullPlotter::getLabel(const char* fileName, int nrPars) {
    vector<string> tmp_labels;
    Int_t nlines = 0;
    ifstream idFile(fileName);
    while (1) {
        if (!idFile.good() || nlines > nrPars) break;
        string label;
        idFile >> label;
        tmp_labels.push_back(label);
        nlines++;
    }
    return tmp_labels;
}
void PullPlotter::applyOverlay(string in, string& out, string suf) {
  if (in == "") return;
  vector<string> parsed = parseString(in,":");
  if (parsed.size() > 1) {
    in = parsed[0];
  }
  out = in+suf;
  for (int i=1;i<(int)parsed.size();i++) {
    out += ":"+parsed[i];
  }
}

void PullPlotter:: applyOverlay(string in, string& out) {
  vector<string> parsed = parseString(in,":");
  if (parsed.size() > 1) {
    out = parsed[1];
  }
}

void PullPlotter::computeFlags(string cardName) {
  if (!overridden) {
    dogg_4l_lvlv = 0;
    dogg_4l      = 0;
    dogg         = 0;
    dollll       = 0;
    dollqq       = 0;
    dollvv       = 0;
    dolvqq       = 0;
    dolvlv       = 0;
    doll         = 0;
    dolh         = 0;
    dowh         = 0;
    dozh         = 0;
    dovh         = 0;
    dozz         = 0;
    doww         = 0;
    dott         = 0;
    docb         = 0;
    dolog        = 1;
    drawBands    = 0;
    showLabel    = 1;
    // labelTxt = "";
    // if (1) docb = 1;

    doLogx = !dozoom && docb;

    if (cardName.find("2012") != string::npos || cardName.find("12_") != string::npos || cardName.find("2013") != string::npos) {
      do2012 = true;
    }
    else if (cardName.find("2011") != string::npos || cardName.find("11_") != string::npos) {
      do2011 = true;
    }

    if (do2011) energy = "7";
    if (do2012) energy = "8";
    if (do2011 && do2012) lumi += "+";

    if (cardName.find("gg_4l_lvlv") != string::npos) {
      dogg_4l_lvlv = true;
    }
    else if (cardName.find("gg_4l") != string::npos) {
      if (do2011) {
        lumi   = "4.9";
        energy = "7";
        year   = "2011";
      }
      else if (do2012) {
        lumi   = "3.4";
        energy = "8";
        year   = "2012";
      }
      else {
        lumi   = "4.9+3.2";
        energy = "7+8";
        year   = "2011+2012";
      }
      channel_label = "#gamma#gamma+4l";
      dogg_4l = true;
    }
    else if (cardName.find("gg") != string::npos) {
      lumi_2011 = "4.8";
      lumi_2012 = "5.9";
      if (do2011) {
        lumi   = "4.9";
        energy = "7";
        year   = "2011";
      }
      else if (do2012) {
        lumi   = "5.9";
        energy = "8";
        year   = "2012";
      }
      else {
        lumi   = "4.9+3.2";
        energy = "7+8";
        year   = "2011+2012";
      }
      dogg = true;
    }
    else if (cardName.find("llll") != string::npos) {
      lumi_2011 = "4.8";
      lumi_2012 = "5.8";
      if (do2011) {
        lumi   = "4.8";
        energy = "7";
        year   = "2011";
      }
      else if (do2012) {
        lumi   = "5.8";
        energy = "8";
        year   = "2012";
      }
      else {
        lumi   = "4.8+3.2";
        energy = "7+8";
        year   = "2011+2012";
      }
      dollll = true;
    }
    else if (cardName.find("_llqq_") != string::npos) {
      dollqq = true;
    }
    else if (cardName.find("_llvv_") != string::npos) {
      dollvv = true;
    }
    else if (cardName.find("_lvqq_") != string::npos) {
      dolvqq = true;
    }
    else if (cardName.find("lvlv") != string::npos) {
      lumi_2011 = "4.7";
      lumi_2012 = "20.7";
      lumi      = "4.7+20.7";
      year      = "2011+2012";
      if (do2011) {
        lumi   = "4.7";
        energy = "7";
      }
      if (do2011 && do2012) {
        lumi += "+";
      }
      if (do2012) {
        lumi   = "20.7";
        energy = "8";
      }
      doLogx = false;
      dolvlv = true;
    }
    else if (cardName.find("_tt_") != string::npos) {
      dott = true;
    }
    else if (cardName.find("_lh_") != string::npos) {
      dolh = true;
    }
    else if (cardName.find("_wh_") != string::npos) {
      dowh = true;
    }
    else if (cardName.find("_zh_") != string::npos) {
      dozh = true;
    }
    else if (cardName.find("_vh_") != string::npos) {
      dovh = true;
    }
    else if (cardName.find("_ww_") != string::npos) {
      doww = true;
    }
    else if (cardName.find("_zz_") != string::npos) {
      dozz = true;
    }
    else if (cardName.find("_tt_") != string::npos) {
      dott = true;
    }
    else {
      lumi_2011 = "4.6-4.8";
      lumi_2012 = "5.8-5.9";
      if (do2011) {
        lumi = "4.6-4.9";
      }
      else if (do2012) {
        lumi = "5.8-5.9";
      }
      else {
        lumi = "4.9+5.8";
      }
      if (do2011) {
        year = "2011";
      }
      else if (do2012) {
        year = "2012";
      }
      else {
        year = "2011+2012";
      }
      docb = true;
    }
  }
}

void  PullPlotter::drawPlot(string cardName, int nrCols, fileHolder& numbers) {
  numbers.setNrCols(nrCols);
  numbers.readInFile(cardName);
  vector<double> vec_massPoints_tmp = numbers.massPoints;
  int nrNumbers = vec_massPoints_tmp.size();
  for (int i=0;i<nrNumbers;i++) {
    if (vec_massPoints_tmp[i] < minMass || vec_massPoints_tmp[i] > maxMass) {
      numbers.removeMass(vec_massPoints_tmp[i]);
    }
  }
}

double*  PullPlotter::getAry(vector<double> numbers) {
  int nrPoints = numbers.size();
  double* ary = new double[nrPoints];
  for (int i=0;i<nrPoints;i++) {
    ary[i] = numbers[i];
  }
  return ary;
}

TGraph*  PullPlotter::makeGraph(string title, int n, double* x_ary, double* y_ary) {
  TGraph* graph = new TGraph(n, x_ary, y_ary);
  graph->SetTitle("");
  graph->GetXaxis()->SetTitle("m_{H} [GeV]");
  graph->GetYaxis()->SetTitle(title.c_str());
  return graph;
}

TGraphAsymmErrors*  PullPlotter::makeGraphErr(string title, int n, double* x_ary, double* central, double* errlo, double* errhi) {
  TGraphAsymmErrors* graph = new TGraphAsymmErrors(n, x_ary, central, NULL, NULL, errlo, errhi);
  graph->SetTitle("");
  graph->GetXaxis()->SetTitle("m_{H} [GeV]");
  graph->GetYaxis()->SetTitle(title.c_str());
  return graph;
}

TGraphAsymmErrors*  PullPlotter::makeGraphErr(string title, int n, double* x_ary, double* central, double* cenlo, double* cenhi, double* errlo, double* errhi) {
  TGraphAsymmErrors* graph = new TGraphAsymmErrors(n, x_ary, central, cenlo, cenhi, errlo, errhi);
  graph->SetTitle("");
  graph->GetXaxis()->SetTitle("m_{H} [GeV]");
  graph->GetYaxis()->SetTitle(title.c_str());
  return graph;
}

TLegend*  PullPlotter::makeLeg() {
  xmin_leg  = labelPosX;
  xdiff_leg = 0.22;
  ymax_leg  = 0.74;

  if (overlay != "") {
    ymax_leg = 0.74;
    ydiff_leg += 0.005;
  }
  if (overlay2 != "") {
    ydiff_leg += 0.005;
  }
  if (overlay3 != "") {
    ydiff_leg += 0.005;
  }

  TLegend* leg = new TLegend(xmin_leg, ymax_leg-ydiff_leg, xmin_leg+xdiff_leg, ymax_leg, "", "NDC");
  leg->SetFillStyle(0);
  leg->SetTextSize(textSize);
  leg->SetBorderSize(0);
  leg->SetFillColor(0);
  return leg;
}

TLegend*  PullPlotter::makeLeg2()
{
  xmin_leg  = 0.32;
  xdiff_leg = 0.22;
  ymax_leg  = 0.72;

  if (overlay != "") {
    ymax_leg = 0.74;
    ydiff_leg += 0.005;
  }
  if (overlay2 != "") {
    ydiff_leg += 0.005;
  }
  if (overlay3 != "") {
    ydiff_leg += 0.005;
  }

  TLegend* leg = new TLegend(xmin_leg+xdiff_leg, ymax_leg-ydiff_leg/2, xmin_leg+2*xdiff_leg, ymax_leg, "", "NDC");
  leg->SetFillStyle(0);
  leg->SetTextSize(textSize);
  leg->SetBorderSize(0);
  leg->SetFillColor(0);
  return leg;
}

void  PullPlotter::save(string baseName, string type, TCanvas* c1) {
  c_saved = c1;
  saveName_saved = baseName;
  system(("mkdir -vp " + type + "-files").c_str());
  stringstream saveName;
  saveName << type << "-files/" << baseName << "." << type;
  if (doSave) c1->SaveAs(saveName.str().c_str());
}

void  PullPlotter::save() {
  bool doSave_save = doSave;
  doSave = 1;
  save(saveName_saved,"eps",c_saved);
  save(saveName_saved,"pdf",c_saved);
  save(saveName_saved,"C",c_saved);
  doSave=doSave_save;
}

void  PullPlotter::addButton() {
  TButton *but1 = new TButton("For Approval","34+56",labelPosX,labelPosY+0.04,labelPosX+0.2,labelPosY+0.1);
  but1->Draw();
}

void  PullPlotter::drawInsert() {
  txtPosY = 0.742;
  txtPosX = 0.51;

  if (doInj) {
    txtPosY -= 0.05;
  }
  if (dolvlv) {
    txtPosX += 0.05;
  }

  TLatex t;
  t.SetNDC();
  t.SetTextSize(textSize);

  if (do2011 || do2012) {
    stringstream lumiLatex;
    lumiLatex << "#sqrt{s} = " << energy << " TeV: #lower[-0.2]{#scale[0.6]{#int}}Ldt = " << lumi << " fb^{-1}";
    t.DrawLatex(labelPosX,0.75,lumiLatex.str().c_str());
  }
  else {
    stringstream lumiLatex1;
    lumiLatex1 << "#splitline{#sqrt{s} = 7 TeV:  #lower[-0.2]{#scale[0.6]{#int}}Ldt = " << lumi_2011 << " fb^{-1}}{";
    lumiLatex1 << "#sqrt{s} = 8 TeV:  #lower[-0.2]{#scale[0.6]{#int}}Ldt = " << lumi_2012 << " fb^{-1}}";
    t.DrawLatex(txtPosX,txtPosY,lumiLatex1.str().c_str());
  }

  if (showLabel) {
    ATLASLabel(labelPosX,labelPosY,labelTxt.c_str(),1);
  }
  if (forApproval) {
    addButton();
  }
  // if (!dozoom) {
  //   if (doLogx) {
  //     c1->SetLogx(1);
  //   }
  // }
}

void  PullPlotter::drawTopRight(string s) {
  TLatex t;
  t.SetTextSize(textSize);
  t.SetNDC();
  t.DrawLatex(labelPosX + 0.455, labelPosY, s.c_str());
}
