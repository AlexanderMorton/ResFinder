#include "ResonanceFinder/VHAnalysisRunner.h"

#include "ResonanceFinder/ManualHistoCollector.h"
#include "ResonanceFinder/HistoLumiRescaler.h"
#include "ResonanceFinder/DownVariationAdder.h"

RF::VHAnalysisRunner::VHAnalysisRunner(TString analysis) : RF::IBinnedAnalysisRunner(analysis, new RF::ManualHistoCollector())
{  
  m_dataScale = 1;
  m_oldLumi = 1;
  m_newLumi = 1;
}

RF::VHAnalysisRunner::~VHAnalysisRunner()
{
}

void RF::VHAnalysisRunner::configInputGetter()
{
   RF::ManualHistoCollector *mhc = dynamic_cast<RF::ManualHistoCollector*>(m_histoCollector);

   // manually add data (TreeHistoCollector automatically does that, but ManualHistoCollector
   // does not, as it is a low-level class allowing disrespect of rigid directory structure)

   for (auto &kv : m_selections) {
      // selection string is ignored
      mhc->addSample(kv.first, mhc->getHistoCollection().tagData());
   }
}

void RF::VHAnalysisRunner::setLumiRescale(Double_t oldLumi, Double_t newLumi) {
  m_oldLumi = oldLumi;
  m_newLumi = newLumi;
}

void RF::VHAnalysisRunner::addOneSideVariation(TString variation, TString tagUp, TString tagDn)
{

  if (variation != "") {
    bool alreadyAdded =  std::find(m_onesidevar.begin(), m_onesidevar.end(), variation) != m_onesidevar.end();
    if (!alreadyAdded) {
      m_onesidevar.push_back(variation);
      m_onesidetagUp.push_back(tagUp);
      m_onesidetagDn.push_back(tagDn);
    }

  }
  return ;
}

void RF::VHAnalysisRunner::manipulate()
{

  RF::HistoCollection &hc = histoCollection();

  int idx = 0;
  for (auto kv : m_onesidevar) {
    // add down variations
    if (kv != "") {
      RF::DownVariationAdder dva;
      dva.setVariation(kv + m_onesidetagUp[idx], kv + m_onesidetagDn[idx]);
      dva.manipulate(hc);
    }
    idx++;
  }



  // first, rescale to luminosity
  if (m_oldLumi != m_newLumi) {
    RF::HistoLumiRescaler hlr;
    hlr.setLumi(m_oldLumi, m_newLumi);
    hlr.manipulate(hc);
  }




  for (auto &kv : hc.histos()){

    // apply data scale if it's data
    if(kv.first.Contains(hc.tagData())) kv.second->Scale(m_dataScale);

    // apply any signal scales
    for (auto &ks : m_sigScales){
      if(kv.first.Contains(ks.first)){
	kv.second->Scale(ks.second);
      }
      
      if((ks.first == TString("All")) || (ks.first == TString("all"))){
	for(auto sig : signals()){
	  if(kv.first.Contains(sig)){ //this is a signal, oK to scale
	    kv.second->Scale(ks.second);	
	  }
	}
      }
    }
  
  
    // apply any bkgd scales
    for (auto &kb : m_bkgdScales){
      if(kv.first.Contains(kb.first)) kv.second->Scale(kb.second);
    }
  }

}

void RF::VHAnalysisRunner::setTagData(TString tag)
{
  histoCollection().setTagData(tag);
}

void RF::VHAnalysisRunner::setTagNominal(TString tag)
{
  histoCollection().setTagNominal(tag);
}

void RF::VHAnalysisRunner::scaleSignal(double sf, TString name){
  if(sf == 1) return;
  
  for( auto sig : signals()){
    if(sig==name){
      if(m_sigScales.find(sig) == m_sigScales.end()){
	m_sigScales[sig] = sf;
      }
      else cout << "Error: already have a scale for this signal: " << name << endl;
    }
  }

  if((name==TString("All")) || (name==TString("all"))){
    if(m_sigScales.find(name) == m_sigScales.end()){
      m_sigScales[name] = sf;   
    }   
  }

}

void RF::VHAnalysisRunner::scaleBkgd(double sf, TString name){
  if(sf == 1) return;
  if(name.IsNull()) return;

  if(m_bkgdScales.find(name) == m_bkgdScales.end()) m_bkgdScales[name] = sf;
  else cout << "Error: already have a scale for this background: " << name << endl;
}

void RF::VHAnalysisRunner::scaleData(double sf){
  m_dataScale = sf;
}
