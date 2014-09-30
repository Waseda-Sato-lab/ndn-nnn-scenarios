/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Zhu Li <phillipszhuli1990@gmail.com>
 *           Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-header-helper.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-header-helper.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-header-helper.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _NNN_HEADER_HELPER_H_
#define _NNN_HEADER_HELPER_H_

#include <ns3-dev/ns3/ptr.h>

namespace ns3 {

class Header;
class Packet;

namespace nnn {

class NNNAddress;

typedef NNNAddress NameComponents;

/**
 * @ingroup nnn-helpers
 *
 * \brief Class implementing functionality to detect Nnn packet type and
 * create the corresponding object
 *
 * Nnn doesn't really have a header, so we need this class to
 * determine type of Nnn packet and return corresponent header class,
 * NULLp, SO, DO, EN, AEN, REN or INF
 *
 * Throws UnknownHeaderException if header type couldn't be determined
 */
class HeaderHelper
{
public:
	/**
     @brief enum for Nnn packet types
	 */
	enum Type {NULL_NNN, SO_NNN, DO_NNN,
		EN_NNN, AEN_NNN, REN_NNN, INF_NNN};

	static Type
	GetNnnHeaderType (Ptr<const Packet> packet);
};

/**
 * \brief Exception thrown if NNN stack receives unrecognized message type
 */
class UnknownHeaderException {};

} // namespace nnn
} // namespace ns3

#endif // _NNN_HEADER_HELPER_H_
