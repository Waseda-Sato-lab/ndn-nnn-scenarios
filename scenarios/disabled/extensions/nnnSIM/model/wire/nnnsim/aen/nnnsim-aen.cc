#include "nnnsim-aen.h"

NNN_NAMESPACE_BEGIN

namespace wire {
namespace nnnSIM {

NS_OBJECT_ENSURE_REGISTERED (AEN);

NS_LOG_COMPONENT_DEFINE ("nnn.wire.nnnSIM.AEN");

AEN::AEN ()
: m_aen_p (Create<nnn::AEN> ())
{
}

AEN::AEN (Ptr<nnn::AEN> aen_p)
: m_aen_p (aen_p)
{
}

Ptr<nnn::AEN>
AEN::GetAEN ()
{
	return m_aen_p;
}

TypeId
AEN::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::nnn::AEN::nnnSIM")
    		.SetGroupName ("Nnn")
    		.SetParent<Header> ()
    		.AddConstructor<AEN> ()
    		;
	return tid;
}

TypeId
AEN::GetInstanceTypeId (void) const
{
	return GetTypeId ();
}

Ptr<Packet>
AEN::ToWire (Ptr<const nnn::AEN> aen_p)
{
	Ptr<const Packet> p = aen_p->GetWire ();
	if (!p)
	{
		// Mechanism packets have no payload, make an empty packet
		Ptr<Packet> packet = Create<Packet> ();
		AEN wireEncoding (ConstCast<nnn::AEN> (aen_p));
		packet->AddHeader (wireEncoding);
		aen_p->SetWire (packet);

		p = packet;
	}
	return p->Copy ();
}

Ptr<nnn::AEN>
AEN::FromWire (Ptr<Packet> packet)
{
	Ptr<nnn::AEN> aen_p = Create<nnn::AEN> ();
	Ptr<Packet> wire = packet->Copy ();

	AEN wireEncoding (aen_p);
	packet->RemoveHeader (wireEncoding);

	// Mechanism packets have no payload, make an empty packet
	aen_p->SetWire (wire);

	return aen_p;
}

uint32_t
AEN::GetSerializedSize (void) const
{
	size_t size =
			4 +                                  /* Packetid */
			2 +                                  /* Length of packet */
			2 +                                  /* Timestamp */
			2 +                                  /* Lease time */
			NnnSim::SerializedSizeName (m_aen_p->GetName ()); /* Name size */


	NS_LOG_INFO ("Serialize size = " << size);
	return size;
}

void
AEN::Serialize (Buffer::Iterator start) const
{
	// Get the total size of the serialized packet
	uint32_t totalsize = GetSerializedSize ();

	// Serialize packetid
	start.WriteU32(m_aen_p->GetPacketId());

	// Get the length of the packet
	start.WriteU16(totalsize - 4); // Minus packetid size of 32 bits

	// Check that the lifetime is within the limits
	NS_ASSERT_MSG (0 <= m_aen_p->GetLifetime ().ToInteger (Time::S) &&
			m_aen_p->GetLifetime ().ToInteger (Time::S) < 65535,
			"Incorrect Lifetime (should not be smaller than 0 and larger than 65535");

	// Round lifetime to seconds
	start.WriteU16 (static_cast<uint16_t> (m_aen_p->GetLifetime ().ToInteger (Time::S)));

	NS_ASSERT_MSG (0 <= m_aen_p->GetLeasetime ().ToInteger (Time::S) &&
			m_aen_p->GetLeasetime ().ToInteger (Time::S) < 65535,
			"Incorrect Lease time (should not be smaller than 0 and larger than 65535");

	// Round lease time to seconds
	start.WriteU16 (static_cast<uint16_t> (m_aen_p->GetLeasetime ().ToInteger (Time::S)));

	// Serialize NNN address
	NnnSim::SerializeName(start, m_aen_p->GetName());
}

uint32_t
AEN::Deserialize (Buffer::Iterator start)
{
	Buffer::Iterator i = start;

	// Read packet id
	if (i.ReadU32 () != 1)
		throw new AENException ();

	// Read length of packet
	uint16_t packet_len = i.ReadU16 ();

	// Read lifetime of the packet
	m_aen_p->SetLifetime (Seconds (i.ReadU16 ()));

	// Read the PoA Type
	m_aen_p->SetLeasetime (Seconds (i.ReadU16 ()));

	// Deserialize the name
	m_aen_p->SetName(NnnSim::DeserializeName(i));

	NS_ASSERT (GetSerializedSize () == (i.GetDistanceFrom (start)));

	return i.GetDistanceFrom (start);
}

void
AEN::Print (std::ostream &os) const
{
	m_aen_p->Print (os);
}

}
}

NNN_NAMESPACE_END
