/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Zhu Li <phillipszhuli1990@gmail.com>
 *           Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-net-device-face.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-net-device-face.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-net-device-face.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ns3-dev/ns3/channel.h>
#include <ns3-dev/ns3/net-device.h>
#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/packet.h>
#include <ns3-dev/ns3/node.h>
#include <ns3-dev/ns3/pointer.h>
#include <ns3-dev/ns3/point-to-point-net-device.h>

#include "naming/nnn-address.h"
#include "nnn-l3-protocol.h"
#include "nnn-net-device-face.h"

NS_LOG_COMPONENT_DEFINE ("nnn.NetDeviceFace");

namespace ns3 {
namespace nnn {

NS_OBJECT_ENSURE_REGISTERED (NetDeviceFace);

TypeId
NetDeviceFace::GetTypeId ()
{
	static TypeId tid = TypeId ("ns3::nnn::NetDeviceFace")
    		.SetParent<Face> ()
    		.SetGroupName ("Nnn")
    		;
	return tid;
}

/**
 * By default, Nnn face are created in the "down" state.  Before
 * becoming useable, the user must invoke SetUp on the face
 */
NetDeviceFace::NetDeviceFace (Ptr<Node> node, const Ptr<NetDevice> &netDevice)
: Face (node)
, m_netDevice (netDevice)
{
	NS_LOG_FUNCTION (this << netDevice);

	SetMetric (1); // default metric

	NS_ASSERT_MSG (m_netDevice != 0, "NetDeviceFace needs to be assigned a valid NetDevice");
}

NetDeviceFace::~NetDeviceFace ()
{
	NS_LOG_FUNCTION_NOARGS ();
}

NetDeviceFace& NetDeviceFace::operator= (const NetDeviceFace &)
{
	return *this;
}

Ptr<NetDevice>
NetDeviceFace::GetNetDevice () const
{
	return m_netDevice;
}

void
NetDeviceFace::RegisterProtocolHandlers (const NULLpHandler &NULLpHandler, const SOHandler &SOHandler,
		const DOHandler &DOHandler, const ENHandler &ENHandler,
		const AENHandler &AENHandler, const RENHandler &RENHandler,
		const INFHandler &INFHandler)
{
	NS_LOG_FUNCTION (this);

	Face::RegisterProtocolHandlers (NULLpHandler, SOHandler, DOHandler,
			ENHandler, AENHandler, RENHandler, INFHandler);

	m_node->RegisterProtocolHandler (MakeCallback (&NetDeviceFace::ReceiveFromNetDevice, this),
			NNNL3Protocol::ETHERNET_FRAME_TYPE, m_netDevice, true/*promiscuous mode*/);
}

void
NetDeviceFace:: UnRegisterProtocolHandlers ()
{
	m_node->UnregisterProtocolHandler (MakeCallback (&NetDeviceFace::ReceiveFromNetDevice, this));
	Face::UnRegisterProtocolHandlers ();
}

bool
NetDeviceFace::Send (Ptr<Packet> packet)
{
	if (!Face::Send (packet))
	{
		return false;
	}

	NS_LOG_FUNCTION (this << packet);

	NS_ASSERT_MSG (packet->GetSize () <= m_netDevice->GetMtu (),
			"Packet size " << packet->GetSize () << " exceeds device MTU "
			<< m_netDevice->GetMtu ()
			<< " for Nnn; fragmentation not supported");

	// Here is where I think we can use anything other than broadcast
	bool ok = m_netDevice->Send (packet, m_netDevice->GetBroadcast (),
			NNNL3Protocol::ETHERNET_FRAME_TYPE);
	return ok;
}

// callback
void
NetDeviceFace::ReceiveFromNetDevice (Ptr<NetDevice> device,
		Ptr<const Packet> p,
		uint16_t protocol,
		const Address &from,
		const Address &to,
		NetDevice::PacketType packetType)
{
	NS_LOG_FUNCTION (device << p << protocol << from << to << packetType);
	Receive (p);
}


std::ostream&
NetDeviceFace::Print (std::ostream& os) const
{
#ifdef NS3_LOG_ENABLE
	os << "dev[" << GetNode ()->GetId () << "]=net(" << GetId ();

	if (DynamicCast<PointToPointNetDevice> (m_netDevice))
	{
		// extra debugging information which available ONLY for PointToPointNetDevice's
		os << ",";
		os << DynamicCast<PointToPointNetDevice> (m_netDevice)->GetChannel ()->GetDevice (0)->GetNode ()->GetId ();
		os << "-";
		os << DynamicCast<PointToPointNetDevice> (m_netDevice)->GetChannel ()->GetDevice (1)->GetNode ()->GetId ();
	}
	os << ")";
#else
	os << "dev=net(" << GetId () << ")";
#endif
	return os;
}

} // namespace nnnsim
} // namespace ns3

