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
 * @brief Routines to serialize/deserialize NULL packets in nnnSIM format
 */
class NULLp : public Header
{
public:
	NULLp ();
	NULLp (Ptr<nnn::NULLp> null_p);

	Ptr<nnn::NULLp>
	GetNULLp ();

	static Ptr<Packet>
	ToWire (Ptr<const nnn::NULLp> null_p);

	static Ptr<nnn::NULLp>
	FromWire (Ptr<Packet> packet);

	// from Header
	static TypeId GetTypeId (void);
	virtual TypeId GetInstanceTypeId (void) const;
	virtual void Print (std::ostream &os) const;
	virtual uint32_t GetSerializedSize (void) const;
	virtual void Serialize (Buffer::Iterator start) const;
	virtual uint32_t Deserialize (Buffer::Iterator start);

private:
	Ptr<nnn::NULLp> m_null_p;
};
}
}

NNN_NAMESPACE_END
