#include "ResonanceFinder/Variation.h"

RF::Variation::Variation() : m_name_up(""), m_name_down("")
{
}

RF::Variation::Variation(TString name)
{
   setNames(name);
}

RF::Variation::Variation(TString name, TString name_up, TString name_down) : m_name(name), m_name_up(name_up), m_name_down(name_down)
{
}

RF::Variation::~Variation()
{
}

void RF::Variation::setNames(TString name)
{
   m_name = name;
   // add automatically _up and _down
   m_name_up = name + "_up";
   m_name_down = name + "_down";
}

void RF::Variation::setNames(TString name, TString name_up, TString name_down)
{
   m_name = name;
   m_name_up = name_up;
   m_name_down = name_down;
}

TString RF::Variation::name() const
{
   return m_name;
}

void RF::Variation::setName(TString name)
{
   m_name = name;
}

TString RF::Variation::name_up() const
{
   return m_name_up;
}

void RF::Variation::setNameUp(TString name_up)
{
   m_name_up = name_up;
}

TString RF::Variation::name_down() const
{
   return m_name_down;
}

void RF::Variation::setNameDown(TString name_down)
{
   m_name_down = name_down;
}
