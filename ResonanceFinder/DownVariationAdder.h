#ifndef __RF_DownVariationAdder_h__
#define __RF_DownVariationAdder_h__

///
/// \brief DownVariationAdder - add down variation to HistoCollection
/// \author Valerio Ippolito - Harvard University
/// \date May 8, 2015
///
/// DownVariationAdder is a simple class which takes an HistoCollection as an input,
/// and creates a missing down variation using
///     2*nominal - variation_up
/// (for example, for pt resolution down).
///

#include "ResonanceFinder/IHistoManipulator.h"

namespace RF {
   class DownVariationAdder : public IHistoManipulator {
   public:
      DownVariationAdder();
      ~DownVariationAdder();

      void setVariation(TString name_up, TString name_down);
      TString variation_up() const;
      TString variation_down() const;

      virtual void manipulate(HistoCollection &hc);

   private:
      TString m_variation_up;
      TString m_variation_down;
   };
}

#endif
