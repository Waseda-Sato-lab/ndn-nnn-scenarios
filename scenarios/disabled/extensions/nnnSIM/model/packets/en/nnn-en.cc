/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *	         Zhu Li <philipszhuli1990@ruri.waseda.jp>
 *
 *  nnn-en.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-en.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-en.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ns3-dev/ns3/log.h>

#include "nnn-en.h"

NS_LOG_COMPONENT_DEFINE ("nnn.EN");

namespace ns3 {

class Packet;

namespace nnn {

EN::EN ()
 : m_packetid (3)
 , m_ttl      (Seconds (0))
 , m_poa_type (0)
 , m_poas     (std::vector<Mac48Address> ())
 , m_wire     (0)
{

}

EN::EN (std::vector<Mac48Address> signatures)
 : m_packetid (3)
 , m_ttl      (Seconds (1))
 , m_poa_type (0)
 , m_poas     (signatures)
 , m_wire     (0)
{
}

EN::EN (const EN &en_p)
 : m_packetid  (3)
 , m_ttl       (en_p.m_ttl)
 , m_poa_type  (en_p.m_poa_type)
 , m_poas      (en_p.m_poas)
 , m_wire      (0)
{
	NS_LOG_FUNCTION("EN correct copy constructor");
}

uint32_t
EN::GetPacketId ()
{
	return m_packetid;
}

uint16_t
EN::GetPoaType () const
{
	return m_poa_type;
}

void
EN::SetPoaType (uint16_t type)
{
	m_poa_type = type;
}

void
EN::AddPoa (Mac48Address signature)
{
	m_poas.push_back(signature);
}

void
EN::AddPoa (std::vector<Mac48Address> signatures)
{
	m_poas.insert(m_poas.end (), signatures.begin (), signatures.end ());
}

uint32_t
EN::GetNumPoa () const
{
	return m_poas.size();
}

std::vector<Mac48Address>
EN::GetPoas () const
{
	return m_poas;
}

Mac48Address
EN::GetOnePoa (uint32_t index) const
{
	if (index < GetNumPoa ())
		return m_poas[index];
	else
		return Mac48Address();
}

void
EN::SetLifetime (Time ttl)
{
	m_ttl = ttl;
	m_wire = 0;
}

Time
EN::GetLifetime () const
{
	return m_ttl;
}

void
EN::Print (std::ostream &os) const
{
	uint32_t num = GetNumPoa ();
	uint16_t type = GetPoaType ();
	os << "<EN>\n";
	os << "  <TTL>" << GetLifetime () << "</TTL>\n";
	os << "  <POATYPE>" << type << "</POATYPE>\n";
	os << "  <POANUM>" << num << "</POANUM>\n";
	for (int i = 0; i < num; i++)
	{
		os << "  <POA" << i << ">" << m_poas[i] << "</POA" << i << ">\n";
	}
	os << "</EN>";
}

}
}
