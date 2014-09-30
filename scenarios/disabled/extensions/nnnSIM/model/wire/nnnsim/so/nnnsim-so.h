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

/**
 * @brief Routines to serialize/deserialize SO packets in nnnSIM format
 */
class SO : public Header
{
public:
	SO ();
	SO (Ptr<nnn::SO> so);

	Ptr<nnn::SO>
	GetSO ();

	static Ptr<Packet>
	ToWire (Ptr<const nnn::SO> so);

	static Ptr<nnn::SO>
	FromWire (Ptr<Packet> packet);

	// from Header
	static TypeId GetTypeId (void);
	virtual TypeId GetInstanceTypeId (void) const;
	virtual void Print (std::ostream &os) const;
	virtual uint32_t GetSerializedSize (void) const;
	virtual void Serialize (Buffer::Iterator start) const;
	virtual uint32_t Deserialize (Buffer::Iterator start);

private:
	Ptr<nnn::SO> m_so_p;
};

}
}

NNN_NAMESPACE_END
