#include "nnnsim-inf.h"

NNN_NAMESPACE_BEGIN

namespace wire {
namespace nnnSIM {

NS_OBJECT_ENSURE_REGISTERED (INF);

NS_LOG_COMPONENT_DEFINE ("nnn.wire.nnnSIM.INF");

INF::INF ()
: m_inf_p (Create<nnn::INF> ())
{
}

INF::INF (Ptr<nnn::INF> inf_p)
: m_inf_p (inf_p)
{
}

Ptr<nnn::INF>
INF::GetINF()
{
	return m_inf_p;
}

TypeId
INF::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::nnn::INF::nnnSIM")
    		.SetGroupName ("Nnn")
    		.SetParent<Header> ()
    		.AddConstructor<INF> ()
    		;
	return tid;
}

TypeId
INF::GetInstanceTypeId (void) const
{
	return GetTypeId ();
}

Ptr<Packet>
INF::ToWire (Ptr<const nnn::INF> inf_p)
{
	Ptr<const Packet> p = inf_p->GetWire ();
	if (!p)
	{
		// Mechanism packets have no payload, make an empty packet
		Ptr<Packet> packet = Create<Packet> ();
		INF wireEncoding (ConstCast<nnn::INF> (inf_p));
		packet->AddHeader (wireEncoding);
		inf_p->SetWire (packet);

		p = packet;
	}
	return p->Copy ();
}

Ptr<nnn::INF>
INF::FromWire (Ptr<Packet> packet)
{
	Ptr<nnn::INF> inf_p = Create<nnn::INF> ();
	Ptr<Packet> wire = packet->Copy ();

	INF wireEncoding (inf_p);
	packet->RemoveHeader (wireEncoding);

	// Mechanism packets have no payload, make an empty packet
	inf_p->SetWire (wire);

	return inf_p;
}

uint32_t
INF::GetSerializedSize (void) const
{
	size_t size =
			4 +                                  /* Packetid */
			2 +                                  /* Length of packet */
			2 +                                  /* Timestamp */
			2 +                                  /* re Lease time */
			NnnSim::SerializedSizeName (m_inf_p->GetOldName ()) + /* Name size */
			NnnSim::SerializedSizeName (m_inf_p->GetNewName ()); /* Name size */

	NS_LOG_INFO ("Serialize size = " << size);
	return size;
}

void
INF::Serialize (Buffer::Iterator start) const
{
	// Get the total size of the serialized packet
	uint32_t totalsize = GetSerializedSize ();

	// Serialize packetid
	start.WriteU32(m_inf_p->GetPacketId());

	// Get the length of the packet
	start.WriteU16(totalsize - 4); // Minus packetid size of 32 bits

	// Check that the lifetime is within the limits
	NS_ASSERT_MSG (0 <= m_inf_p->GetLifetime ().ToInteger (Time::S) &&
			m_inf_p->GetLifetime ().ToInteger (Time::S) < 65535,
			"Incorrect Lifetime (should not be smaller than 0 and larger than 65535");

	// Round lifetime to seconds
	start.WriteU16 (static_cast<uint16_t> (m_inf_p->GetLifetime ().ToInteger (Time::S)));

	NS_ASSERT_MSG (0 <= m_inf_p->GetRemainLease ().ToInteger (Time::S) &&
			m_inf_p->GetRemainLease ().ToInteger (Time::S) < 65535,
			"Incorrect Lease time (should not be smaller than 0 and larger than 65535");

	// Round lease time to seconds
	start.WriteU16 (static_cast<uint16_t> (m_inf_p->GetRemainLease ().ToInteger (Time::S)));

	// Serialize NNN address
	NnnSim::SerializeName(start, m_inf_p->GetOldName());

	// Serialize NNN address
	NnnSim::SerializeName(start, m_inf_p->GetNewName());
}

uint32_t
INF::Deserialize (Buffer::Iterator start)
{
	Buffer::Iterator i = start;

	// Read packet id
	if (i.ReadU32 () != 1)
		throw new INFException ();

	// Read length of packet
	uint16_t packet_len = i.ReadU16 ();

	// Read lifetime of the packet
	m_inf_p->SetLifetime (Seconds (i.ReadU16 ()));

	// Read the PoA Type
	m_inf_p->SetRemainLease (Seconds (i.ReadU16 ()));

	// Deserialize the old name
	m_inf_p->SetOldName(NnnSim::DeserializeName(i));

	// Deserialize the new name
	m_inf_p->SetNewName(NnnSim::DeserializeName(i));

	NS_ASSERT (GetSerializedSize () == (i.GetDistanceFrom (start)));

	return i.GetDistanceFrom (start);
}

void
INF::Print (std::ostream &os) const
{
	m_inf_p->Print (os);
}

}
}

NNN_NAMESPACE_END
