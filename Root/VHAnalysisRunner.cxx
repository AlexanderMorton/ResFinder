#include "ResonanceFinder/VHAnalysisRunner.h"

#include "ResonanceFinder/ManualHistoCollector.h"

RF::VHAnalysisRunner::VHAnalysisRunner(TString analysis) : RF::IBinnedAnalysisRunner(analysis, new RF::ManualHistoCollector())
{  
  m_dataScale = 1;
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

void RF::VHAnalysisRunner::manipulate()
{

  for (auto &kv : histoCollection().histos()){

    // apply data scale if it's data
    if(kv.first.Contains(histoCollection().tagData())) kv.second->Scale(m_dataScale);


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
