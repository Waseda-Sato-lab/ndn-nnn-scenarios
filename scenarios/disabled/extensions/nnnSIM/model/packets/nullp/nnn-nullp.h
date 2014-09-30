/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-nullp.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-nullp.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-nullp.h.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _NNN_NULLP_HEADER_H_
#define _NNN_NULLP_HEADER_H_

#include <ns3-dev/ns3/simple-ref-count.h>
#include <ns3-dev/ns3/nstime.h>
#include <ns3-dev/ns3/packet.h>
#include <ns3-dev/ns3/ptr.h>

namespace ns3 {

class Packet;

namespace nnn {

/**
 * @ingroup nnn
 * @brief NNN Null packet (wire formats are defined in wire)
 **/
class NULLp : public SimpleRefCount<NULLp>
{
public:
	/**
	 * \brief Constructor
	 *
	 * Creates a NULL packet with no payload
	 **/
	NULLp ();

	/**
	 * \brief Constructor
	 *
	 * Creates a NULL packet with payload
	 **/
	NULLp(Ptr<Packet> payload);

	/**
	 * @brief Copy constructor
	 */
	NULLp (const NULLp &nullp);

	/**
	 * @brief Return Id of the packet
	 */
	uint32_t
	GetPacketId();

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
	 * @brief Print NULLp in plain-text to the specified output stream
	 */
	void
	Print (std::ostream &os) const;

private:
	// NO_ASSIGN
	NULLp &
	operator = (const NULLp &other) { return *this; }

private:
	uint32_t m_packetid;      ///< @brief Packet Identifier (0 for NULLp)
	Time m_ttl;               ///< @brief Packet life time (TTL)
	Ptr<Packet> m_payload;    ///< @brief Payload

	mutable Ptr<const Packet> m_wire;
};

inline std::ostream &
operator << (std::ostream &os, const NULLp &i)
{
	i.Print (os);
	return os;
}

inline Ptr<const Packet>
NULLp::GetWire () const
{
	return m_wire;
}

inline void
NULLp::SetWire (Ptr<const Packet> packet) const
{
	m_wire = packet;
}

/**
 * @brief Class for Interest parsing exception
 */
class NULLpException {};

} // namespace nnn
} // namespace ns3

#endif // _NNN_NULLP_HEADER_H_
