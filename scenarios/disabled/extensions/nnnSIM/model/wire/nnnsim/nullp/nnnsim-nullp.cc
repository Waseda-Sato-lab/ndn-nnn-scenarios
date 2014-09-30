#include "nnnsim-nullp.h"

NNN_NAMESPACE_BEGIN

namespace wire {
namespace nnnSIM {

NS_OBJECT_ENSURE_REGISTERED (NULLp);

NS_LOG_COMPONENT_DEFINE ("nnn.wire.nnnSIM.NULLp");

NULLp::NULLp ()
: m_null_p (Create<nnn::NULLp> ())
{
}

NULLp::NULLp (Ptr<nnn::NULLp> null_p)
: m_null_p (null_p)
{
}

Ptr<nnn::NULLp>
NULLp::GetNULLp ()
{
	return m_null_p;
}

TypeId
NULLp::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::nnn::NULLp::nnnSIM")
    		.SetGroupName ("Nnn")
    		.SetParent<Header> ()
    		.AddConstructor<NULLp> ()
    		;
	return tid;
}

TypeId
NULLp::GetInstanceTypeId (void) const
{
	return GetTypeId ();
}

Ptr<Packet>
NULLp::ToWire (Ptr<const nnn::NULLp> null_p)
{
	Ptr<const Packet> p = null_p->GetWire ();
	if (!p)
	{
		Ptr<Packet> packet = Create<Packet> (*null_p->GetPayload ());
		NULLp wireEncoding (ConstCast<nnn::NULLp> (null_p));
		packet->AddHeader (wireEncoding);
		null_p->SetWire (packet);

		p = packet;
	}

	return p->Copy ();
}

Ptr<nnn::NULLp>
NULLp::FromWire (Ptr<Packet> packet)
{
	Ptr<nnn::NULLp> null_p = Create<nnn::NULLp> ();
	Ptr<Packet> wire = packet->Copy ();

	NULLp wireEncoding (null_p);
	packet->RemoveHeader (wireEncoding);

	null_p->SetPayload (packet);
	null_p->SetWire (wire);

	return null_p;
}

uint32_t
NULLp::GetSerializedSize (void) const
{
	size_t size =
			4 +                                  /* Packetid */
			2 +                                  /* Length of packet */
			2 +                                  /* Timestamp */
			m_null_p->GetPayload ()->GetSerializedSize();  /* Payload data */

	NS_LOG_INFO ("Serialize size = " << size);
	return size;
}

void
NULLp::Serialize (Buffer::Iterator start) const
{
	// Get the total size of the serialized packet
	uint32_t totalsize = GetSerializedSize ();
	// Find out the payload size
	uint32_t payloadsize = m_null_p->GetPayload ()->GetSerializedSize ();
	// Create a buffer to be able to serialize the payload
	uint8_t buffer[payloadsize];

	// Serialize packetid
	start.WriteU32(m_null_p->GetPacketId());
	// Get the length of the packet
	start.WriteU16(totalsize - 4); // Minus packetid size of 32 bits

	// Check that the lifetime is within the limits
	NS_ASSERT_MSG (0 <= m_null_p->GetLifetime ().ToInteger (Time::S) &&
			m_null_p->GetLifetime ().ToInteger (Time::S) < 65535,
			"Incorrect Lifetime (should not be smaller than 0 and larger than 65535");

	// Round lifetime to seconds
	start.WriteU16 (static_cast<uint16_t> (m_null_p->GetLifetime ().ToInteger (Time::S)));

	// Serialize the payload and place it in the variable buffer
	m_null_p->GetPayload ()->Serialize (buffer, payloadsize);

	for (int i = 0; i < payloadsize; i++)
	{
		start.WriteU8(buffer[i]);
	}
}

uint32_t
NULLp::Deserialize (Buffer::Iterator start)
{
	Buffer::Iterator i = start;

	// Read packet id
	if (i.ReadU32 () != 0)
		throw new NULLpException ();

	// Read length of packet
	uint16_t packet_len = i.ReadU16 ();

	// Read lifetime of the packet
	m_null_p->SetLifetime (Seconds (i.ReadU16 ()));

	// Update the length. The resulting size is the packet size that
	// we have to deserialize
	packet_len -= 4;

	// Obtain how big the buffer has to be (dividing by 8)
	uint32_t bufsize = packet_len / 8;

	// Create the buffer size
	uint8_t buffer[bufsize];

	// Read 8 bits at a time until the end of the packet
	for (int j = 0; j < bufsize; j++)
	{
		buffer[j] = i.ReadU8 ();
	}

	// Create a packet from the deserialized information
	Ptr<Packet> depacket = Create <Packet> (buffer, packet_len, true);

	// Save the packet information
	m_null_p->SetPayload (depacket);

	NS_ASSERT (GetSerializedSize () == (i.GetDistanceFrom (start)));

	return i.GetDistanceFrom (start);
}

void
NULLp::Print (std::ostream &os) const
{
	m_null_p->Print (os);
}

}
}

NNN_NAMESPACE_END
