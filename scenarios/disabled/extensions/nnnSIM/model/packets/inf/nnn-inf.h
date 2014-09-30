/* -*- Mode: C++; c-file-style: "gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *	         Zhu Li <philipszhuli1990@ruri.waseda.jp>
 *
 *  nnn-inf.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-inf.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-inf.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _NNN_INF_HEADER_H_
#define _NNN_INF_HEADER_H_

#include <ns3-dev/ns3/nstime.h>
#include <ns3-dev/ns3/packet.h>
#include <ns3-dev/ns3/ptr.h>
#include <ns3-dev/ns3/simple-ref-count.h>

#include "../../naming/nnn-address.h"

namespace ns3 {

class Packet;

namespace nnn {

/**
 * @ingroup nnn
 * @brief NNN INF packet (wire formats are defined in wire)
 **/
class INF : public SimpleRefCount<INF>
{
public:
	/**
	 * \brief Constructor
	 *
	 * Creates a INF packet
	 **/
	INF ();

	/**
	 * \brief Constructor
	 *
	 *
	 * @param oldname Old NNN Address
	 * @param newname New NNN Address
	 **/
	INF(Ptr<NNNAddress> oldname, Ptr<NNNAddress> newname);

	/**
	 * \brief Constructor
	 *
	 * Creates a INF packet with payload
	 *
	 * @param name NNN Address
	 * @param payload Packet Ptr
	 **/
	INF(const NNNAddress &oldname, const NNNAddress &newname);

	/**
	 * @brief Copy constructor
	 */
	INF (const INF &inf_p);

	/**
	 * \brief Return Id of the packet
	 *
	 **/
	uint32_t
	GetPacketId ();
	
	/**
	 * \brief Set Old NNN Address name
	 *
	 * @param name smart pointer to NNNAddress
	 *
	 **/
	void
	SetOldName (Ptr<NNNAddress> name);

	/**
	 * \brief Another variant to set old NNN Address
	 *
	 * @param name const reference to NNNAddress object
	 *
	 **/
	void
	SetOldName (const NNNAddress &name);

	/**
	 * \brief Get interest name
	 *
	 * Gets name of the interest.
	 **/
	const NNNAddress&
	GetOldName () const;

	/**
	 * @brief Get smart pointer to the interest name (to avoid extra memory usage)
	 */
	Ptr<const NNNAddress>
	GetOldNamePtr () const;

	/**
	 * \brief Set Old NNN Address name
	 *
	 * @param name smart pointer to NNNAddress
	 *
	 **/
	void
	SetNewName (Ptr<NNNAddress> name);

	/**
	 * \brief Another variant to set old NNN Address
	 *
	 * @param name const reference to NNNAddress object
	 *
	 **/
	void
	SetNewName (const NNNAddress &name);

	/**
	 * \brief Get interest name
	 *
	 * Gets name of the interest.
	 **/
	const NNNAddress&
	GetNewName () const;

	/**
	 * @brief Get smart pointer to the interest name (to avoid extra memory usage)
	 */
	Ptr<const NNNAddress>
	GetNewNamePtr () const;

	/**
	 * \brief Set time out for INF packet
	 * Indicates the (approximate) time remaining before the packet times out.
	 * The timeout is relative to the arrival time of the interest at the current node.
	 * Based heavily on the NDN implementation for Interest Life time
	 * \see http://www.ndn.org/releases/latest/doc/technical/InterestMessage.html for more information.
	 * @param[in] time interest lifetime
	 */
	void
	SetLifetime (Time ttl);

	/**
	 * \brief Get time out value for INF packet
	 * Indicates the (approximate) time remaining before the packet times out.
	 * The timeout is relative to the arrival time of the interest at the current node.
	 * Based heavily on the NDN implementation for Interest Life time
	 * \see http://www.ndn.org/releases/latest/doc/technical/InterestMessage.html for more information.
	 */
	Time
	GetLifetime () const;

	void
	SetRemainLease (Time ex_lease);

	Time
	GetRemainLease () const;

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
	 * @brief Print INF in plain-text to the specified output stream
	 */
	void
	Print (std::ostream &os) const;

private:
	// NO_ASSIGN
	INF &
	operator = (const INF &other) { return *this; }

private:
	uint32_t m_packetid;         ///< @brief Packet Identifier (6 for INF)
	Time m_ttl;                  ///< @brief Packet life time (TTL)
	Ptr<NNNAddress> m_old_name;  ///< @brief Old NNN Address used in the packet
	Ptr<NNNAddress> m_new_name;  ///< @brief New NNN Address used in the packet
	Time m_re_lease;             ///< @brief Packet Remaining lease time

	mutable Ptr<const Packet> m_wire;
};

inline std::ostream &
operator << (std::ostream &os, const INF &i)
{
	i.Print (os);
	return os;
}

inline Ptr<const Packet>
INF::GetWire () const
{
	return m_wire;
}

inline void
INF::SetWire (Ptr<const Packet> packet) const
{
	m_wire = packet;
}

/**
 * @brief Class for Interest parsing exception
 */
class INFException {};

} // namespace nnn
} // namespace ns3

#endif // _NNN_INF_HEADER_H_
