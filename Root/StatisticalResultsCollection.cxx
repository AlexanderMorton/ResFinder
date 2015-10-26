#include "ResonanceFinder/StatisticalResultsCollection.h"

#include <TFile.h>
#include <TTree.h>
#include <stdexcept>

RF::StatisticalResultsCollection::StatisticalResultsCollection() : m_name(""), m_treeName("stats")
{
}

RF::StatisticalResultsCollection::StatisticalResultsCollection(TString name) : m_name(name), m_treeName("stats")
{
}

RF::StatisticalResultsCollection::~StatisticalResultsCollection()
{
}

TString RF::StatisticalResultsCollection::name() const
{
   return m_name;
}

void RF::StatisticalResultsCollection::setName(TString name)
{
   m_name = name;
}

TString RF::StatisticalResultsCollection::treeName() const
{
   return m_treeName;
}

void RF::StatisticalResultsCollection::setTreeName(TString name)
{
   m_treeName = name;
}

void RF::StatisticalResultsCollection::addResult(StatisticalResults &res, Double_t point)
{
   const Double_t key = point;

   if (m_results.find(key) == m_results.end()) {
      m_results[key]  = res;
   } else {
      throw std::runtime_error("StatisticalResults already read");
   }
}

void RF::StatisticalResultsCollection::addResults(StatisticalResultsCollection &other)
{
   for (auto &kv : other.results()) {
      const Double_t key = kv.first;

      if (m_results.find(key) == m_results.end()) {
         m_results[key] = kv.second;
      } else {
         throw std::runtime_error("StatisticalResults already read");
      }
   }
}

RF::StatisticalResults& RF::StatisticalResultsCollection::result(Double_t point)
{
   return m_results[point];
}

void RF::StatisticalResultsCollection::persist(TDirectory *file)
{
   file->cd();
   TTree *tree = new TTree(treeName(), name());
   tree->SetDirectory(file);

   Float_t tree_point;
   Float_t tree_null_pvalue;
   Float_t tree_null_pvalue_err;
   Float_t tree_alt_pvalue;
   Float_t tree_CLb;
   Float_t tree_CLs;
   Float_t tree_CLsplusb;
   Float_t tree_obs_lowerlimit;
   Float_t tree_obs_lowerlimit_err;
   Float_t tree_obs_upperlimit;
   Float_t tree_obs_upperlimit_err;
   Float_t tree_exp_upperlimit;
   Float_t tree_exp_upperlimit_plus1;
   Float_t tree_exp_upperlimit_plus2;
   Float_t tree_exp_upperlimit_minus1;
   Float_t tree_exp_upperlimit_minus2;
   tree->Branch("point", &tree_point, "point/F");;
   tree->Branch("null_pvalue", &tree_null_pvalue, "null_pvalue/F");;
   tree->Branch("null_pvalue_err", &tree_null_pvalue_err, "null_pvalue_err/F");;
   tree->Branch("alt_pvalue", &tree_alt_pvalue, "alt_pvalue/F");;
   tree->Branch("CLb", &tree_CLb, "CLb/F");;
   tree->Branch("CLs", &tree_CLs, "CLs/F");;
   tree->Branch("CLsplusb", &tree_CLsplusb, "CLsplusb/F");;
   tree->Branch("obs_lowerlimit", &tree_obs_lowerlimit, "obs_lowerlimit/F");;
   tree->Branch("obs_lowerlimit_err", &tree_obs_lowerlimit_err, "obs_lowerlimit_err/F");;
   tree->Branch("obs_upperlimit", &tree_obs_upperlimit, "obs_upperlimit/F");;
   tree->Branch("obs_upperlimit_err", &tree_obs_upperlimit_err, "obs_upperlimit_err/F");;
   tree->Branch("exp_upperlimit", &tree_exp_upperlimit, "exp_upperlimit/F");;
   tree->Branch("exp_upperlimit_plus1", &tree_exp_upperlimit_plus1, "exp_upperlimit_plus1/F");;
   tree->Branch("exp_upperlimit_plus2", &tree_exp_upperlimit_plus2, "exp_upperlimit_plus2/F");;
   tree->Branch("exp_upperlimit_minus1", &tree_exp_upperlimit_minus1, "exp_upperlimit_minus1/F");;
   tree->Branch("exp_upperlimit_minus2", &tree_exp_upperlimit_minus2, "exp_upperlimit_minus2/F");;

   for (auto & kv : m_results) {
      tree_point = kv.first;

      tree_null_pvalue = kv.second.null_pvalue;;
      tree_null_pvalue_err = kv.second.null_pvalue_err;;
      tree_alt_pvalue = kv.second.alt_pvalue;;
      tree_CLb = kv.second.CLb;;
      tree_CLs = kv.second.CLs;;
      tree_CLsplusb = kv.second.CLsplusb;;
      tree_obs_lowerlimit = kv.second.obs_lowerlimit;;
      tree_obs_lowerlimit_err = kv.second.obs_lowerlimit_err;;
      tree_obs_upperlimit = kv.second.obs_upperlimit;;
      tree_obs_upperlimit_err = kv.second.obs_upperlimit_err;;
      tree_exp_upperlimit = kv.second.exp_upperlimit;;
      tree_exp_upperlimit_plus1 = kv.second.exp_upperlimit_plus1;;
      tree_exp_upperlimit_plus2 = kv.second.exp_upperlimit_plus2;;
      tree_exp_upperlimit_minus1 = kv.second.exp_upperlimit_minus1;;
      tree_exp_upperlimit_minus2 = kv.second.exp_upperlimit_minus2;;
      tree->Fill();
   }

   file->Write();
}

void RF::StatisticalResultsCollection::persist(TString filename)
{
   TFile *file = new TFile(filename, "RECREATE");
   persist(file);
   delete file;
}

void RF::StatisticalResultsCollection::retrieve(TDirectory *file)
{
   reset();

   TTree *tree = dynamic_cast<TTree*>(file->Get(treeName()));
   if (!tree) throw std::runtime_error("tree not found");

   tree->SetDirectory(file);

   Float_t tree_point;
   Float_t tree_null_pvalue;
   Float_t tree_null_pvalue_err;
   Float_t tree_alt_pvalue;
   Float_t tree_CLb;
   Float_t tree_CLs;
   Float_t tree_CLsplusb;
   Float_t tree_obs_lowerlimit;
   Float_t tree_obs_lowerlimit_err;
   Float_t tree_obs_upperlimit;
   Float_t tree_obs_upperlimit_err;
   Float_t tree_exp_upperlimit;
   Float_t tree_exp_upperlimit_plus1;
   Float_t tree_exp_upperlimit_plus2;
   Float_t tree_exp_upperlimit_minus1;
   Float_t tree_exp_upperlimit_minus2;
   tree->SetBranchAddress("point", &tree_point);
   tree->SetBranchAddress("null_pvalue", &tree_null_pvalue);
   tree->SetBranchAddress("null_pvalue_err", &tree_null_pvalue_err);
   tree->SetBranchAddress("alt_pvalue", &tree_alt_pvalue);
   tree->SetBranchAddress("CLb", &tree_CLb);
   tree->SetBranchAddress("CLs", &tree_CLs);
   tree->SetBranchAddress("CLsplusb", &tree_CLsplusb);
   tree->SetBranchAddress("obs_lowerlimit", &tree_obs_lowerlimit);
   tree->SetBranchAddress("obs_lowerlimit_err", &tree_obs_lowerlimit_err);
   tree->SetBranchAddress("obs_upperlimit", &tree_obs_upperlimit);
   tree->SetBranchAddress("obs_upperlimit_err", &tree_obs_upperlimit_err);
   tree->SetBranchAddress("exp_upperlimit", &tree_exp_upperlimit);
   tree->SetBranchAddress("exp_upperlimit_plus1", &tree_exp_upperlimit_plus1);
   tree->SetBranchAddress("exp_upperlimit_plus2", &tree_exp_upperlimit_plus2);
   tree->SetBranchAddress("exp_upperlimit_minus1", &tree_exp_upperlimit_minus1);
   tree->SetBranchAddress("exp_upperlimit_minus2", &tree_exp_upperlimit_minus2);

   for (Long64_t entry = 0; entry < tree->GetEntries(); ++entry) {
      tree->GetEntry(entry);

      RF::StatisticalResults res;
      res.null_pvalue = tree_null_pvalue;
      res.null_pvalue_err = tree_null_pvalue_err;
      res.alt_pvalue = tree_alt_pvalue;
      res.CLb = tree_CLb;
      res.CLs = tree_CLs;
      res.CLsplusb = tree_CLsplusb;
      res.obs_lowerlimit = tree_obs_lowerlimit;
      res.obs_lowerlimit_err = tree_obs_lowerlimit_err;
      res.obs_upperlimit = tree_obs_upperlimit;
      res.obs_upperlimit_err = tree_obs_upperlimit_err;
      res.exp_upperlimit = tree_exp_upperlimit;
      res.exp_upperlimit_plus1 = tree_exp_upperlimit_plus1;
      res.exp_upperlimit_plus2 = tree_exp_upperlimit_plus2;
      res.exp_upperlimit_minus1 = tree_exp_upperlimit_minus1;
      res.exp_upperlimit_minus2 = tree_exp_upperlimit_minus2;

      addResult(res, tree_point);
   }
}

void RF::StatisticalResultsCollection::retrieve(TString filename)
{
   TFile *file = new TFile(filename);
   retrieve(file);
   delete file;
}

void RF::StatisticalResultsCollection::reset()
{
   m_results.clear();
}

std::vector<Double_t> RF::StatisticalResultsCollection::points()
{
   std::vector<Double_t> result;
   for (auto &kv : m_results) result.push_back(kv.first);
   return result;
}

RF::StatisticalResultsMap_t& RF::StatisticalResultsCollection::results()
{
   return m_results;
}

