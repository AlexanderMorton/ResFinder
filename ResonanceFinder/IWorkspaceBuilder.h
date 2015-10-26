#ifndef __RF_IWorkspaceBuilder_h__
#define __RF_IWorkspaceBuilder_h__

///
/// \brief IWorkspaceBuilder - interface class for WorkspaceBuilder objects
/// \author Valerio Ippolito - Harvard University
/// \date Mar 23, 2015
///
/// IWorkspaceBuilder is the interface for all WorkspaceBuilder objects. It
/// holds a list of channels to be used for building the likelihood model,
/// and a pointer to the generated workspace. Concrete implementations have
/// to implement the build() method, and deal with the production of the
/// RooStats ModelConfig (using the rigid input structure provided by the
/// Channel hierarchy). The wsName() method, which returns the standard name
/// of the created workspace, and the analogous modelConfigName() method,
/// must also be implemented.
///

#include "ResonanceFinder/Channel.h"

#include <map>
#include <stdexcept>

class TString;
class RooWorkspace;

namespace RF {
   class IWorkspaceBuilder {
   public:
      inline IWorkspaceBuilder(TString name);
      inline virtual ~IWorkspaceBuilder() = 0;

      inline TString name() const;
      inline void setName(TString name);
      inline TString poi() const;
      inline void setPOI(TString name);
      inline TString dest() const;
      inline void setDest(TString name);

      inline void addChannel(TString name);
      inline void addChannel(const Channel &channel);
      inline Channel& channel(TString name);
      inline void resetChannels();

      virtual void build() = 0;
      virtual const char *wsName() = 0;
      virtual const char *modelConfigName() = 0;

      inline RooWorkspace* ws();

   protected:
      inline void setWS(RooWorkspace *w);
      inline std::vector<TString> channels();

   private:
      TString m_name;
      TString m_poi;
      TString m_dest;
      RooWorkspace* m_workspace;
      std::map<TString, Channel> m_channels;
   };
}

RF::IWorkspaceBuilder::IWorkspaceBuilder(TString name) : m_name(name), m_workspace(nullptr)
{
}

RF::IWorkspaceBuilder::~IWorkspaceBuilder()
{
}

TString RF::IWorkspaceBuilder::name() const
{
   return m_name;
}

void RF::IWorkspaceBuilder::setName(TString name)
{
   m_name = name;
}

TString RF::IWorkspaceBuilder::poi() const
{
   return m_poi;
}

void RF::IWorkspaceBuilder::setPOI(TString name)
{
   m_poi = name;
}

TString RF::IWorkspaceBuilder::dest() const
{
   return m_dest;
}

void RF::IWorkspaceBuilder::setDest(TString name)
{
   m_dest = name;
}

void RF::IWorkspaceBuilder::addChannel(TString name)
{
   if (m_channels.find(name) != m_channels.end()) throw std::runtime_error("Channel already defined");
   m_channels[name] = RF::Channel(name);
}

void RF::IWorkspaceBuilder::addChannel(const Channel &channel)
{
   if (m_channels.find(channel.name()) != m_channels.end()) throw std::runtime_error("Channel already defined");
   m_channels[channel.name()] = channel;
}

RF::Channel& RF::IWorkspaceBuilder::channel(TString name)
{
   if (m_channels.find(name) == m_channels.end()) throw std::runtime_error("Channel unknown");
   return m_channels[name];
}

void RF::IWorkspaceBuilder::resetChannels()
{
   m_channels.clear();
}

RooWorkspace* RF::IWorkspaceBuilder::ws()
{
   return m_workspace;
}

void RF::IWorkspaceBuilder::setWS(RooWorkspace *w)
{
   m_workspace = w;
}

std::vector<TString> RF::IWorkspaceBuilder::channels()
{
   std::vector<TString> result;
   for (auto i : m_channels) result.push_back(i.first);
   return result;
}

#endif
