/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  wire-nnnsim.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  wire-nnnsim.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with wire-nnnsim.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NNN_WIRE_NNNSIM_SYNTAX_H
#define NNN_WIRE_NNNSIM_SYNTAX_H

#include <ns3-dev/ns3/ptr.h>
#include <ns3-dev/ns3/nstime.h>
#include <ns3-dev/ns3/buffer.h>

#include "../nnn-common.h"
#include "../naming/nnn-address.h"

NNN_NAMESPACE_BEGIN

namespace wire {

/**
 * \brief Helper to encode ndnSIM wire elements
 */
class NnnSim
{
public:
	/**
	 * @brief Append Name in nnnSIM encoding
	 * @param start Buffer to store serialized Interest
	 * @param name constant reference to Name object
	 *
	 * @returns written length
	 */
	static size_t
	SerializeName (Buffer::Iterator &start, const NNNAddress &name);

	/**
	 * @brief Estimate size of Name in nnnSIM encoding
	 * @param name constant reference to Name object
	 * @returns estimated length
	 */
	static size_t
	SerializedSizeName (const NNNAddress &name);

	/**
	 * @brief Deserialize Name from nnnSIM encodeing
	 * @param start Buffer that stores serialized Interest
	 * @param name Name object
	 */
	static Ptr<NNNAddress>
	DeserializeName (Buffer::Iterator &start);
}; // NnnSim

} // wire

NNN_NAMESPACE_END

#endif // NNN_WIRE_NDNSIM_SYNTAX_H
