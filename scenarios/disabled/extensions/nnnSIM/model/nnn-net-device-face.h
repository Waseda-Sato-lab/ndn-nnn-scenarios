/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *	         Zhu Li <philipszhuli1990@ruri.waseda.jp>
 *
 *  nnn-net-device-face.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-net-device-face.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-net-device-face.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NNN_NET_DEVICE_FACE_H
#define NNN_NET_DEVICE_FACE_H

#include <ns3-dev/ns3/net-device.h>

#include "nnn-face.h"

namespace ns3 {
namespace nnn {

/**
 * \ingroup Nnn-face
 * \brief Implementation of layer-2 (Ethernet) Nnn face
 *
 * This class defines basic functionality of Nnn face. Face is core
 * component responsible for actual delivery of data packet to and
 * from Nnn stack
 *
 * NnnNetDevice face is permanently associated with one NetDevice
 * object and this object cannot be changed for the lifetime of the
 * face
 *
 * \see NnnAppFace, NnnNetDeviceFace, NnnIpv4Face, NnnUdpFace
 */
class NetDeviceFace  : public Face
{
public:
	static TypeId
	GetTypeId ();

	/**
	 * \brief Constructor
	 *
	 * @param node Node associated with the face
	 * @param netDevice a smart pointer to NetDevice object to which
	 * this face will be associate
	 */
	NetDeviceFace (Ptr<Node> node, const Ptr<NetDevice> &netDevice);
	virtual ~NetDeviceFace();

	////////////////////////////////////////////////////////////////////
	// methods overloaded from NnnFace
	virtual void
	RegisterProtocolHandlers (const NULLpHandler &NULLpHandler, const SOHandler &SOHandler,
			const DOHandler &DOHandler, const ENHandler &ENHandler,
			const AENHandler &AENHandler, const RENHandler &RENHandler,
			const INFHandler &INFHandler);

	virtual void
	UnRegisterProtocolHandlers ();

protected:
	virtual bool
	Send (Ptr<Packet> p);

public:
	/**
	 * @brief Print out name of the NnnFace to the stream
	 */
	virtual std::ostream&
	Print (std::ostream &os) const;
	////////////////////////////////////////////////////////////////////

	/**
	 * \brief Get NetDevice associated with the face
	 *
	 * \returns smart pointer to NetDevice associated with the face
	 */
	Ptr<NetDevice> GetNetDevice () const;

private:
	NetDeviceFace (const NetDeviceFace &); ///< \brief Disabled copy constructor
	NetDeviceFace& operator= (const NetDeviceFace &); ///< \brief Disabled copy operator

	/// \brief callback from lower layers
	void ReceiveFromNetDevice (Ptr<NetDevice> device,
			Ptr<const Packet> p,
			uint16_t protocol,
			const Address &from,
			const Address &to,
			NetDevice::PacketType packetType);

private:
	Ptr<NetDevice> m_netDevice; ///< \brief Smart pointer to NetDevice
};

} // namespace nnn
} // namespace ns3

#endif //NNN_NET_DEVICE_FACE_H
