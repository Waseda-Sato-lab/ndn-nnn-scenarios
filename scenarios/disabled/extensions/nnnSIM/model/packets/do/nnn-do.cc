/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-do.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-do.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-do.cc.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/unused.h>
#include <ns3-dev/ns3/packet.h>

#include "nnn-do.h"

NS_LOG_COMPONENT_DEFINE ("nnn.DO");

namespace ns3 {
namespace nnn {

DO::DO ()
 : m_packetid (2)
 , m_ttl      (Seconds (0))
 , m_payload  (Create<Packet> ())
 , m_wire     (0)
{

}

DO::DO (Ptr<NNNAddress> name, Ptr<Packet> payload)
 : m_packetid (2)
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

DO::DO (const NNNAddress &name, Ptr<Packet> payload)
 : m_packetid (2)
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

DO::DO (const DO &do_p)
 : m_packetid (2)
 , m_ttl      (do_p.m_ttl)
 , m_name     (Create<NNNAddress> (do_p.GetName()))
 , m_payload  (do_p.GetPayload ()->Copy ())
 , m_wire     (0)
{
	NS_LOG_FUNCTION("DO correct copy constructor");
}

uint32_t
DO::GetPacketId ()
{
	return m_packetid;
}

void
DO::SetName (Ptr<NNNAddress> name)
{
	m_name = name;
	m_wire = 0;
}

void
DO::SetName (const NNNAddress &name)
{
	m_name = Create<NNNAddress> (name);
	m_wire = 0;
}


const NNNAddress&
DO::GetName () const
{
	if (m_name == 0) throw DOException ();
	return *m_name;
}


Ptr<const NNNAddress>
DO::GetNamePtr () const
{
	return m_name;
}

void
DO::SetLifetime (Time ttl)
{
	m_ttl = ttl;
	m_wire = 0;
}

Time
DO::GetLifetime () const
{
	return m_ttl;
}

void
DO::SetPayload (Ptr<Packet> payload)
{
	m_payload = payload;
	m_wire = 0;
}

Ptr<const Packet>
DO::GetPayload () const
{
	return m_payload;
}

void
DO::Print (std::ostream &os) const
{
	os << "<DO>\n";
	os << "  <TTL>" << GetLifetime () << "</TTL>\n";
	os << "  <Name>" << GetName () << "</Name>\n";
	if (m_payload != 0)
	{
		os << "  <Payload>Yes</Payload>\n";
	} else
	{
		os << "  <Payload>No</Payload>\n";
	}
	os << "</DO>";
}

} // namespace nnn
} // namespace ns3
