#ifndef __RF_Variation_h__
#define __RF_Variation_h__

///
/// \brief Variation - class representing a variation (e.g. shape uncertainty)
/// \author Valerio Ippolito - Harvard University
/// \date Mar 23, 2015
///
/// Variation is a class representing a variation, which can be in the binned
/// workspace case a shape uncertainty (and is, in general, a shape+normalisation
/// effect).
///

#include <TString.h>

namespace RF {
   class Variation {
   public:
      Variation();
      Variation(TString name);
      Variation(TString name, TString name_up, TString name_down);
      ~Variation();

      void setNames(TString name);
      void setNames(TString name, TString name_up, TString name_down);
      TString name()const ;
      void setName(TString name);
      TString name_up()const ;
      void setNameUp(TString name_up);
      TString name_down()const ;
      void setNameDown(TString name_down);

   private:
      TString m_name; // real name
      TString m_name_up; // name characterising the input (i.e. tree name)
      TString m_name_down;
   };
}

#endif
