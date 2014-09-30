/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-so.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-so.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-so.cc.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/unused.h>
#include <ns3-dev/ns3/packet.h>

#include "nnn-so.h"

NS_LOG_COMPONENT_DEFINE ("nnn.SO");

namespace ns3 {
namespace nnn {

SO::SO ()
 : m_packetid (1)
 , m_ttl      (Seconds (0))
 , m_payload  (Create<Packet> ())
 , m_wire     (0)
{

}

SO::SO (Ptr<NNNAddress> name, Ptr<Packet> payload)
 : m_packetid (1)
 , m_ttl      (Seconds (1))
 , m_name     (name)
 , m_wire     (0)
{
	if (m_payload == 0)
	{
		m_payload = Create<Packet> ();
	} else
	{
		m_payload = payload;
	}
}

SO::SO (const NNNAddress &name, Ptr<Packet> payload)
 : m_packetid (1)
 , m_ttl      (Seconds (1))
 , m_name     (Create<NNNAddress> (name))
 , m_wire     (0)
{
	if (m_payload == 0)
	{
		m_payload = Create<Packet> ();
	} else
	{
		m_payload = payload;
	}
}

SO::SO (const SO &so_p)
 : m_packetid (1)
 , m_ttl      (so_p.m_ttl)
 , m_name     (Create<NNNAddress> (so_p.GetName()))
 , m_payload  (so_p.GetPayload ()->Copy ())
 , m_wire     (0)
{
	NS_LOG_FUNCTION("SO correct copy constructor");
}

uint32_t
SO::GetPacketId ()
{
	return m_packetid;
}

void
SO::SetName (Ptr<NNNAddress> name)
{
	m_name = name;
	m_wire = 0;
}

void
SO::SetName (const NNNAddress &name)
{
	m_name = Create<NNNAddress> (name);
	m_wire = 0;
}


const NNNAddress&
SO::GetName () const
{
	if (m_name == 0) throw SOException ();
	return *m_name;
}


Ptr<const NNNAddress>
SO::GetNamePtr () const
{
	return m_name;
}

void
SO::SetLifetime (Time ttl)
{
	m_ttl = ttl;
	m_wire = 0;
}

Time
SO::GetLifetime () const
{
	return m_ttl;
}

void
SO::SetPayload (Ptr<Packet> payload)
{
	m_payload = payload;
	m_wire = 0;
}

Ptr<const Packet>
SO::GetPayload () const
{
	return m_payload;
}

void
SO::Print (std::ostream &os) const
{
	os << "<SO>\n";
	os << "  <TTL>" << GetLifetime () << "</TTL>\n";
	os << "  <Name>" << GetName () << "</Name>\n";
	if (m_payload != 0)
	{
		os << "  <Payload>Yes</Payload>\n";
	} else
	{
		os << "  <Payload>No</Payload>\n";
	}
	os << "</SO>";
}

} // namespace nnn
} // namespace ns3
