/* -*- Mode:C++; c-file-style:"gnu" -*- */
/*
 * Copyright 2014 Waseda University, Sato Laboratory
 *   Author: Zhu Li <phillipszhuli1990@gmail.com>
 *           Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 *  nnn-stack-helper.cc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  nnn-stack-helper.cc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with nnn-stack-helper.cc.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NNN_STACK_HELPER_H
#define NNN_STACK_HELPER_H

#include <ns3-dev/ns3/nstime.h>
#include <ns3-dev/ns3/object-factory.h>
#include <ns3-dev/ns3/packet.h>
#include <ns3-dev/ns3/ptr.h>

namespace ns3 {

class Node;

namespace nnn {

class FaceContainer;
class Face;
class NetDeviceFace;
class NNNL3Protocol;

/**
 * \ingroup nnn
 * \defgroup nnn-helpers Helpers
 */
/**
 * \ingroup nnn-helpers
 * \brief Adding nnn functionality to existing Nodes.
 *
 * This helper enables pcap and ascii tracing of events in the nnn stack
 * associated with a node.  This is substantially similar to the tracing that
 * happens in device helpers, but the important difference is that, well, there
 * is no device.  This means that the creation of output file names will change,
 * and also the user-visible methods will not reference devices and therefore
 * the number of trace enable methods is reduced.
 *
 * Normally we eschew multiple inheritance, however, the classes
 * PcapUserHelperFornnn and AsciiTraceUserHelperFornnn are treated as
 * "mixins".  A mixin is a self-contained class that encapsulates a general
 * attribute or a set of functionality that may be of interest to many other
 * classes.
 */
class NNNStackHelper
{
public:
	/**
	 * \brief Create a new NnnStackHelper with a default NDN_FLOODING NNNForwarding stategy
	 */
	NNNStackHelper();

	/**
	 * \brief Destroy the NnnStackHelper
	 */
	virtual ~NNNStackHelper ();

	/**
	 * @brief Set parameters of NnnL3Protocol
	 */
	void
	SetStackAttributes (const std::string &attr1 = "", const std::string &value1 = "",
			const std::string &attr2 = "", const std::string &value2 = "",
			const std::string &attr3 = "", const std::string &value3 = "",
			const std::string &attr4 = "", const std::string &value4 = "");


	/**
	 * @brief Set NNNForwarding strategy class and its attributes
	 * @param NNNForwardingStrategyClass string containing name of the NNNForwarding strategy class
	 *
	 * Valid options are "ns3::NdnFloodingStrategy" (default) and "ns3::NdnBestRouteStrategy"
	 *
	 * Other strategies can be implemented, inheriting ns3::NNNForwardingStrategy class
	 */
	void
	SetNNNForwardingStrategy (const std::string &NNNForwardingStrategyClass,
			const std::string &attr1 = "", const std::string &value1 = "",
			const std::string &attr2 = "", const std::string &value2 = "",
			const std::string &attr3 = "", const std::string &value3 = "",
			const std::string &attr4 = "", const std::string &value4 = "");


	/**
	 * @brief Set content store class and its attributes
	 * @param contentStoreClass string, representing class of the content store
	 */

	/**
	 * @brief Set PIT class and its attributes
	 * @param pitClass string, representing class of PIT

  void
  SetPit (const std::string &pitClass,
          const std::string &attr1 = "", const std::string &value1 = "",
          const std::string &attr2 = "", const std::string &value2 = "",
          const std::string &attr3 = "", const std::string &value3 = "",
          const std::string &attr4 = "", const std::string &value4 = "");
	 */

	/**
	 * @brief Set NNST class and its attributes
	 * @param nnstClass string, representing class of NNST
	 */
	void
	SetNNST (const std::string &nnstClass,
			const std::string &attr1 = "", const std::string &value1 = "",
			const std::string &attr2 = "", const std::string &value2 = "",
			const std::string &attr3 = "", const std::string &value3 = "",
			const std::string &attr4 = "", const std::string &value4 = "");

	typedef Callback< Ptr<NetDeviceFace>, Ptr<Node>, Ptr<NNNL3Protocol>, Ptr<NetDevice> > NetDeviceFaceCreateCallback;

	/**
	 * @brief Add callback to create and configure instance of the face, based on supplied Ptr<Node> and Ptr<NetDevice>
	 *
	 * It is possible to set up several callbacks for different NetDevice types.
	 *
	 * Currently, there is only one specialized callback for PointToPointNetDevice, which creates face and sets limits (if enabled)
	 * based on PointToPoint link parameters
	 *
	 * If none of the callbacks fit the TypeId of NetDevice, a default callback is used (DefaultNetDeviceCallback)
	 */
	void
	AddNetDeviceFaceCreateCallback (TypeId netDeviceType, NetDeviceFaceCreateCallback callback);

	/**
	 * @brief Update callback to create and configure instance of the face, based on supplied Ptr<Node> and Ptr<NetDevice>
	 *
	 * It is possible to set up several callbacks for different NetDevice types.
	 *
	 * Using this method, it is possible to override Face creation for PointToPointNetDevices
	 */
	void
	UpdateNetDeviceFaceCreateCallback (TypeId netDeviceType, NetDeviceFaceCreateCallback callback);

	/**
	 * @brief Remove callback to create and configure instance of the face, based on supplied Ptr<Node> and Ptr<NetDevice>
	 */
	void
	RemoveNetDeviceFaceCreateCallback (TypeId netDeviceType, NetDeviceFaceCreateCallback callback);

	/**
	 * @brief Enable Interest limits (disabled by default)
	 *
	 * @param enable           Enable or disable limits
	 * @param avgRtt           Average RTT
	 * @param avgData Average size of contentObject packets (including all headers)
	 * @param avgInterest      Average size of interest packets (including all headers)

void
  EnableLimits (bool enable = true, Time avgRtt=Seconds(0.1), uint32_t avgData=1100, uint32_t avgInterest=40);
	 */

	/**
	 * \brief Install Nnn stack on the node
	 *
	 * This method will assert if called on a node that already has Nnn object
	 * installed on it
	 *
	 * \param nodeName The name of the node on which to install the stack.
	 *
	 * \returns list of installed faces in the form of a smart pointer
	 * to NnnFaceContainer object
	 */
	Ptr<FaceContainer>
	Install (const std::string &nodeName) const;

	/**
	 * \brief Install Nnn stack on the node
	 *
	 * This method will assert if called on a node that already has Ndn object
	 * installed on it
	 *
	 * \param node The node on which to install the stack.
	 *
	 * \returns list of installed faces in the form of a smart pointer
	 * to FaceContainer object
	 */
	Ptr<FaceContainer>
	Install (Ptr<Node> node) const;

	/**
	 * \brief Install Nnn stack on each node in the input container
	 *
	 * The program will assert if this method is called on a container with a node
	 * that already has an ndn object aggregated to it.
	 *
	 * \param c NodeContainer that holds the set of nodes on which to install the
	 * new stacks.
	 *
	 * \returns list of installed faces in the form of a smart pointer
	 * to FaceContainer object
	 */
	Ptr<FaceContainer>
	Install (const NodeContainer &c) const;

	/**
	 * \brief Install Nnn stack on all nodes in the simulation
	 *
	 * \returns list of installed faces in the form of a smart pointer
	 * to FaceContainer object
	 */
	Ptr<FaceContainer>
	InstallAll () const;

	/**
	 * \brief Add NNNForwarding entry to NNST
	 *
	 * \param nodeName Node name
	 * \param prefix Routing prefix
	 * \param faceId Face index
	 * \param metric Routing metric
	 */
	static void
	AddRoute (const std::string &nodeName, const std::string &prefix, uint32_t faceId, int32_t metric);

	/**
	 * \brief Add NNNForwarding entry to NNST
	 *
	 * \param nodeName Node
	 * \param prefix Routing prefix
	 * \param faceId Face index
	 * \param metric Routing metric
	 */
	static void
	AddRoute (Ptr<Node> node, const std::string &prefix, uint32_t faceId, int32_t metric);

	/**
	 * \brief Add NNNForwarding entry to NNST
	 *
	 * \param node   Node
	 * \param prefix Routing prefix
	 * \param face   Face
	 * \param metric Routing metric
	 */
	static void
	AddRoute (Ptr<Node> node, const std::string &prefix, Ptr<Face> face, int32_t metric);

	/**
	 * @brief Add NNNForwarding entry to NNST (work only with point-to-point links)
	 *
	 * \param node Node
	 * \param prefix Routing prefix
	 * \param otherNode The other node, to which interests (will be used to infer face id
	 * \param metric Routing metric
	 */
	static void
	AddRoute (Ptr<Node> node, const std::string &prefix, Ptr<Node> otherNode, int32_t metric);

	/**
	 * @brief Add NNNForwarding entry to NNST (work only with point-to-point links)
	 *
	 * \param nodeName Node name (refer to ns3::Names)
	 * \param prefix Routing prefix
	 * \param otherNode The other node name, to which interests (will be used to infer face id (refer to ns3::Names)
	 * \param metric Routing metric
	 */
	static void
	AddRoute (const std::string &nodeName, const std::string &prefix, const std::string &otherNodeName, int32_t metric);

	/**
	 * \brief Set flag indicating necessity to install default routes in NNST
	 */
	void
	SetDefaultRoutes (bool needSet);

private:
	Ptr<NetDeviceFace>
	DefaultNetDeviceCallback (Ptr<Node> node, Ptr<NNNL3Protocol> ndn, Ptr<NetDevice> netDevice) const;

	Ptr<NetDeviceFace>
	PointToPointNetDeviceCallback (Ptr<Node> node, Ptr<NNNL3Protocol> ndn, Ptr<NetDevice> netDevice) const;

private:
	NNNStackHelper (const NNNStackHelper &);
	NNNStackHelper &operator = (const NNNStackHelper &o);

private:
	ObjectFactory m_nnnFactory;
	ObjectFactory m_nnnforwardingstrategyFactory;
	//  ObjectFactory m_contentStoreFactory;
	//  ObjectFactory m_pitFactory;
	ObjectFactory m_nnstFactory;

	//  bool     m_limitsEnabled;
	//  Time     m_avgRtt;
	//  uint32_t m_avgDataSize;
	//  uint32_t m_avgInterestSize;
	bool     m_needSetDefaultRoutes;

	typedef std::list< std::pair<TypeId, NetDeviceFaceCreateCallback> > NetDeviceCallbackList;
	NetDeviceCallbackList m_netDeviceCallbacks;
};

} // namespace ndn
} // namespace ns3

#endif /* NNN_STACK_HELPER_H */
