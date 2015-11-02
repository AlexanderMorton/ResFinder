#include "ResonanceFinder/DownVariationAdder.h"

RF::DownVariationAdder::DownVariationAdder() : m_variation_up(""), m_variation_down("")
{
}

RF::DownVariationAdder::~DownVariationAdder()
{
}

void RF::DownVariationAdder::setVariation(TString name_up, TString name_down)
{
   m_variation_up = name_up;
   m_variation_down = name_down;
}

TString RF::DownVariationAdder::variation_up() const
{
   return m_variation_up;
}

TString RF::DownVariationAdder::variation_down() const
{
   return m_variation_down;
}

void RF::DownVariationAdder::manipulate(HistoCollection &hc)
{
   UInt_t n_changed(0);
   
   for (auto chan : hc.channels()) {
     for (auto sample : hc.samples(chan)) {
       RF::Histo_t *h_up = hc.getHistogram(chan, sample, m_variation_up);
       RF::Histo_t *h_dn = hc.getHistogram(chan, sample, m_variation_down);
       if (h_up) {
	 // (examples: data, samples for which variation up is not a relevant source of uncertainty)
	 RF::Histo_t *nominal = hc.getHistogram(chan, sample, hc.tagNominal());
	 
	 RF::Histo_t *down = dynamic_cast<RF::Histo_t*>(nominal->Clone(TString::Format("%s_%s", h_up->GetName(), m_variation_down.Data())));
	 down->Scale(2.0);
	 down->Add(h_up, -1.0);
	 
	 hc.addHistogram(down, chan, sample, m_variation_down);
	 n_changed++;
       }
       else if (h_dn) {
	 // (examples: data, samples for which variation up is not a relevant source of uncertainty)
	 RF::Histo_t *nominal = hc.getHistogram(chan, sample, hc.tagNominal());
	 
	 RF::Histo_t *up = dynamic_cast<RF::Histo_t*>(nominal->Clone(TString::Format("%s_%s", h_dn->GetName(), m_variation_up.Data())));
	 up->Scale(2.0);
	 up->Add(h_dn, -1.0);
	 
	 hc.addHistogram(up, chan, sample, m_variation_up);
	 n_changed++;
       }
     }
   }
   
   if (n_changed == 0) throw std::runtime_error("DownVariationAdder called for a systematics whose up variation does not exist");
}
