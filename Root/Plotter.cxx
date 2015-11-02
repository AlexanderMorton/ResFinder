#include "ResonanceFinder/Plotter.h"

#include "ResonanceFinder/StatisticalResultsCollection.h"
#include <TCanvas.h>
#include <TGraphAsymmErrors.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TLatex.h>
#include <TLine.h>
#include <TMath.h>
#include <Math/DistFunc.h>

RF::Plotter::Plotter(TString name, TString dest) : m_varName("mass"), m_varUnit("GeV"), m_name(name), m_dest(dest)
{
   setOutputFormat(RF::Plotter::OutputFormat::png);
}

RF::Plotter::~Plotter()
{
}

TString RF::Plotter::name() const
{
   return m_name;
}

void RF::Plotter::setName(TString name)
{
   m_name = name;
}

TString RF::Plotter::dest() const
{
   return m_dest;
}

void RF::Plotter::setDest(TString name)
{
   m_dest = name;
}

TString RF::Plotter::varName() const
{
   return m_varName;
}

void RF::Plotter::setVarName(TString name)
{
   m_varName = name;
}

TString RF::Plotter::varUnit() const
{
   return m_varUnit;
}

void RF::Plotter::setVarUnit(TString name)
{
   m_varUnit = name;
}

RF::Plotter::OutputFormat RF::Plotter::outputFormat() const
{
   return m_format;
}

void RF::Plotter::setOutputFormat(RF::Plotter::OutputFormat format)
{
   m_format = format;

   if (format == RF::Plotter::OutputFormat::pdf) {
      m_formatString = ".pdf";
   } else if (format == RF::Plotter::OutputFormat::png) {
      m_formatString = ".png";
   } else if (format == RF::Plotter::OutputFormat::eps) {
      m_formatString = ".eps";
   } else if (format == RF::Plotter::OutputFormat::root) {
      m_formatString = ".root";
   } else if (format == RF::Plotter::OutputFormat::C) {
      m_formatString = ".C";
   } else throw std::logic_error("Format not implemented");
}

RF::Plotter::SampleSpec& RF::Plotter::getSpec(Double_t point)
{
   if (m_spec.find(point) == m_spec.end()) throw std::runtime_error("Requesting specifications for an unknow mass point");

   return m_spec[point];
}

void RF::Plotter::setSpec(Double_t point, Double_t br, Double_t xsec)
{
   m_spec[point].branching_ratio = br;
   m_spec[point].cross_section = xsec;
}

void RF::Plotter::process(RF::StatisticalResultsCollection &data)
{
   // determine file path prefix
   const TString prefix = m_dest + "/" + m_name;

   plot_p0(data, prefix);
   plot_limit(data, prefix);
}

void RF::Plotter::plot_p0(RF::StatisticalResultsCollection &data, TString prefix)
{
   const TString obj_suffix = TString(prefix).ReplaceAll("/", "_").ReplaceAll(".", "p").ReplaceAll("~", "_");

   TCanvas *c1 = new TCanvas("c_p0_" + obj_suffix, "p0_" + obj_suffix, 600, 600);

   TGraph *g = new TGraph();
   g->SetNameTitle("g_p0_" + obj_suffix, "p0");

   UInt_t i = 0;
   Double_t maxLimit = 1000;
   Double_t minLimit = 1 - ROOT::Math::gaussian_cdf(6);
   for (auto &kv : data.results()) {
      g->SetPoint(i, kv.first, kv.second.null_pvalue);
      if (kv.second.null_pvalue) minLimit = TMath::Min(minLimit, kv.second.null_pvalue); // protected against 0 p-value
      i++;
   }
   g->SetMinimum(minLimit);
   g->SetMaximum(maxLimit);

   const TString xtitle = (m_varUnit == "") ? m_varName : m_varName + " [" + m_varUnit + "]";
   const TString ytitle = "Local p_{0}";
   g->GetXaxis()->SetTitle(xtitle);
   g->GetXaxis()->SetNdivisions(505);
   g->GetYaxis()->SetTitle(ytitle);
   g->GetYaxis()->SetNdivisions(505);
   g->Draw("apl");

   c1->Update();

   // draw sigma lines
   TLine l;
   l.SetLineWidth(2);
   l.SetLineStyle(2);
   l.SetLineColor(kRed);
   const Double_t xmin = g->GetXaxis()->GetXmin();
   const Double_t xmax = g->GetXaxis()->GetXmax();
   l.SetLineColor(kBlack);
   l.DrawLine(xmin, 1, xmax, 1);

   l.SetLineColor(kRed);
   l.DrawLine(xmin, 1 - ROOT::Math::gaussian_cdf(0), xmax, 1 - ROOT::Math::gaussian_cdf(0));

   TLatex t;
   const Double_t delta = (xmax - xmin) * 0.02;
   t.DrawLatex(xmax + delta, (1 - ROOT::Math::gaussian_cdf(0)) * 0.8, "#color[2]{0#sigma}");

   Int_t n = 1;
   while (minLimit < 1 - ROOT::Math::gaussian_cdf(n)) {
      l.DrawLine(xmin, 1 - ROOT::Math::gaussian_cdf(n), xmax, 1 - ROOT::Math::gaussian_cdf(n));
      TString cmd = TString::Format("#color[2]{%d#sigma}", n);
      t.DrawLatex(xmax + delta, (1 - ROOT::Math::gaussian_cdf(n)) * 0.4, cmd);
      n++;
   }
   c1->SetLogy(1);


   c1->SaveAs(prefix + "_p0" + m_formatString);

   delete c1;
}

void RF::Plotter::plot_limit(RF::StatisticalResultsCollection &data, TString prefix)
{
   const TString obj_suffix = TString(prefix).ReplaceAll("/", "_").ReplaceAll(".", "p").ReplaceAll("~", "_");

   TCanvas *c1 = new TCanvas("c_limit_" + obj_suffix, "limit_" + obj_suffix, 600, 600);

   TGraphAsymmErrors *g_exp_1s = new TGraphAsymmErrors(data.results().size());
   TGraphAsymmErrors *g_exp_2s = new TGraphAsymmErrors(data.results().size());
   TGraph *g_obs = new TGraph(data.results().size());
   TGraph *g_prediction = new TGraph(data.results().size());
   g_exp_1s->SetNameTitle("g_exp_1s_limit_" + obj_suffix, "95% CL expected limit (+/-1 #sigma)");
   g_exp_2s->SetNameTitle("g_exp_2s_limit_" + obj_suffix, "95% CL expected limit (+/-2 #sigma)");
   g_obs->SetNameTitle("g_obs_limit_" + obj_suffix, "95% CL observed limit");
   g_prediction->SetNameTitle("g_prediction_" + obj_suffix, "prediction");

   g_exp_1s->SetFillColor(kGreen);
   g_exp_2s->SetFillColor(kYellow);
   g_prediction->SetLineStyle(kDashed);
   g_prediction->SetLineColor(kBlue + 1);

   UInt_t i = 0;
   for (auto &kv : data.results()) {
      if (m_spec.find(kv.first) == m_spec.end()) throw std::runtime_error("Cross-section info not found");
      const Double_t mu_to_xsec = m_spec[kv.first].cross_section / m_spec[kv.first].branching_ratio;
      const Double_t exp = kv.second.exp_upperlimit * mu_to_xsec;
      const Double_t exp_p1 = kv.second.exp_upperlimit_plus1 * mu_to_xsec;
      const Double_t exp_p2 = kv.second.exp_upperlimit_plus2 * mu_to_xsec;
      const Double_t exp_m1 = kv.second.exp_upperlimit_minus1 * mu_to_xsec;
      const Double_t exp_m2 = kv.second.exp_upperlimit_minus2 * mu_to_xsec;
      const Double_t obs = kv.second.obs_upperlimit * mu_to_xsec;
      const Double_t pred = mu_to_xsec;

      g_exp_1s->SetPoint(i, kv.first, exp);
      g_exp_2s->SetPoint(i, kv.first, exp);
      g_obs->SetPoint(i, kv.first, obs);
      g_prediction->SetPoint(i, kv.first, pred);

      g_exp_1s->SetPointError(i, 0, 0, exp - exp_m1, exp_p1 - exp);
      g_exp_2s->SetPointError(i, 0, 0, exp - exp_m2, exp_p2 - exp);

      i++;
   }

   const TString xtitle = (m_varUnit == "") ? m_varName : m_varName + " [" + m_varUnit + "]";
   const TString ytitle = "95% CL cross-section limit";
   g_exp_2s->GetXaxis()->SetTitle(xtitle);
   g_exp_2s->GetXaxis()->SetNdivisions(505);
   g_exp_2s->GetYaxis()->SetTitle(ytitle);

   g_exp_2s->Draw("al3");
   g_exp_1s->Draw("l3");
   g_obs->Draw("lp");
   g_prediction->Draw("l");

   c1->Update();
   c1->SetLogy(1);
   c1->SaveAs(prefix + "_limit" + m_formatString);
}
