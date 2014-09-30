#include "nnnsim-do.h"

NNN_NAMESPACE_BEGIN

namespace wire {
namespace nnnSIM {

NS_OBJECT_ENSURE_REGISTERED (DO);

NS_LOG_COMPONENT_DEFINE ("nnn.wire.nnnSIM.DO");

DO::DO ()
: m_do_p (Create<nnn::DO> ())
{
}

DO::DO (Ptr<nnn::DO> do_p)
: m_do_p (do_p)
{
}

Ptr<nnn::DO>
DO::GetDO ()
{
	return m_do_p;
}

TypeId
DO::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::nnn::DO::nnnSIM")
    		.SetGroupName ("Nnn")
    		.SetParent<Header> ()
    		.AddConstructor<DO> ()
    		;
	return tid;
}

TypeId
DO::GetInstanceTypeId (void) const
{
	return GetTypeId ();
}

Ptr<Packet>
DO::ToWire (Ptr<const nnn::DO> do_p)
{
	Ptr<const Packet> p = do_p->GetWire ();
	if (!p)
	{
		Ptr<Packet> packet = Create<Packet> (*do_p->GetPayload ());
		DO wireEncoding (ConstCast<nnn::DO> (do_p));
		packet->AddHeader (wireEncoding);
		do_p->SetWire (packet);

		p = packet;
	}
	return p->Copy ();
}

Ptr<nnn::DO>
DO::FromWire (Ptr<Packet> packet)
{
	Ptr<nnn::DO> do_p = Create<nnn::DO> ();
	Ptr<Packet> wire = packet->Copy ();

	DO wireEncoding (do_p);
	packet->RemoveHeader (wireEncoding);

	do_p->SetPayload (packet);
	do_p->SetWire (wire);

	return do_p;
}

uint32_t
DO::GetSerializedSize (void) const
{
	size_t size =
			4 +                                  /* Packetid */
			2 +                                  /* Length of packet */
			2 +                                  /* Timestamp */
			NnnSim::SerializedSizeName (m_do_p->GetName ()) + /* Name size */
			m_do_p->GetPayload ()->GetSerializedSize();    /* Payload data */

	NS_LOG_INFO ("Serialize size = " << size);
	return size;
}

void
DO::Serialize (Buffer::Iterator start) const
{
	// Get the total size of the serialized packet
	uint32_t totalsize = GetSerializedSize ();
	// Find out the payload size
	uint32_t payloadsize = m_do_p->GetPayload ()->GetSerializedSize ();
	// Create a buffer to be able to serialize the payload
	uint8_t buffer[payloadsize];

	// Serialize packetid
	start.WriteU32(m_do_p->GetPacketId());
	// Get the length of the packet
	start.WriteU16(totalsize - 4); // Minus packetid size of 32 bits
	// Check that the lifetime is within the limits
	NS_ASSERT_MSG (0 <= m_do_p->GetLifetime ().ToInteger (Time::S) &&
			m_do_p->GetLifetime ().ToInteger (Time::S) < 65535,
			"Incorrect Lifetime (should not be smaller than 0 and larger than 65535");

	// Round lifetime to seconds
	start.WriteU16 (static_cast<uint16_t> (m_do_p->GetLifetime ().ToInteger (Time::S)));

	// Serialize NNN address
	NnnSim::SerializeName(start, m_do_p->GetName());

	// Serialize the payload and place it in the variable buffer
	m_do_p->GetPayload ()->Serialize (buffer, payloadsize);

	for (int i = 0; i < payloadsize; i++)
	{
		start.WriteU8(buffer[i]);
	}
}

uint32_t
DO::Deserialize (Buffer::Iterator start)
{
	Buffer::Iterator i = start;

	// Read packet id
	if (i.ReadU32 () != 1)
		throw new NULLpException ();

	// Read length of packet
	uint16_t packet_len = i.ReadU16 ();

	// Read lifetime of the packet
	m_do_p->SetLifetime (Seconds (i.ReadU16 ()));

	// Update the length. The resulting size is the packet size that
	// we have to deserialize
	packet_len -= 4;

	// Deserialize the name
	m_do_p->SetName(NnnSim::DeserializeName(i));

	// Substract the size of what we just got
	packet_len -= NnnSim::SerializedSizeName (m_do_p->GetName ());

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
	m_do_p->SetPayload (depacket);

	NS_ASSERT (GetSerializedSize () == (i.GetDistanceFrom (start)));

	return i.GetDistanceFrom (start);
}

void
DO::Print (std::ostream &os) const
{
	m_do_p->Print (os);
}

}
}

NNN_NAMESPACE_END
