/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Zhu Li <phillipszhuli1990@gmail.com>
 *           Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-stack-helper.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-stack-helper.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-stack-helper.h.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <limits>
#include <map>

#include <ns3-dev/ns3/assert.h>
#include <ns3-dev/ns3/callback.h>
#include <ns3-dev/ns3/channel.h>
#include <ns3-dev/ns3/config.h>
#include <ns3-dev/ns3/core-config.h>
#include <ns3-dev/ns3/data-rate.h>
#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/names.h>
#include <ns3-dev/ns3/net-device.h>
#include <ns3-dev/ns3/node.h>
#include <ns3-dev/ns3/node-list.h>
#include <ns3-dev/ns3/object.h>
#include <ns3-dev/ns3/packet-socket-factory.h>
#include <ns3-dev/ns3/point-to-point-helper.h>
#include <ns3-dev/ns3/point-to-point-net-device.h>
#include <ns3-dev/ns3/simulator.h>
#include <ns3-dev/ns3/string.h>

#include "nnn-net-device-face.h"
#include "nnn-l3-protocol.h"

#include "nnn-forwarding-strategy.h"
#include "nnn-address.h"
#include "nnn-face-container.h"
#include "nnn-stack-helper.h"

NS_LOG_COMPONENT_DEFINE ("nnn.NNNStackHelper");

namespace ns3 {
namespace nnn {

NNNStackHelper::NNNStackHelper ()
{
  m_nnnFactory.         SetTypeId ("ns3::nnn::NNNL3Protocol");
  m_nnnforwardingstrategyFactory.    SetTypeId ("ns3::nnn::fw::NNNForwardingStrategy");
  m_nnstFactory.         SetTypeId ("ns3::nnn::nnst::Default");

  m_netDeviceCallbacks.push_back (std::make_pair (PointToPointNetDevice::GetTypeId (), MakeCallback (&NNNStackHelper::PointToPointNetDeviceCallback, this)));
  // default callback will be fired if non of others callbacks fit or did the job
}

NNNStackHelper::~NNNStackHelper ()
{
}

void
NNNStackHelper::SetStackAttributes (const std::string &attr1, const std::string &value1,
                                 const std::string &attr2, const std::string &value2,
                                 const std::string &attr3, const std::string &value3,
                                 const std::string &attr4, const std::string &value4)
{
  if (attr1 != "")
      m_nnnFactory.Set (attr1, StringValue (value1));
  if (attr2 != "")
      m_nnnFactory.Set (attr2, StringValue (value2));
  if (attr3 != "")
      m_nnnFactory.Set (attr3, StringValue (value3));
  if (attr4 != "")
      m_nnnFactory.Set (attr4, StringValue (value4));
}

void
NNNStackHelper::SetNNNForwardingStrategy (const std::string &NNNForwardingStrategy,
                                    const std::string &attr1, const std::string &value1,
                                    const std::string &attr2, const std::string &value2,
                                    const std::string &attr3, const std::string &value3,
                                    const std::string &attr4, const std::string &value4)
{
	m_nnnforwardingstrategyFactory.SetTypeId (NNNForwardingStrategy);
  if (attr1 != "")
	  m_nnnforwardingstrategyFactory.Set (attr1, StringValue (value1));
  if (attr2 != "")
	  m_nnnforwardingstrategyFactory.Set (attr2, StringValue (value2));
  if (attr3 != "")
	  m_nnnforwardingstrategyFactory.Set (attr3, StringValue (value3));
  if (attr4 != "")
	  m_nnnforwardingstrategyFactory.Set (attr4, StringValue (value4));
}

void
NNNStackHelper::SetNnst (const std::string &nnstClass,
                     const std::string &attr1, const std::string &value1,
                     const std::string &attr2, const std::string &value2,
                     const std::string &attr3, const std::string &value3,
                     const std::string &attr4, const std::string &value4)
{
  m_nnstFactory.SetTypeId (nnstClass);
  if (attr1 != "")
      m_nnstFactory.Set (attr1, StringValue (value1));
  if (attr2 != "")
      m_nnstFactory.Set (attr2, StringValue (value2));
  if (attr3 != "")
      m_nnstFactory.Set (attr3, StringValue (value3));
  if (attr4 != "")
      m_nnstFactory.Set (attr4, StringValue (value4));
}

void
NNNStackHelper::SetDefaultRoutes (bool needSet)
{
  NS_LOG_FUNCTION (this << needSet);
  m_needSetDefaultRoutes = needSet;
}

Ptr<FaceContainer>
NNNStackHelper::Install (const NodeContainer &c) const
{
  Ptr<FaceContainer> faces = Create<FaceContainer> ();
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      faces->AddAll (Install (*i));
    }
  return faces;
}

Ptr<FaceContainer>
NNNStackHelper::InstallAll () const
{
  return Install (NodeContainer::GetGlobal ());
}

Ptr<FaceContainer>
NNNStackHelper::Install (Ptr<Node> node) const
{
  // NS_ASSERT_MSG (m_forwarding, "SetForwardingHelper() should be set prior calling Install() method");
  Ptr<FaceContainer> faces = Create<FaceContainer> ();

  if (node->GetObject<NNNL3Protocol> () != 0)
    {
      NS_FATAL_ERROR ("NNNStackHelper::Install (): Installing "
                      "a NnnStack to a node with an existing Nnn object");
      return 0;
    }

  // Create NNNL3Protocol
  Ptr<NNNL3Protocol> nnn = m_nnnFactory.Create<NNNL3Protocol> ();

  // Create and aggregate NNST
  Ptr<Nnst> nnst = m_nnstFactory.Create<Nnst> ();
  nnn->AggregateObject (nnst);

  // Create and aggregate PIT
  //nnn->AggregateObject (m_pitFactory.Create<Pit> ());

  // Create and aggregate forwarding strategy
  nnn->AggregateObject (m_nnnforwardingstrategyFactory.Create<NNNForwardingStrategy> ());

  // Create and aggregate content store
 // nnn->AggregateObject (m_contentStoreFactory.Create<ContentStore> ());

  // Aggregate NNNL3Protocol on node
  node->AggregateObject (nnn);

  for (uint32_t index=0; index < node->GetNDevices (); index++)
    {
      Ptr<NetDevice> device = node->GetDevice (index);
      // This check does not make sense: LoopbackNetDevice is installed only if IP stack is installed,
      // Normally, nnnSIM works without IP stack, so no reason to check
      // if (DynamicCast<LoopbackNetDevice> (device) != 0)
      //   continue; // don't create face for a LoopbackNetDevice

      Ptr<NetDeviceFace> face;

      for (std::list< std::pair<TypeId, NetDeviceFaceCreateCallback> >::const_iterator item = m_netDeviceCallbacks.begin ();
           item != m_netDeviceCallbacks.end ();
           item++)
        {
          if (device->GetInstanceTypeId () == item->first ||
              device->GetInstanceTypeId ().IsChildOf (item->first))
            {
              face = item->second (node, nnn, device);
              if (face != 0)
                break;
            }
        }
      if (face == 0)
        {
          face = DefaultNetDeviceCallback (node, nnn, device);
        }

      if (m_needSetDefaultRoutes)
        {
          // default route with lowest priority possible
          AddRoute (node, "/", StaticCast<Face> (face), std::numeric_limits<int32_t>::max ());
        }

      face->SetUp ();
      faces->Add (face);
    }

  return faces;
}

void
NNNStackHelper::AddNetDeviceFaceCreateCallback (TypeId netDeviceType, NNNStackHelper::NetDeviceFaceCreateCallback callback)
{
  m_netDeviceCallbacks.push_back (std::make_pair (netDeviceType, callback));
}

void
NNNStackHelper::UpdateNetDeviceFaceCreateCallback (TypeId netDeviceType, NetDeviceFaceCreateCallback callback)
{
  for (NetDeviceCallbackList::iterator i = m_netDeviceCallbacks.begin (); i != m_netDeviceCallbacks.end (); i++)
    {
      if (i->first == netDeviceType)
        {
          i->second = callback;
          return;
        }
    }
}

void
NNNStackHelper::RemoveNetDeviceFaceCreateCallback (TypeId netDeviceType, NetDeviceFaceCreateCallback callback)
{
  for (NetDeviceCallbackList::iterator i = m_netDeviceCallbacks.begin (); i != m_netDeviceCallbacks.end (); i++)
    {
      if (i->first == netDeviceType)
        {
          m_netDeviceCallbacks.erase (i);
          return;
        }
    }
}

Ptr<NetDeviceFace>
NNNStackHelper::DefaultNetDeviceCallback (Ptr<Node> node, Ptr<NNNL3Protocol> nnn, Ptr<NetDevice> netDevice) const
{
  NS_LOG_DEBUG ("Creating default NetDeviceFace on node " << node->GetId ());

  Ptr<NetDeviceFace> face = CreateObject<NetDeviceFace> (node, netDevice);

  nnn->AddFace (face);
  NS_LOG_LOGIC ("Node " << node->GetId () << ": added NetDeviceFace as face #" << *face);

  return face;
}

Ptr<NetDeviceFace>
NNNStackHelper::PointToPointNetDeviceCallback (Ptr<Node> node, Ptr<NNNL3Protocol> nnn, Ptr<NetDevice> device) const
{
  NS_LOG_DEBUG ("Creating point-to-point NetDeviceFace on node " << node->GetId ());

  Ptr<NetDeviceFace> face = CreateObject<NetDeviceFace> (node, device);

  nnn->AddFace (face);
  NS_LOG_LOGIC ("Node " << node->GetId () << ": added NetDeviceFace as face #" << *face);

/*  if (m_limitsEnabled)
    {
      Ptr<Limits> limits = face->GetObject<Limits> ();
      if (limits == 0)
        {
          NS_FATAL_ERROR ("Limits are enabled, but the selected forwarding strategy does not support limits. Please revise your scenario");
          exit (1);
        }

      NS_LOG_INFO ("Limits are enabled");
      Ptr<PointToPointNetDevice> p2p = DynamicCast<PointToPointNetDevice> (device);
      if (p2p != 0)
        {
          // Setup bucket filtering
          // Assume that we know average data packet size, and this size is equal default size
          // Set maximum buckets (averaging over 1 second)

          DataRateValue dataRate; device->GetAttribute ("DataRate", dataRate);
          TimeValue linkDelay;   device->GetChannel ()->GetAttribute ("Delay", linkDelay);

          NS_LOG_INFO("DataRate for this link is " << dataRate.Get());

          double maxInterestPackets = 1.0  * dataRate.Get ().GetBitRate () / 8.0 / (m_avgDataSize + m_avgInterestSize);
          // NS_LOG_INFO ("Max packets per second: " << maxInterestPackets);
          // NS_LOG_INFO ("Max burst: " << m_avgRtt.ToDouble (Time::S) * maxInterestPackets);
          NS_LOG_INFO ("MaxLimit: " << (int)(m_avgRtt.ToDouble (Time::S) * maxInterestPackets));

          // Set max to BDP
          limits->SetLimits (maxInterestPackets, m_avgRtt.ToDouble (Time::S));
          limits->SetLinkDelay (linkDelay.Get ().ToDouble (Time::S));
        }
    }
*/
  return face;
}


Ptr<FaceContainer>
NNNStackHelper::Install (const std::string &nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return Install (node);
}


void
NNNStackHelper::AddRoute (Ptr<Node> node, const std::string &prefix, Ptr<Face> face, int32_t metric)
{
  NS_LOG_LOGIC ("[" << node->GetId () << "]$ route add " << prefix << " via " << *face << " metric " << metric);

  Ptr<Nnst>  nnst  = node->GetObject<Nnst> ();

  NameValue prefixValue;
  prefixValue.DeserializeFromString (prefix, MakeNameChecker ());
  nnst->Add (prefixValue.Get (), face, metric);
}

void
NNNStackHelper::AddRoute (Ptr<Node> node, const std::string &prefix, uint32_t faceId, int32_t metric)
{
  Ptr<NNNL3Protocol>     nnn = node->GetObject<NNNL3Protocol> ();
  NS_ASSERT_MSG (nnn != 0, "Nnn stack should be installed on the node");

  Ptr<Face> face = nnn->GetFace (faceId);
  NS_ASSERT_MSG (face != 0, "Face with ID [" << faceId << "] does not exist on node [" << node->GetId () << "]");

  AddRoute (node, prefix, face, metric);
}

void
NNNStackHelper::AddRoute (const std::string &nodeName, const std::string &prefix, uint32_t faceId, int32_t metric)
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  NS_ASSERT_MSG (node != 0, "Node [" << nodeName << "] does not exist");

  Ptr<NNNL3Protocol>     nnn = node->GetObject<NNNL3Protocol> ();
  NS_ASSERT_MSG (nnn != 0, "nnn stack should be installed on the node");

  Ptr<Face> face = nnn->GetFace (faceId);
  NS_ASSERT_MSG (face != 0, "Face with ID [" << faceId << "] does not exist on node [" << nodeName << "]");

  AddRoute (node, prefix, face, metric);
}

void
NNNStackHelper::AddRoute (Ptr<Node> node, const std::string &prefix, Ptr<Node> otherNode, int32_t metric)
{
  for (uint32_t deviceId = 0; deviceId < node->GetNDevices (); deviceId ++)
    {
      Ptr<PointToPointNetDevice> netDevice = DynamicCast<PointToPointNetDevice> (node->GetDevice (deviceId));
      if (netDevice == 0)
        continue;

      Ptr<Channel> channel = netDevice->GetChannel ();
      if (channel == 0)
        continue;

      if (channel->GetDevice (0)->GetNode () == otherNode ||
          channel->GetDevice (1)->GetNode () == otherNode)
        {
          Ptr<NNNL3Protocol> nnn = node->GetObject<NNNL3Protocol> ();
          NS_ASSERT_MSG (nnn != 0, "Nnn stack should be installed on the node");

          Ptr<Face> face = nnn->GetFaceByNetDevice (netDevice);
          NS_ASSERT_MSG (face != 0, "There is no face associated with the p2p link");

          AddRoute (node, prefix, face, metric);

          return;
        }
    }

  NS_FATAL_ERROR ("Cannot add route: Node# " << node->GetId () << " and Node# " << otherNode->GetId () << " are not connected");
}

void
NNNStackHelper::AddRoute (const std::string &nodeName, const std::string &prefix, const std::string &otherNodeName, int32_t metric)
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  NS_ASSERT_MSG (node != 0, "Node [" << nodeName << "] does not exist");

  Ptr<Node> otherNode = Names::Find<Node> (otherNodeName);
  NS_ASSERT_MSG (otherNode != 0, "Node [" << otherNodeName << "] does not exist");

  AddRoute (node, prefix, otherNode, metric);
}


} // namespace nnn
} // namespace ns3
