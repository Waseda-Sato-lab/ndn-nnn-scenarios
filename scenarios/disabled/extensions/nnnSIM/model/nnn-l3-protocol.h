/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Zhu Li <phillipszhuli1990@gmail.com>
 *           Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-l3-protocol.h is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-l3-protocol.h is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-l3-protocol.h.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NNN_L3_PROTOCOL_H
#define NNN_L3_PROTOCOL_H

#include <list>
#include <vector>

#include <ns3-dev/ns3/ptr.h>
#include <ns3-dev/ns3/net-device.h>
#include <ns3-dev/ns3/nstime.h>

namespace ns3 {

class Packet;
class NetDevice;
class Node;
class Header;

namespace nnn {

class Face;
class NNNForwardingStrategy;

/**
 * \defgroup nnn nnnSIM: nnn simulation module
 *
 * This is a modular implementation of NNN protocol for NS-3
 */
/**
 * \ingroup nnn
 * \brief Implementation network-layer of NNN stack
 *
 * This class defines the API to manipulate the following aspects of
 * the NNN stack implementation:
 * -# register a face (Face-derived object) for use by the NNN
 *    layer
 * 
 * Each Face-derived object has conceptually a single NNN
 * interface associated with it.
 *
 * In addition, this class defines NNN packet coding constants
 *
 * \see Face, NNNForwardingStrategy
 */
class NNNL3Protocol :
		public Object
{
public:
	typedef std::vector<Ptr<Face> > FaceList;

	/**
	 * \brief Interface ID
	 *
	 * \return interface ID
	 */
	static TypeId GetTypeId ();

	static const uint16_t ETHERNET_FRAME_TYPE; ///< @brief Ethernet Frame Type of Nnn
	//static const uint16_t IP_STACK_PORT;       ///< @brief TCP/UDP port for NNN stack
	// static const uint16_t IP_PROTOCOL_TYPE;    ///< \brief IP protocol type of NNN

	/**
	 * \brief Default constructor. Creates an empty stack without forwarding strategy set
	 */
	NNNL3Protocol ();
	virtual ~NNNL3Protocol  ();

	/**
	 * \brief Add face to Nnn stack
	 *
	 * \param face smart pointer to NnnFace-derived object
	 * (NnnLocalFace, NnnNetDeviceFace, NnnUdpFace) \returns the
	 * index of the Nnn interface added.
	 *
	 * \see NnnLocalFace, NnnNetDeviceFace, NnnUdpFace
	 */
	virtual uint32_t
	AddFace (const Ptr<Face> &face);

	/**
	 * \brief Get current number of faces added to Nnn stack
	 *
	 * \returns the number of faces
	 */
	virtual uint32_t
	GetNFaces () const;

	/**
	 * \brief Get face by face index
	 * \param face The face number (number in face list)
	 * \returns The NnnFace associated with the Nnn face number.
	 */
	virtual Ptr<Face>
	GetFace (uint32_t face) const;

	/**
	 * \brief Get face by face ID
	 * \param face The face ID number
	 * \returns The NnnFace associated with the Nnn face number.
	 */
	virtual Ptr<Face>
	GetFaceById (uint32_t face) const;

	/**
	 * \brief Remove face from Nnn stack (remove callbacks)
	 */
	virtual void
	RemoveFace (Ptr<Face> face);

	/**
	 * \brief Get face for NetDevice
	 */
	virtual Ptr<Face>
	GetFaceByNetDevice (Ptr<NetDevice> netDevice) const;

protected:
	virtual void DoDispose (void); ///< @brief Do cleanup

	/**
	 * This function will notify other components connected to the node that a new stack member is now connected
	 * This will be used to notify Layer 3 protocol of layer 4 protocol stack to connect them together.
	 */
	virtual void NotifyNewAggregate ();

private:
	NNNL3Protocol (const NNNL3Protocol  &); ///< copy constructor is disabled
	NNNL3Protocol  &operator = (const NNNL3Protocol  &); ///< copy operator is disabled

private:
	uint32_t m_faceCounter; ///< \brief counter of faces. Increased every time a new face is added to the stack
	FaceList m_faces; ///< \brief list of faces that belongs to nnn stack on this node

	// These objects are aggregated, but for optimization, get them here
	Ptr<Node> m_node; ///< \brief node on which nnn stack is installed
	Ptr<NNNForwardingStrategy> m_NNNForwardingStrategy; ///< \brief smart pointer to the selected forwarding strategy
};

} // namespace nnn
} // namespace ns3

#endif /* NNN_L3_PROTOCOL_H */
