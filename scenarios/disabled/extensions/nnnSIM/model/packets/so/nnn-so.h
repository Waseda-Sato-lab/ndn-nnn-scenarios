/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-so.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-so.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-so.h.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _NNN_SO_HEADER_H_
#define _NNN_SO_HEADER_H_

#include <ns3-dev/ns3/simple-ref-count.h>
#include <ns3-dev/ns3/nstime.h>
#include <ns3-dev/ns3/packet.h>
#include <ns3-dev/ns3/ptr.h>

#include "../../naming/nnn-address.h"

namespace ns3 {

class Packet;

namespace nnn {

/**
 * @ingroup nnn
 * @brief NNN Null packet (wire formats are defined in wire)
 **/
class SO : public SimpleRefCount<SO>
{
public:
	/**
	 * \brief Constructor
	 *
	 * Creates a NULL packet with no payload
	 **/
	SO ();

	/**
	 * \brief Constructor
	 *
	 * Creates a SO packet with payload
	 *
	 * @param name NNN Address Ptr
	 * @param payload Packet Ptr
	 **/
	SO(Ptr<NNNAddress> name, Ptr<Packet> payload);

	/**
	 * \brief Constructor
	 *
	 * Creates a SO packet with payload
	 *
	 * @param name NNN Address
	 * @param payload Packet Ptr
	 **/
	SO(const NNNAddress &name, Ptr<Packet> payload);

	/**
	 * @brief Copy constructor
	 */
	SO (const SO &so_p);

	/**
	 * @brief Return Id of the packet
	 */
	uint32_t
	GetPacketId();

	/**
	 * \brief Set NNN name
	 *
	 * @param name smart pointer to Name
	 *
	 **/
	void
	SetName (Ptr<NNNAddress> name);

	/**
	 * \brief Another variant to set NNN name
	 *
	 * @param name const reference to Name object
	 *
	 **/
	void
	SetName (const NNNAddress &name);

	/**
	 * \brief Get NNN name
	 *
	 * Gets name of the interest.
	 **/
	const NNNAddress&
	GetName () const;

	/**
	 * @brief Get smart pointer to the NNN name (to avoid extra memory usage)
	 */
	Ptr<const NNNAddress>
	GetNamePtr () const;

	/**
	 * @brief Sets the payload of the NULL packet
	 */
	void
	SetPayload (Ptr<Packet> payload);

	/**
	 * @brief Gets the payload of the NULL packet
	 */
	Ptr<const Packet>
	GetPayload () const;

	/**
	 * \brief Set time out for NULL packet
	 * Indicates the (approximate) time remaining before the packet times out.
	 * The timeout is relative to the arrival time of the interest at the current node.
	 * Based heavily on the NDN implementation for Interest Life time
	 * \see http://www.ndn.org/releases/latest/doc/technical/InterestMessage.html for more information.
	 * @param[in] time interest lifetime
	 */
	void
	SetLifetime (Time ttl);

	/**
	 * \brief Get time out value for NULL packet
	 * Indicates the (approximate) time remaining before the packet times out.
	 * The timeout is relative to the arrival time of the interest at the current node.
	 * Based heavily on the NDN implementation for Interest Life time
	 * \see http://www.ndn.org/releases/latest/doc/technical/InterestMessage.html for more information.
	 */
	Time
	GetLifetime () const;

	/**
	 * @brief Get wire formatted packet
	 *
	 * If wire formatted packet has not been set before, 0 will be returned
	 */
	inline Ptr<const Packet>
	GetWire () const;

	/**
	 * @brief Set (cache) wire formatted packet
	 */
	inline void
	SetWire (Ptr<const Packet> packet) const;

	/**
	 * @brief Print SO in plain-text to the specified output stream
	 */
	void
	Print (std::ostream &os) const;

private:
	// NO_ASSIGN
	SO &
	operator = (const SO &other) { return *this; }

private:
	uint32_t m_packetid;      ///< @brief Packet Identifier (1 for SO)
	Time m_ttl;               ///< @brief Packet life time (TTL)
	Ptr<NNNAddress> m_name;   ///< @brief Source NNN Address used in the packet
	Ptr<Packet> m_payload;    ///< @brief Payload

	mutable Ptr<const Packet> m_wire;
};

inline std::ostream &
operator << (std::ostream &os, const SO &i)
{
	i.Print (os);
	return os;
}

inline Ptr<const Packet>
SO::GetWire () const
{
	return m_wire;
}

inline void
SO::SetWire (Ptr<const Packet> packet) const
{
	m_wire = packet;
}

/**
 * @brief Class for Interest parsing exception
 */
class SOException {};

} // namespace nnn
} // namespace ns3

#endif // _NNN_SO_HEADER_H_
