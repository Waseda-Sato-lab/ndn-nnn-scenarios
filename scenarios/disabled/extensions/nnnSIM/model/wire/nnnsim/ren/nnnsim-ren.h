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

class REN : public Header
{
public:
	REN ();
	REN (Ptr<nnn::REN> ren_p);

	Ptr<nnn::REN>
	GetREN ();

	static Ptr<Packet>
	ToWire (Ptr<const nnn::REN> ren_p);

	static Ptr<nnn::REN>
	FromWire (Ptr<Packet> packet);

	// from Header
	static TypeId GetTypeId (void);
	virtual TypeId GetInstanceTypeId (void) const;
	virtual void Print (std::ostream &os) const;
	virtual uint32_t GetSerializedSize (void) const;
	virtual void Serialize (Buffer::Iterator start) const;
	virtual uint32_t Deserialize (Buffer::Iterator start);

private:
	Ptr<nnn::REN> m_ren_p;
};

}
}

NNN_NAMESPACE_END
