#include "../nnnsim-common.h"

NNN_NAMESPACE_BEGIN

/**
 * @brief Namespace encapsulating wire operations
 */
namespace wire {

/**
 * @brief Namespace for nnnSIM wire format operations
 */
namespace nnnSIM {

class AEN : public Header
{
public:
	AEN ();
	AEN (Ptr<nnn::AEN> aen_p);

	Ptr<nnn::AEN>
	GetAEN ();

	static Ptr<Packet>
	ToWire (Ptr<const nnn::AEN> aen_p);

	static Ptr<nnn::AEN>
	FromWire (Ptr<Packet> packet);

	// from Header
	static TypeId GetTypeId (void);
	virtual TypeId GetInstanceTypeId (void) const;
	virtual void Print (std::ostream &os) const;
	virtual uint32_t GetSerializedSize (void) const;
	virtual void Serialize (Buffer::Iterator start) const;
	virtual uint32_t Deserialize (Buffer::Iterator start);

private:
	Ptr<nnn::AEN> m_aen_p;
};

}
}

NNN_NAMESPACE_END
