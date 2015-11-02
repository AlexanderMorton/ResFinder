#include "ResonanceFinder/MultiplicativeFactor.h"

RF::MultiplicativeFactor::MultiplicativeFactor() : m_name(""), m_value(0), m_min(0), m_max(0), m_constraint(RF::MultiplicativeFactor::CONSTANT)
{
}

RF::MultiplicativeFactor::MultiplicativeFactor(TString name, Double_t value) : m_name(name), m_value(value), m_min(0), m_max(0), m_constraint(RF::MultiplicativeFactor::CONSTANT)
{
}

RF::MultiplicativeFactor::MultiplicativeFactor(TString name, Double_t value, Double_t min, Double_t max) : m_name(name), m_value(value), m_min(min), m_max(max), m_constraint(RF::MultiplicativeFactor::FREE)
{
}

RF::MultiplicativeFactor::MultiplicativeFactor(TString name, Double_t value, Double_t min, Double_t max, RF::MultiplicativeFactor::ConstraintType constraint) : m_name(name), m_value(value), m_min(min), m_max(max), m_constraint(constraint)
{
}

RF::MultiplicativeFactor::~MultiplicativeFactor()
{
}

TString RF::MultiplicativeFactor::name() const
{
   return m_name;
}

void RF::MultiplicativeFactor::setName(TString name)
{
   m_name = name;
}

Double_t RF::MultiplicativeFactor::value()  const
{
   return m_value;
}

void RF::MultiplicativeFactor::setValue(Double_t value)
{
   m_value = value;
}

Double_t RF::MultiplicativeFactor::min()  const
{
   return m_min;
}

void RF::MultiplicativeFactor::setMin(Double_t min)
{
   m_min = min;
}

Double_t RF::MultiplicativeFactor::max()  const
{
   return m_max;
}

void RF::MultiplicativeFactor::setMax(Double_t max)
{
   m_max = max;
}

RF::MultiplicativeFactor::ConstraintType RF::MultiplicativeFactor::constraint()  const
{
   return m_constraint;
}

void RF::MultiplicativeFactor::setConstraint(RF::MultiplicativeFactor::ConstraintType constraint)
{
   m_constraint = constraint;
}
