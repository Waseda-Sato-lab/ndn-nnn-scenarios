/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Zhu Li <phillipszhuli1990@gmail.com>
 *           Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-face.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-face.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-face.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ns3-dev/ns3/assert.h>
#include <ns3-dev/ns3/boolean.h>
#include <ns3-dev/ns3/double.h>
#include <ns3-dev/ns3/log.h>
#include <ns3-dev/ns3/node.h>
#include <ns3-dev/ns3/packet.h>
#include <ns3-dev/ns3/uinteger.h>


#include <ns3-dev/ns3/simulator.h>
#include <ns3-dev/ns3/random-variable.h>
#include <ns3-dev/ns3/pointer.h>

#include "nnn-face.h"
#include "../helper/nnn-header-helper.h"
#include "wire/nnn-wire.h"

#include <boost/ref.hpp>

NS_LOG_COMPONENT_DEFINE ("nnn.Face");

namespace ns3 {
namespace nnn {

NS_OBJECT_ENSURE_REGISTERED (Face);

TypeId
Face::GetTypeId ()
{
	static TypeId tid = TypeId ("ns3::nnn::Face")
    		.SetParent<Object> ()
    		.SetGroupName ("Nnn")
    		.AddAttribute ("Id", "Face id (unique integer for the Nnn stack on this node)",
    				TypeId::ATTR_GET, // allow only getting it.
    				UintegerValue (0),
    				MakeUintegerAccessor (&Face::m_id),
    				MakeUintegerChecker<uint32_t> ())
    				;
	return tid;
}

/**
 * By default, Nnn face are created in the "down" state
 *  with no MAC addresses.  Before becoming useable, the user must
 * invoke SetUp on them once an Nnn address and mask have been set.
 */
Face::Face (Ptr<Node> node)
: m_node (node)
, m_upstreamNULLpHandler (MakeNullCallback< void, Ptr<Face>, Ptr<NULLp> > ())
, m_upstreamSOHandler (MakeNullCallback< void, Ptr<Face>, Ptr<SO> > ())
, m_upstreamDOHandler (MakeNullCallback< void, Ptr<Face>, Ptr<DO> > ())
, m_ifup (false)
, m_id ((uint32_t)-1)
, m_metric (0)
, m_flags (0)
{
	NS_LOG_FUNCTION (this << node);

	NS_ASSERT_MSG (node != 0, "node cannot be NULL. Check the code");
}

Face::~Face ()
{
	NS_LOG_FUNCTION_NOARGS ();
}

Face::Face (const Face &)
{
}

Face& Face::operator= (const Face &)
{
	return *this;
}

Ptr<Node>
Face::GetNode () const
{
	return m_node;
}

void
Face::RegisterProtocolHandlers (const NULLpHandler &NULLpHandler, const SOHandler &SOHandler,
		const DOHandler &DOHandler, const ENHandler &ENHandler,
		const AENHandler &AENHandler, const RENHandler &RENHandler,
		const INFHandler &INFHandler)
{
	NS_LOG_FUNCTION_NOARGS ();

	m_upstreamNULLpHandler = NULLpHandler;
	m_upstreamSOHandler = SOHandler;
	m_upstreamDOHandler = DOHandler;
	m_upstreamENHandler = ENHandler;
	m_upstreamAENHandler = AENHandler;
	m_upstreamRENHandler = RENHandler;
	m_upstreamINFHandler = INFHandler;
}

void
Face::UnRegisterProtocolHandlers ()
{
	NS_LOG_FUNCTION_NOARGS ();

	m_upstreamNULLpHandler = MakeNullCallback< void, Ptr<Face>, Ptr<NULLp> > ();
	m_upstreamSOHandler = MakeNullCallback< void, Ptr<Face>, Ptr<SO> > ();
	m_upstreamDOHandler = MakeNullCallback< void, Ptr<Face>, Ptr<DO> > ();
}

bool
Face::SendNULLp (Ptr<const NULLp> n_o)
{
	NS_LOG_FUNCTION (this << n_o);

	if (!IsUp ())
	{
		return false;
	}

	return Send (Wire::FromNULLp (n_o));
}


bool
Face::SendSO (Ptr<const SO> so_p)
{
	NS_LOG_FUNCTION (this << boost::cref (*this) << so_p->GetName ());

	if (!IsUp ())
	{
		return false;
	}

	return Send (Wire::FromSO (so_p));
}

bool
Face::SendDO (Ptr<const DO> do_p)
{
	NS_LOG_FUNCTION (this << boost::cref (*this) << do_p->GetName ());

	if (!IsUp ())
	{
		return false;
	}

	return Send (Wire::FromDO (do_p));
}



bool
Face::Send (Ptr<Packet> packet)
{
	return true;
}

bool
Face::Receive (Ptr<const Packet> p)
{
	NS_LOG_FUNCTION (this << p << p->GetSize ());

	if (!IsUp ())
	{
		// no tracing here. If we were off while receiving, we shouldn't even know that something was there
		return false;
	}

	Ptr<Packet> packet = p->Copy (); // give upper layers a rw copy of the packet
	try
	{
		HeaderHelper::Type type = HeaderHelper::GetNnnHeaderType (packet);
		switch (type)
		{
		case HeaderHelper::NULL_NNN:
			return ReceiveNULLp (Wire::ToNULLp (packet, Wire::WIRE_FORMAT_NNNSIM));
		case HeaderHelper::SO_NNN:
			return ReceiveSO (Wire::ToSO (packet, Wire::WIRE_FORMAT_NNNSIM));
		case HeaderHelper::DO_NNN:
			return ReceiveDO (Wire::ToDO (packet, Wire::WIRE_FORMAT_NNNSIM));
		default:
			NS_FATAL_ERROR ("Not supported NNN header");
			return false;
		}

		// exception will be thrown if packet is not recognized
	}
	catch (UnknownHeaderException)
	{
		NS_FATAL_ERROR ("Unknown NNN header. Should not happen");
		return false;
	}

	return false;
}

bool
Face::ReceiveNULLp (Ptr<NULLp> n_o)
{
	if (!IsUp ())
	{
		// no tracing here. If we were off while receiving, we shouldn't even know that something was there
		return false;
	}

	m_upstreamNULLpHandler (this, n_o);
	return true;
}

bool
Face::ReceiveSO (Ptr<SO> so_p)
{
	if (!IsUp ())
	{
		// no tracing here. If we were off while receiving, we shouldn't even know that something was there
		return false;
	}

	m_upstreamSOHandler (this, so_p);
	return true;
}

bool
Face::ReceiveDO (Ptr<DO> do_p)
{
	if (!IsUp ())
	{
		// no tracing here. If we were off while receiving, we shouldn't even know that something was there
		return false;
	}

	m_upstreamDOHandler (this, do_p);
	return true;
}

void
Face::SetMetric (uint16_t metric)
{
	NS_LOG_FUNCTION (metric);
	m_metric = metric;
}

uint16_t
Face::GetMetric (void) const
{
	return m_metric;
}

void
Face::SetFlags (uint32_t flags)
{
	m_flags = flags;
}

bool
Face::operator== (const Face &face) const
{
	NS_ASSERT_MSG (m_node->GetId () == face.m_node->GetId (),
			"NN_Faces of different nodes should not be compared to each other: " << *this << " == " << face);

	return (m_id == face.m_id);
}

bool
Face::operator< (const Face &face) const
{
	NS_ASSERT_MSG (m_node->GetId () == face.m_node->GetId (),
			"NN_Faces of different nodes should not be compared to each other: " << *this << " == " << face);

	return (m_id < face.m_id);
}

std::ostream&
Face::Print (std::ostream &os) const
{
	os << "id=" << GetId ();
	return os;
}

std::ostream&
operator<< (std::ostream& os, const Face &face)
{
	face.Print (os);
	return os;
}

} // namespace nnn
} // namespace ns3

