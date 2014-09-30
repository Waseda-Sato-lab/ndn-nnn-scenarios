#include "nnnsim-en.h"

NNN_NAMESPACE_BEGIN

namespace wire {
namespace nnnSIM {

NS_OBJECT_ENSURE_REGISTERED (EN);

NS_LOG_COMPONENT_DEFINE ("nnn.wire.nnnSIM.EN");

EN::EN ()
: m_en_p (Create<nnn::EN> ())
{
}

EN::EN (Ptr<nnn::EN> en_p)
: m_en_p (en_p)
{
}

Ptr<nnn::EN>
EN::GetEN ()
{
	return m_en_p;
}

TypeId
EN::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::nnn::EN::nnnSIM")
    		.SetGroupName ("Nnn")
    		.SetParent<Header> ()
    		.AddConstructor<EN> ()
    		;
	return tid;
}

TypeId
EN::GetInstanceTypeId (void) const
{
	return GetTypeId ();
}

Ptr<Packet>
EN::ToWire (Ptr<const nnn::EN> en_p)
{
	Ptr<const Packet> p = en_p->GetWire ();
	if (!p)
	{
		// Mechanism packets have no payload, make an empty packet
		Ptr<Packet> packet = Create<Packet> ();
		EN wireEncoding (ConstCast<nnn::EN> (en_p));
		packet->AddHeader (wireEncoding);
		en_p->SetWire (packet);

		p = packet;
	}
	return p->Copy ();
}

Ptr<nnn::EN>
EN::FromWire (Ptr<Packet> packet)
{
	Ptr<nnn::EN> en_p = Create<nnn::EN> ();
	Ptr<Packet> wire = packet->Copy ();

	EN wireEncoding (en_p);
	packet->RemoveHeader (wireEncoding);

	// Mechanism packets have no payload, make an empty packet
	en_p->SetWire (wire);

	return en_p;
}

uint32_t
EN::GetSerializedSize (void) const
{
	uint16_t poatype = m_en_p->GetPoaType ();
	size_t poatype_size = 0;
	size_t poa_num = 0;

	if (poatype == 0)
	{
		poa_num = m_en_p->GetNumPoa ();
		poatype_size = 6; // Hardcoded size of a Mac48Address
	}

	size_t size =
			4 +                                  /* Packetid */
			2 +                                  /* Length of packet */
			2 +                                  /* Timestamp */
			2 +                                  /* PoA Type */
			2 +                                  /* Number of PoAs */
			poa_num * poatype_size;              /* Total size of PoAs */

	NS_LOG_INFO ("Serialize size = " << size);
	return size;
}

void
EN::Serialize (Buffer::Iterator start) const
{
	// Get the total size of the serialized packet
	uint32_t totalsize = GetSerializedSize ();
	// Find out the PoA total size
	uint32_t totalpoabufsize = (totalsize -4 - 8);

	uint16_t poatype = m_en_p->GetPoaType ();
	size_t bufsize = 0;

	if (poatype == 0)
		bufsize = 6; // Hardcoded Mac48Address size

	// Create a buffer to be able to serialize PoAs
	uint8_t buffer[bufsize];

	uint32_t totalpoas = m_en_p->GetNumPoa();

	NS_ASSERT_MSG (totalpoabufsize == (totalpoas * bufsize),
			"Incorrect number of PoAs or serialization sizes" );

	// Serialize packetid
	start.WriteU32(m_en_p->GetPacketId());
	// Get the length of the packet
	start.WriteU16(totalsize - 4); // Minus packetid size of 32 bits
	// Check that the lifetime is within the limits
	NS_ASSERT_MSG (0 <= m_en_p->GetLifetime ().ToInteger (Time::S) &&
			m_en_p->GetLifetime ().ToInteger (Time::S) < 65535,
			"Incorrect Lifetime (should not be smaller than 0 and larger than 65535");

	// Round lifetime to seconds
	start.WriteU16 (static_cast<uint16_t> (m_en_p->GetLifetime ().ToInteger (Time::S)));

	// Serialize the PoA Type
	start.WriteU16(poatype);

	// Serialize the Number of PoAs
	start.WriteU16(totalpoas);

	// Go through the PoA list
	for (int i = 0; i < totalpoas; i++)
	{
		// Use the CopyTo function to get the bit representation
		m_en_p->GetOnePoa(i).CopyTo(buffer);

		// Since the bit representation is in 8 bit chunks, serialize it
		// accordingly
		for (int j = 0; j < bufsize; j++)
			start.WriteU8(buffer[j]);
	}
}

uint32_t
EN::Deserialize (Buffer::Iterator start)
{
	Buffer::Iterator i = start;

	// Read packet id
	if (i.ReadU32 () != 1)
		throw new ENException ();

	// Read length of packet
	uint16_t packet_len = i.ReadU16 ();

	// Read lifetime of the packet
	m_en_p->SetLifetime (Seconds (i.ReadU16 ()));

	// Read the PoA Type
	m_en_p->SetPoaType(i.ReadU16 ());

	size_t bufsize = 0;

	// Obtain how big the buffer has to be (dividing by 8)
	if (m_en_p->GetPoaType () == 0)
	{
		bufsize = 6;
	}

	// Read the number of PoAs the packet is holding
	uint32_t num_poa = i.ReadU16 ();

	// Update the length. The resulting size is the packet size that
	// we have to deserialize
	packet_len -= 8;

	// Create the buffer size
	uint8_t buffer[bufsize];

	// For the number of PoAs, read the 8 bits until the end of the PoA size
	for (int k = 0; k < num_poa; k++)
	{
		for (int j = 0; j < bufsize; j++)
		{
			buffer[j] = i.ReadU8 ();
		}

		if (m_en_p->GetPoaType () == 0)
		{
			Mac48Address tmp = Mac48Address ();
			tmp.CopyFrom (buffer);

			m_en_p->AddPoa (tmp);
		}
	}

	NS_ASSERT (GetSerializedSize () == (i.GetDistanceFrom (start)));

	return i.GetDistanceFrom (start);
}

void
EN::Print (std::ostream &os) const
{
	m_en_p->Print (os);
}

}
}

NNN_NAMESPACE_END
