/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Zhu Li <phillipszhuli1990@gmail.com>
 *           Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-header-helper.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-header-helper.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-header-helper.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/packet.h>
#include <ns3-dev/ns3/header.h>
#include <ns3-dev/ns3/object.h>

#include <iomanip>
#include "nnn-header-helper.h"

#include "../model/packets/nnn-packets.h"

NS_LOG_COMPONENT_DEFINE ("nnn.HeaderHelper");

namespace ns3 {
namespace nnn {

HeaderHelper::Type
HeaderHelper::GetNnnHeaderType (Ptr<const Packet> packet)
{
	// Our header ID is of 32 bits
	uint8_t type[4];
	uint32_t read = packet->CopyData (type, 4);

	if (read != 4) throw UnknownHeaderException();

	// Convert the bytes to a 32 bit unsigned integer
	uint32_t retval = 0;
	retval |= type[0];
	retval <<= 8;
	retval |= type[1];
	retval <<= 8;
	retval |= type[2];
	retval <<= 8;
	retval |= type[3];

	NS_LOG_DEBUG (*packet);

	switch (retval)
	{
	case 0:
		return HeaderHelper::NULL_NNN;
	case 1:
		return HeaderHelper::SO_NNN;
	case 2:
		return HeaderHelper::DO_NNN;
	case 3:
		return HeaderHelper::EN_NNN;
	case 4:
		return HeaderHelper::AEN_NNN;
	case 5:
		return HeaderHelper::REN_NNN;
	case 6:
		return HeaderHelper::INF_NNN;
	}

	NS_LOG_DEBUG (*packet);
	throw UnknownHeaderException();
}

} // namespace ndn
} // namespace ns3
