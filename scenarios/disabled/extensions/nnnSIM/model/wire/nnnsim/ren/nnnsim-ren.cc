#include "nnnsim-ren.h"

NNN_NAMESPACE_BEGIN

namespace wire {
namespace nnnSIM {

NS_OBJECT_ENSURE_REGISTERED (REN);

NS_LOG_COMPONENT_DEFINE ("nnn.wire.nnnSIM.REN");

REN::REN ()
: m_ren_p (Create<nnn::REN> ())
{
}

REN::REN (Ptr<nnn::REN> ren_p)
: m_ren_p (ren_p)
{
}

Ptr<nnn::REN>
REN::GetREN()
{
	return m_ren_p;
}

TypeId
REN::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::nnn::REN::nnnSIM")
    		.SetGroupName ("Nnn")
    		.SetParent<Header> ()
    		.AddConstructor<REN> ()
    		;
	return tid;
}

TypeId
REN::GetInstanceTypeId (void) const
{
	return GetTypeId ();
}

Ptr<Packet>
REN::ToWire (Ptr<const nnn::REN> ren_p)
{
	Ptr<const Packet> p = ren_p->GetWire ();
	if (!p)
	{
		// Mechanism packets have no payload, make an empty packet
		Ptr<Packet> packet = Create<Packet> ();
		REN wireEncoding (ConstCast<nnn::REN> (ren_p));
		packet->AddHeader (wireEncoding);
		ren_p->SetWire (packet);

		p = packet;
	}
	return p->Copy ();
}

Ptr<nnn::REN>
REN::FromWire (Ptr<Packet> packet)
{
	Ptr<nnn::REN> ren_p = Create<nnn::REN> ();
	Ptr<Packet> wire = packet->Copy ();

	REN wireEncoding (ren_p);
	packet->RemoveHeader (wireEncoding);

	// Mechanism packets have no payload, make an empty packet
	ren_p->SetWire (wire);

	return ren_p;
}

uint32_t
REN::GetSerializedSize (void) const
{
	uint16_t poatype = m_ren_p->GetPoaType ();
	size_t poatype_size = 0;
	size_t poa_num = 0;

	if (poatype == 0)
	{
		poa_num = m_ren_p->GetNumPoa ();
		poatype_size = 6; // Hardcoded size of a Mac48Address
	}

	size_t size =
			4 +                                  /* Packetid */
			2 +                                  /* Length of packet */
			2 +                                  /* Timestamp */
			2 +                                  /* PoA Type */
			2 +                                  /* Number of PoAs */
			poa_num * poatype_size +             /* Total size of PoAs */
			2 +                                  /* Lease time */
			NnnSim::SerializedSizeName (m_ren_p->GetName ()); /* Name size */

	NS_LOG_INFO ("Serialize size = " << size);
	return size;
}

void
REN::Serialize (Buffer::Iterator start) const
{
	// Get the total size of the serialized packet
	uint32_t totalsize = GetSerializedSize ();

	// Serialize packetid
	start.WriteU32(m_ren_p->GetPacketId());

	// Get the length of the packet
	start.WriteU16(totalsize - 4); // Minus packetid size of 32 bits

	// Check that the lifetime is within the limits
	NS_ASSERT_MSG (0 <= m_ren_p->GetLifetime ().ToInteger (Time::S) &&
			m_ren_p->GetLifetime ().ToInteger (Time::S) < 65535,
			"Incorrect Lifetime (should not be smaller than 0 and larger than 65535");

	// Round lifetime to seconds
	start.WriteU16 (static_cast<uint16_t> (m_ren_p->GetLifetime ().ToInteger (Time::S)));

	uint16_t poatype = m_ren_p->GetPoaType ();
	size_t bufsize = 0;

	if (poatype == 0)
		bufsize = 6; // Hardcoded Mac48Address size

	// Create a buffer to be able to serialize PoAs
	uint8_t buffer[bufsize];

	uint32_t totalpoas = m_ren_p->GetNumPoa();

	// Serialize the PoA Type
	start.WriteU16(poatype);

	// Serialize the Number of PoAs
	start.WriteU16(totalpoas);

	// Go through the PoA list
	for (int i = 0; i < totalpoas; i++)
	{
		// Use the CopyTo function to get the bit representation
		m_ren_p->GetOnePoa(i).CopyTo(buffer);

		// Since the bit representation is in 8 bit chunks, serialize it
		// accordingly
		for (int j = 0; j < bufsize; j++)
			start.WriteU8(buffer[j]);
	}

	NS_ASSERT_MSG (0 <= m_ren_p->GetRemainLease ().ToInteger (Time::S) &&
			m_ren_p->GetRemainLease ().ToInteger (Time::S) < 65535,
			"Incorrect Lease time (should not be smaller than 0 and larger than 65535");

	// Round lease time to seconds
	start.WriteU16 (static_cast<uint16_t> (m_ren_p->GetRemainLease ().ToInteger (Time::S)));

	// Serialize NNN address
	NnnSim::SerializeName(start, m_ren_p->GetName());
}

uint32_t
REN::Deserialize (Buffer::Iterator start)
{
	Buffer::Iterator i = start;

	// Read packet id
	if (i.ReadU32 () != 1)
		throw new INFException ();

	// Read length of packet
	uint16_t packet_len = i.ReadU16 ();

	// Read lifetime of the packet
	m_ren_p->SetLifetime (Seconds (i.ReadU16 ()));

	// Read the PoA Type
	m_ren_p->SetPoaType (i.ReadU16 ());

	// Read the number of PoAs
	size_t bufsize = 0;

	// Obtain how big the buffer has to be (dividing by 8)
	if (m_ren_p->GetPoaType () == 0)
	{
		bufsize = 6;
	}

	// Read the number of PoAs the packet is holding
	uint32_t num_poa = i.ReadU16 ();

	// Create the buffer size
	uint8_t buffer[bufsize];

	// For the number of PoAs, read the 8 bits until the end of the PoA size
	for (int k = 0; k < num_poa; k++)
	{
		for (int j = 0; j < bufsize; j++)
		{
			buffer[j] = i.ReadU8 ();
		}

		if (m_ren_p->GetPoaType () == 0)
		{
			Mac48Address tmp = Mac48Address ();
			tmp.CopyFrom (buffer);

			m_ren_p->AddPoa (tmp);
		}
	}

	// Read the least time
	m_ren_p->SetRemainLease (Seconds (i.ReadU16 ()));

	// Deserialize the old name
	m_ren_p->SetName(NnnSim::DeserializeName(i));

	NS_ASSERT (GetSerializedSize () == (i.GetDistanceFrom (start)));

	return i.GetDistanceFrom (start);
}

void
REN::Print (std::ostream &os) const
{
	m_ren_p->Print (os);
}

}
}

NNN_NAMESPACE_END
