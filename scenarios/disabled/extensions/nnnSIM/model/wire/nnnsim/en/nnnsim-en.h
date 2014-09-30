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

class EN : public Header
{
public:
	EN ();
	EN (Ptr<nnn::EN> en_p);

	Ptr<nnn::EN>
	GetEN ();

	static Ptr<Packet>
	ToWire (Ptr<const nnn::EN> en_p);

	static Ptr<nnn::EN>
	FromWire (Ptr<Packet> packet);

	// from Header
	static TypeId GetTypeId (void);
	virtual TypeId GetInstanceTypeId (void) const;
	virtual void Print (std::ostream &os) const;
	virtual uint32_t GetSerializedSize (void) const;
	virtual void Serialize (Buffer::Iterator start) const;
	virtual uint32_t Deserialize (Buffer::Iterator start);

private:
	Ptr<nnn::EN> m_en_p;
};

}
}

NNN_NAMESPACE_END
