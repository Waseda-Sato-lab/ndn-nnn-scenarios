/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *           Zhu Li <philipszhuli1990@ruri.waseda.jp>
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
 *  along with nnn-en.cc.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/unused.h>
#include <ns3-dev/ns3/packet.h>

#include "nnn-aen.h"

NS_LOG_COMPONENT_DEFINE ("nnn.AEN");

namespace ns3 {
namespace nnn {

AEN::AEN ()
 : m_packetid (4)
 , m_ttl      (Seconds (0))
 , m_lease    ()
 , m_wire     (0)
{

}

AEN::AEN (Ptr<NNNAddress> name)
 : m_packetid (4)
 , m_ttl      (Seconds (300))
 , m_name     (name)
 , m_wire     (0)
{

}

AEN::AEN (const NNNAddress &name)
 : m_packetid (4)
 , m_ttl      (Seconds (300))
 , m_name     (Create<NNNAddress> (name))
 , m_wire     (0)
{

}


AEN::AEN (const AEN &aen_p)
 : m_packetid (4)
 , m_ttl      (aen_p.m_ttl)
 , m_lease    (Seconds (300))
 , m_name     (Create<NNNAddress> (aen_p.GetName()))
 , m_wire     (0)
{
	NS_LOG_FUNCTION("AEN correct copy constructor");
}

uint32_t
AEN::GetPacketId ()
{
	return m_packetid;
}

void
AEN::SetName (Ptr<NNNAddress> name)
{
	m_name = name;
	m_wire = 0;
}

void
AEN::SetName (const NNNAddress &name)
{
	m_name = Create<NNNAddress> (name);
	m_wire = 0;
}


const NNNAddress&
AEN::GetName () const
{
	if (m_name == 0) throw AENException ();
	return *m_name;
}


Ptr<const NNNAddress>
AEN::GetNamePtr () const
{
	return m_name;
}

void
AEN::SetLifetime (Time ttl)
{
	m_ttl = ttl;
	m_wire = 0;
}

Time
AEN::GetLifetime () const
{
	return m_ttl;
}

void
AEN::SetLeasetime (Time lease)
{
	m_lease = lease;
}

Time
AEN::GetLeasetime () const
{
	return m_lease;
}

void
AEN::Print (std::ostream &os) const
{
	os << "<AEN>\n";
	os << "  <TTL>" << GetLifetime () << "</TTL>\n";
	os << "  <Name>" << GetName () << "</Name>\n";
	os << "  <Lease>" << GetLeasetime () << "</Lease>\n";
	os << "</AEN>";
}

} // namespace nnn
} // namespace ns3
