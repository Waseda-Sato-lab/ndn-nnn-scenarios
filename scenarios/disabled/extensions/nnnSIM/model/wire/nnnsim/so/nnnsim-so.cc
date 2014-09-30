#include "nnnsim-so.h"

NNN_NAMESPACE_BEGIN

namespace wire {
namespace nnnSIM {

NS_OBJECT_ENSURE_REGISTERED (SO);

NS_LOG_COMPONENT_DEFINE ("nnn.wire.nnnSIM.SO");

SO::SO ()
: m_so_p (Create<nnn::SO> ())
{
}

SO::SO (Ptr<nnn::SO> so_p)
: m_so_p (so_p)
{
}

Ptr<nnn::SO>
SO::GetSO ()
{
	return m_so_p;
}

Ptr<Packet>
SO::ToWire (Ptr<const nnn::SO> so_p)
{
	Ptr<const Packet> p = so_p->GetWire ();
	if (!p)
	{
		Ptr<Packet> packet = Create<Packet> (*so_p->GetPayload ());
		SO wireEncoding (ConstCast<nnn::SO> (so_p));
		packet->AddHeader (wireEncoding);
		so_p->SetWire (packet);

		p = packet;
	}

	return p->Copy ();
}

Ptr<nnn::SO>
SO::FromWire (Ptr<Packet> packet)
{
	Ptr<nnn::SO> so_p = Create<nnn::SO> ();
	Ptr<Packet> wire = packet->Copy ();

	SO wireEncoding (so_p);
	packet->RemoveHeader (wireEncoding);

	so_p->SetPayload (packet);
	so_p->SetWire (wire);

	return so_p;
}

TypeId
SO::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::nnn::SO::nnnSIM")
    		.SetGroupName ("Nnn")
    		.SetParent<Header> ()
    		.AddConstructor<SO> ()
    		;
	return tid;
}

TypeId
SO::GetInstanceTypeId (void) const
{
	return GetTypeId ();
}

uint32_t
SO::GetSerializedSize () const
{
	size_t size =
			4 +                                  /* Packetid */
			2 +                                  /* Length of packet */
			2 +                                  /* Timestamp */
			NnnSim::SerializedSizeName (m_so_p->GetName ()) + /* Name size */
			m_so_p->GetPayload ()->GetSerializedSize();    /* Payload data */

	NS_LOG_INFO ("Serialize size = " << size);
	return size;
}

void
SO::Serialize (Buffer::Iterator start) const
{
	// Get the total size of the serialized packet
	uint32_t totalsize = GetSerializedSize ();
	// Find out the payload size
	uint32_t payloadsize = m_so_p->GetPayload ()->GetSerializedSize ();
	// Create a buffer to be able to serialize the payload
	uint8_t buffer[payloadsize];

	// Serialize packetid
	start.WriteU32(m_so_p->GetPacketId());
	// Get the length of the packet
	start.WriteU16(totalsize - 4); // Minus packetid size of 32 bits
	// Check that the lifetime is within the limits
	NS_ASSERT_MSG (0 <= m_so_p->GetLifetime ().ToInteger (Time::S) &&
			m_so_p->GetLifetime ().ToInteger (Time::S) < 65535,
			"Incorrect Lifetime (should not be smaller than 0 and larger than 65535");

	// Round lifetime to seconds
	start.WriteU16 (static_cast<uint16_t> (m_so_p->GetLifetime ().ToInteger (Time::S)));

	// Serialize
	NnnSim::SerializeName(start, m_so_p->GetName());

	// Serialize the payload and place it in the variable buffer
	m_so_p->GetPayload ()->Serialize (buffer, payloadsize);

	for (int i = 0; i < payloadsize; i++)
	{
		start.WriteU8(buffer[i]);
	}
}

uint32_t
SO::Deserialize (Buffer::Iterator start)
{
	Buffer::Iterator i = start;

	// Read packet id
	if (i.ReadU32 () != 1)
		throw new NULLpException ();

	// Read length of packet
	uint16_t packet_len = i.ReadU16 ();

	// Read lifetime of the packet
	m_so_p->SetLifetime (Seconds (i.ReadU16 ()));

	// Update the length. The resulting size is the packet size that
	// we have to deserialize
	packet_len -= 4;

	// Deserialize the name
	m_so_p->SetName(NnnSim::DeserializeName(i));

	// Substract the size of what we just got
	packet_len -= NnnSim::SerializedSizeName (m_so_p->GetName ());

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
	m_so_p->SetPayload (depacket);

	NS_ASSERT (GetSerializedSize () == (i.GetDistanceFrom (start)));

	return i.GetDistanceFrom (start);
}

void
SO::Print (std::ostream &os) const
{
	m_so_p->Print (os);
}

}
}

NNN_NAMESPACE_END
