#ifndef __RF_MultiplicativeFactor_h__
#define __RF_MultiplicativeFactor_h__

///
/// \brief MultiplicativeFactor - class representing a multiplicative factor
/// \author Valerio Ippolito - Harvard University
/// \date Mar 23, 2015
///
/// MultiplicativeFactor is a class representing a multiplicative factor,
/// i.e. something changing yields associated to a Sample by a given amount
/// (which can be free to fit data, possibly with a constraint, or just
/// remain constant).
///

#include <TString.h>

namespace RF {
   class MultiplicativeFactor {
   public:
      typedef enum {
         CONSTANT,
         FREE,
         GAUSSIAN,
         LOGNORMAL
      } ConstraintType;

      MultiplicativeFactor();
      MultiplicativeFactor(TString name, Double_t value);
      MultiplicativeFactor(TString name, Double_t value, Double_t min, Double_t max);
      MultiplicativeFactor(TString name, Double_t value, Double_t min, Double_t max, ConstraintType constraint);
      ~MultiplicativeFactor();

      TString name() const;
      void setName(TString name);
      Double_t value() const;
      void setValue(Double_t value);
      Double_t min() const;
      void setMin(Double_t min);
      Double_t max() const;
      void setMax(Double_t max);
      ConstraintType constraint() const;
      void setConstraint(ConstraintType constraint);
   private:
      TString m_name;
      Double_t m_value;
      Double_t m_min;
      Double_t m_max;
      ConstraintType m_constraint;
   };
}

#endif
