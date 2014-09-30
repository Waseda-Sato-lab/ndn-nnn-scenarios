/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Zhu Li <phillipszhuli1990@gmail.com>
 *           Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-l3-protocol.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-l3-protocol.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-l3-protocol.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ns3-dev/ns3/callback.h>
#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/node.h>
#include <ns3-dev/ns3/object-vector.h>
#include <ns3-dev/ns3/packet.h>
#include <ns3-dev/ns3/pointer.h>
#include <ns3-dev/ns3/random-variable.h>
#include <ns3-dev/ns3/simulator.h>
#include <ns3-dev/ns3/trace-source-accessor.h>
#include <ns3-dev/ns3/uinteger.h>

#include "nnn-l3-protocol.h"

#include "nnn-nnst.h"
#include "nnn-so.h"
#include "nnn-do.h"
#include "nnn-en.h"
#include "nnn-aen.h"
#include "nnn-ren.h"
#include "nnn-inf.h"

#include "nnn-face.h"
#include "nnn-forwarding-strategy.h"
#include "nnn-net-device-face.h"

#include <boost/foreach.hpp>

NS_LOG_COMPONENT_DEFINE ("nnn.L3Protocol");

namespace ns3 {
namespace nnn {

const uint16_t NNNL3Protocol::ETHERNET_FRAME_TYPE = 0x7786;
//const uint16_t L3Protocol::IP_STACK_PORT = 9695;

NS_OBJECT_ENSURE_REGISTERED (NNNL3Protocol);

TypeId
NNNL3Protocol::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::nnn::NNNL3Protocol")
    		.SetGroupName ("nnn")
    		.SetParent<Object> ()
    		.AddConstructor<NNNL3Protocol> ()
    		.AddAttribute ("FaceList", "List of faces associated with nnn stack",
    				ObjectVectorValue (),
    				MakeObjectVectorAccessor (&NNNL3Protocol::m_faces),
    				MakeObjectVectorChecker<Face> ())
    				;
	return tid;
}

NNNL3Protocol::NNNL3Protocol ()
: m_faceCounter (0)
{
	NS_LOG_FUNCTION (this);
}

NNNL3Protocol::~NNNL3Protocol ()
{
	NS_LOG_FUNCTION (this);
}

/*
 * This method is called by AddAgregate and completes the aggregation
 * by setting the node in the ndn stack
 */
void
NNNL3Protocol::NotifyNewAggregate ()
{
	// not really efficient, but this will work only once
	if (m_node == 0)
	{
		m_node = GetObject<Node> ();
		if (m_node != 0)
		{
			NS_ASSERT_MSG (m_NNNForwardingStrategy != 0,
					"Forwarding strategy should be aggregated before NNNL3Protocol");
		}
	}
	if (m_NNNForwardingStrategy == 0)
	{
		m_NNNForwardingStrategy = GetObject<ForwardingStrategy> ();
	}

	Object::NotifyNewAggregate ();
}

void
NNNL3Protocol::DoDispose (void)
{
	NS_LOG_FUNCTION (this);

	// for (FaceList::iterator i = m_faces.begin (); i != m_faces.end (); ++i)
	//   {
	//     *i = 0;
	//   }
	m_faces.clear ();
	m_node = 0;

	// Force delete on objects
	m_NNNForwardingStrategy = 0; // there is a reference to PIT stored in here

	Object::DoDispose ();
}

uint32_t
NNNL3Protocol::AddFace (const Ptr<Face> &face)
{
	NS_LOG_FUNCTION (this << &face);

	face->SetId (m_faceCounter); // sets a unique ID of the face. This ID serves only informational purposes

	// ask face to register in lower-layer stack
	face->RegisterProtocolHandlers (MakeCallback (&ForwardingStrategy::OnSO, m_NNNForwardingStrategy),
			MakeCallback (&ForwardingStrategy::OnDO, m_NNNForwardingStrategy));

	m_faces.push_back (face);
	m_faceCounter++;

	m_NNNForwardingStrategy->AddFace (face); // notify that face is added
	return face->GetId ();
}

void
NNNL3Protocol::RemoveFace (Ptr<Face> face)
{
	NS_LOG_FUNCTION (this << boost::cref (*face));
	// ask face to register in lower-layer stack
	face->UnRegisterProtocolHandlers ();
	Ptr<Pit> pit = GetObject<Pit> ();

	// just to be on a safe side. Do the process in two steps
	std::list< Ptr<pit::Entry> > entriesToRemoves;
	for (Ptr<pit::Entry> pitEntry = pit->Begin (); pitEntry != 0; pitEntry = pit->Next (pitEntry))
	{
		pitEntry->RemoveAllReferencesToFace (face);

		// If this face is the only for the associated FIB entry, then FIB entry will be removed soon.
		// Thus, we have to remove the whole PIT entry
		if (pitEntry->GetFibEntry ()->m_faces.size () == 1 &&
				pitEntry->GetFibEntry ()->m_faces.begin ()->GetFace () == face)
		{
			entriesToRemoves.push_back (pitEntry);
		}
	}
	BOOST_FOREACH (Ptr<pit::Entry> removedEntry, entriesToRemoves)
	{
		pit->MarkErased (removedEntry);
	}

	FaceList::iterator face_it = find (m_faces.begin(), m_faces.end(), face);
	if (face_it == m_faces.end ())
	{
		return;
	}
	m_faces.erase (face_it);

	GetObject<Fib> ()->RemoveFromAll (face);
	m_NNNForwardingStrategy->RemoveFace (face); // notify that face is removed
}

Ptr<Face>
NNNL3Protocol::GetFace (uint32_t index) const
{
	NS_ASSERT (0 <= index && index < m_faces.size ());
	return m_faces[index];
}

Ptr<Face>
NNNL3Protocol::GetFaceById (uint32_t index) const
{
	BOOST_FOREACH (const Ptr<Face> &face, m_faces) // this function is not supposed to be called often, so linear search is fine
    		{
		if (face->GetId () == index)
			return face;
    		}
	return 0;
}

Ptr<Face>
NNNL3Protocol::GetFaceByNetDevice (Ptr<NetDevice> netDevice) const
{
	BOOST_FOREACH (const Ptr<Face> &face, m_faces) // this function is not supposed to be called often, so linear search is fine
    		{
		Ptr<NetDeviceFace> netDeviceFace = DynamicCast<NetDeviceFace> (face);
		if (netDeviceFace == 0) continue;

		if (netDeviceFace->GetNetDevice () == netDevice)
			return face;
    		}
	return 0;
}

uint32_t
NNNL3Protocol::GetNFaces (void) const
{
	return m_faces.size ();
}

} //namespace ndn
} //namespace ns3
