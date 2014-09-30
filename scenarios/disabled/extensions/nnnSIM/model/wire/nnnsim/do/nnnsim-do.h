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
 * @brief Routines to serialize/deserialize DO packets in nnnSIM format
 */
class DO : public Header
{
public:
	DO ();
	DO (Ptr<nnn::DO> do_p);

	Ptr<nnn::DO>
	GetDO ();

	static Ptr<Packet>
	ToWire (Ptr<const nnn::DO> do_p);

	static Ptr<nnn::DO>
	FromWire (Ptr<Packet> packet);

	// from Header
	static TypeId GetTypeId (void);
	virtual TypeId GetInstanceTypeId (void) const;
	virtual void Print (std::ostream &os) const;
	virtual uint32_t GetSerializedSize (void) const;
	virtual void Serialize (Buffer::Iterator start) const;
	virtual uint32_t Deserialize (Buffer::Iterator start);

private:
	Ptr<nnn::DO> m_do_p;
};

}
}

NNN_NAMESPACE_END
