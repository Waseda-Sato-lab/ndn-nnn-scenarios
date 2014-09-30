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

class INF : public Header
{
public:
	INF ();
	INF (Ptr<nnn::INF> inf_p);

	Ptr<nnn::INF>
	GetINF ();

	static Ptr<Packet>
	ToWire (Ptr<const nnn::INF> inf_p);

	static Ptr<nnn::INF>
	FromWire (Ptr<Packet> packet);

	// from Header
	static TypeId GetTypeId (void);
	virtual TypeId GetInstanceTypeId (void) const;
	virtual void Print (std::ostream &os) const;
	virtual uint32_t GetSerializedSize (void) const;
	virtual void Serialize (Buffer::Iterator start) const;
	virtual uint32_t Deserialize (Buffer::Iterator start);

private:
	Ptr<nnn::INF> m_inf_p;
};

}
}

NNN_NAMESPACE_END
