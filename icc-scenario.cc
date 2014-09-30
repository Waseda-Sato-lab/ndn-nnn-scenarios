/* -*- Mode:C++; c-file-style:"gnu"; -*- */
/*
 * ndn-mobility-random.cc
 *  Random walk Wifi Mobile scenario for ndnSIM
 *
 * Copyright (c) 2014 Waseda University, Sato Laboratory
 * Author: Jairo Eduardo Lopez <jairo@ruri.waseda.jp>
 *
 * Special thanks to University of Washington for initial templates
 *
 *  ndn-mobility-random is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  ndn-mobility-random is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero Public License for more details.
 *
 *  You should have received a copy of the GNU Affero Public License
 *  along with ndn-mobility-random.  If not, see <http://www.gnu.org/licenses/>.
 */

// Standard C++ modules
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iterator>
#include <iostream>
#include <map>
#include <string>
#include <sys/time.h>
#include <vector>

// Random modules
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/variate_generator.hpp>

// ns3 modules
#include <ns3-dev/ns3/applications-module.h>
#include <ns3-dev/ns3/bridge-helper.h>
#include <ns3-dev/ns3/csma-module.h>
#include <ns3-dev/ns3/core-module.h>
#include <ns3-dev/ns3/mobility-module.h>
#include <ns3-dev/ns3/network-module.h>
#include <ns3-dev/ns3/point-to-point-module.h>
#include <ns3-dev/ns3/wifi-module.h>

// ndnSIM modules
#include <ns3-dev/ns3/ndnSIM-module.h>
#include <ns3-dev/ns3/ndnSIM/utils/tracers/ipv4-rate-l3-tracer.h>
#include <ns3-dev/ns3/ndnSIM/utils/tracers/ipv4-seqs-app-tracer.h>

// Extension files
// #include "minstrel-wifi-manager.h"

using namespace ns3;
using namespace boost;
using namespace std;

namespace br = boost::random;

typedef struct timeval TIMER_TYPE;
#define TIMER_NOW(_t) gettimeofday (&_t,NULL);
#define TIMER_SECONDS(_t) ((double)(_t).tv_sec + (_t).tv_usec*1e-6)
#define TIMER_DIFF(_t1, _t2) (TIMER_SECONDS (_t1)-TIMER_SECONDS (_t2))

char scenario[250] = "fakeInterest";

NS_LOG_COMPONENT_DEFINE (scenario);

// Number generator
br::mt19937_64 gen;

// Obtains a random number from a uniform distribution between min and max.
// Must seed number generator to ensure randomness at runtime.
int obtain_Num(int min, int max)
{
	br::uniform_int_distribution<> dist(min, max);
	return dist(gen);
}

// Obtain a random double from a uniform distribution between min and max.
// Must seed number generator to ensure randomness at runtime.
double obtain_Num(double min, double max)
{
	br::uniform_real_distribution<> dist(min, max);
	return dist(gen);
}

// Function to change the SSID of a Node, depending on distance
void SetSSIDviaDistance(uint32_t mtId, Ptr<MobilityModel> node, std::map<std::string, Ptr<MobilityModel> > aps)
{
	char configbuf[250];
	char buffer[250];

	// This causes the device in mtId to change the SSID, forcing AP change
	sprintf(configbuf, "/NodeList/%d/DeviceList/0/$ns3::WifiNetDevice/Mac/Ssid", mtId);

	std::map<double, std::string> SsidDistance;

	// Iterate through the map of seen Ssids
	for (std::map<std::string, Ptr<MobilityModel> >::iterator ii=aps.begin(); ii!=aps.end(); ++ii)
	{
		// Calculate the distance from the AP to the node and save into the map
		SsidDistance[node->GetDistanceFrom((*ii).second)] = (*ii).first;
	}

	double distance = SsidDistance.begin()->first;
	std::string ssid(SsidDistance.begin()->second);

	sprintf(buffer, "Change to SSID %s at distance of %f", ssid.c_str(), distance);

	NS_LOG_INFO(buffer);

	// Because the map sorts by std:less, the first position has the lowest distance
	Config::Set(configbuf, SsidValue(ssid));

	// Empty the maps
	SsidDistance.clear();
}

int main (int argc, char *argv[])
{
	// These are our scenario arguments
	uint32_t sectors = 2;                         // Number of wireless sectors
	uint32_t aps = 2;					          // Number of wireless access nodes in a sector
	uint32_t mobile = 1;				          // Number of mobile terminals
	uint32_t servers = 1;				          // Number of servers in the network
	uint32_t wnodes = aps * sectors;              // Number of nodes in the network
	uint32_t xaxis = 300;                         // Size of the X axis
	uint32_t yaxis = 300;                         // Size of the Y axis
	double sec = 0.0;                             // Movement start
	bool fake = false;							  // Enable fake interest or not
	bool traceFiles = false;                      // Tells to run the simulation with traceFiles
	bool smart = false;                           // Tells to run the simulation with SmartFlooding
	bool bestr = false;                           // Tells to run the simulation with BestRoute
	bool walk = true;                             // Do random walk at walking speed
	double speed= 5;							  // MN's speed	change here (1.4 | 8.3 | 16.7)
	char results[250] = "results";                // Directory to place results
	double endTime = 200;                         // Number of seconds to run the simulation
	double MBps = 0.15;                           // MB/s data rate desired for applications
	int contentSize = -1;                         // Size of content to be retrieved
	int maxSeq = -1;                              // Maximum number of Data packets to request
	double retxtime = 0.05;                       // How frequent Interest retransmission timeouts should be checked (seconds)
	int csSize = 10000000;                        // How big the Content Store should be
	//double deltaTime = 10;
        std::string nsTFile;                          // Name of the NS Trace file to use
	char nsTDir[250] = "./Waypoints";           // Directory for the waypoint files
	
	// Variable for buffer
	char buffer[250];

	CommandLine cmd;
	cmd.AddValue ("mobile", "Number of mobile terminals in simulation", mobile);
	cmd.AddValue ("servers", "Number of servers in the simulation", servers);
	cmd.AddValue ("results", "Directory to place results", results);
	cmd.AddValue ("start", "Starting second", sec);
	cmd.AddValue ("fake", "Enable fake interest", fake);
	cmd.AddValue ("trace", "Enable trace files", traceFiles);
	cmd.AddValue ("smart", "Enable SmartFlooding forwarding", smart);
	cmd.AddValue ("bestr", "Enable BestRoute forwarding", bestr);
	cmd.AddValue ("csSize", "Number of Interests a Content Store can maintain", csSize);
	cmd.AddValue ("walk", "Enable random walk at walking speed", walk);
	cmd.AddValue ("speed", "Number of speed/hour of mobile terminals in the simulation", speed);
	cmd.AddValue ("endTime", "How long the simulation will last (Seconds)", endTime);
	cmd.AddValue ("mbps", "Data transmission rate for NDN App in MBps", MBps);
	cmd.AddValue ("size", "Content size in MB (-1 is for no limit)", contentSize);
	cmd.AddValue ("retx", "How frequent Interest retransmission timeouts should be checked in seconds", retxtime);
	cmd.AddValue ("traceFile", "Directory containing Ns2 movement trace files (Usually created by Bonnmotion)", nsTDir);
	//cmd.AddValue ("deltaTime", "time interval (s) between updates (default 100)", deltaTime);	
	cmd.Parse (argc,argv);

	NS_LOG_INFO("Random walk at human walking speed - 1.4m/s");
	sprintf(buffer, "ns3::ConstantRandomVariable[Constant=%f]", speed);

	uint32_t top = speed;

		sprintf(buffer, "%s/Walk_random.ns_movements", nsTDir);
		switch (top)
		{
		case 5:
                	sprintf(buffer, "%s/1.4.ns_movements", nsTDir);
			break;
		case 10:
                	sprintf(buffer, "%s/2.8.ns_movements", nsTDir);
			break;
		case 20:
                	sprintf(buffer, "%s/5.6.ns_movements", nsTDir);
			break;
		case 30:
                	sprintf(buffer, "%s/8.3.ns_movements", nsTDir);
			break;
		case 40:
                	sprintf(buffer, "%s/11.2.ns_movements", nsTDir);
			break;
		case 50:
                	sprintf(buffer, "%s/13.9.ns_movements", nsTDir);
			break;
		case 60:
                	sprintf(buffer, "%s/16.7.ns_movements", nsTDir);
			break;
		case 70:
                	sprintf(buffer, "%s/19.4.ns_movements", nsTDir);
			break;
		case 80:
                	sprintf(buffer, "%s/22.2.ns_movements", nsTDir);
			break;
		}
		nsTFile = buffer;

	Ptr<ListPositionAllocator> initialMobile = CreateObject<ListPositionAllocator> ();
	initialMobile->Add(Vector(100.0, 0.0, 0.0));

	 // What the NDN Data packet payload size is fixed to 1024 bytes
	uint32_t payLoadsize = 1024;

	// Give the content size, find out how many sequence numbers are necessary
	if (contentSize > 0)
	{
		maxSeq = 1 + (((contentSize*1000000) - 1) / payLoadsize);
	}

	// How many Interests/second a producer creates
	double intFreq = (MBps * 1000000) / payLoadsize;

	vector<double> centralXpos;
	vector<double> centralYpos;
	centralXpos.push_back(50.0);
	centralXpos.push_back(250.0);
	centralYpos.push_back(-50.0);
	centralYpos.push_back(-50.0);

	vector<double> wirelessXpos;
	vector<double> wirelessYpos;
	wirelessXpos.push_back(0);
	wirelessXpos.push_back(100);
	wirelessXpos.push_back(200);
	wirelessXpos.push_back(300);
	wirelessYpos.push_back(0);
	wirelessYpos.push_back(0);
	wirelessYpos.push_back(0);
	wirelessYpos.push_back(0);

	NS_LOG_INFO ("------Creating nodes------");
	// Node definitions for mobile terminals (consumers)
	NodeContainer mobileTerminalContainer;
	mobileTerminalContainer.Create(mobile);

	std::vector<uint32_t> mobileNodeIds;

	// Save all the mobile Node IDs
	for (int i = 0; i < mobile; i++)
	{
		mobileNodeIds.push_back(mobileTerminalContainer.Get (i)->GetId ());
	}

	// Central Nodes
	NodeContainer centralContainer;
	centralContainer.Create (sectors);

	// Wireless access Nodes
	NodeContainer wirelessContainer;
	wirelessContainer.Create (wnodes);

	// Separate the wireless nodes into sector specific containers
	std::vector<NodeContainer> sectorNodes;

	for (int i = 0; i < sectors; i++)
	{
		NodeContainer wireless;
		for (int j = i*aps; j < aps + i*aps; j++)
		{
			wireless.Add(wirelessContainer.Get (j));
		}
		sectorNodes.push_back(wireless);
	}

	// Find out how many first level nodes we will have
	// The +1 is for the server which will be attached to the first level nodes
	int first = (sectors / 3) + 1;


	// Container for all NDN capable nodes
	NodeContainer allNdnNodes;
	allNdnNodes.Add (centralContainer);
	allNdnNodes.Add (wirelessContainer);

	// Container for server (producer) nodes
	NodeContainer serverNodes;
	serverNodes.Create (servers);

	std::vector<uint32_t> serverNodeIds;

	// Save all the mobile Node IDs
	for (int i = 0; i < servers; i++)
	{
		serverNodeIds.push_back(serverNodes.Get (i)->GetId ());
	}

	// Container for all nodes without NDN specific capabilities
	NodeContainer allUserNodes;
	allUserNodes.Add (mobileTerminalContainer);
	allUserNodes.Add (serverNodes);

	// Make sure to seed our random
	gen.seed (std::time (0) + (long long)getpid () << 32);

	MobilityHelper Server;
	Ptr<ListPositionAllocator> initialServer = CreateObject<ListPositionAllocator> ();

	Vector posServer (150, -100, 0.0);
	initialServer->Add (posServer);

	Server.SetPositionAllocator(initialServer);
	Server.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
	Server.Install(serverNodes);

	NS_LOG_INFO ("------Placing Central nodes-------");
	MobilityHelper centralStations;

	Ptr<ListPositionAllocator> initialCenter = CreateObject<ListPositionAllocator> ();

	for (int i = 0; i < sectors; i++)
	{
		Vector pos (centralXpos[i], centralYpos[i], 0.0);
		initialCenter->Add (pos);
	}

	centralStations.SetPositionAllocator(initialCenter);
	centralStations.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
	centralStations.Install(centralContainer);

	NS_LOG_INFO ("------Placing wireless access nodes------");
	MobilityHelper wirelessStations;

	Ptr<ListPositionAllocator> initialWireless = CreateObject<ListPositionAllocator> ();

	for (int i = 0; i < wnodes; i++)
	{
		Vector pos (wirelessXpos[i], wirelessYpos[i], 0.0);
		initialWireless->Add (pos);
	}

	wirelessStations.SetPositionAllocator(initialWireless);
	wirelessStations.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	wirelessStations.Install(wirelessContainer);

	NS_LOG_INFO ("------Placing mobile node and determining direction and speed------");
	MobilityHelper mobileStations;


	sprintf(buffer, "0|%d|0|%d", xaxis, yaxis);
	string bounds = string(buffer);


	sprintf(buffer, "Reading NS trace file %s", nsTFile.c_str());
    NS_LOG_INFO(buffer);

	Ns2MobilityHelper ns2 = Ns2MobilityHelper (nsTFile);
	ns2.Install ();

	// Connect Wireless Nodes to central nodes
	// Because the simulation is using Wifi, PtP connections are 100Mbps
	// with 5ms delay
	NS_LOG_INFO("------Connecting Central nodes to wireless access nodes------");

	vector <NetDeviceContainer> ptpWLANCenterDevices;

	PointToPointHelper p2p_100mbps5ms;
	p2p_100mbps5ms.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
	p2p_100mbps5ms.SetChannelAttribute ("Delay", StringValue ("5ms"));

	for (int i = 0; i < sectors; i++)
	{
		NetDeviceContainer ptpWirelessCenterDevices;

		for (int j = 0; j < aps; j++)
		{
			ptpWirelessCenterDevices.Add (p2p_100mbps5ms.Install (centralContainer.Get (i), sectorNodes[i].Get (j) ));
		}

		ptpWLANCenterDevices.push_back (ptpWirelessCenterDevices);
	}

	// Connect the server to central node
	NetDeviceContainer ptpServerlowerNdnDevices;
	for(int i =0; i < sectors; i++){
		ptpServerlowerNdnDevices.Add (p2p_100mbps5ms.Install (serverNodes.Get (0), centralContainer.Get (i)));

	}


	NS_LOG_INFO ("------Creating Wireless cards------");

	// Use the Wifi Helper to define the wireless interfaces for APs
	WifiHelper wifi;
	wifi.SetStandard(WIFI_PHY_STANDARD_80211g);
	// The N standard is apparently not completely supported in NS-3
	//wifi.setStandard(WIFI_PHY_STANDARD_80211n_2_4GHZ);
	// The ConstantRateWifiManager only works with one rate, making issues
	//wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager");
	// The MinstrelWifiManager isn't working on the current version of NS-3
	//wifi.SetRemoteStationManager ("ns3::MinstrelWifiManager");
	wifi.SetRemoteStationManager ("ns3::ArfWifiManager");


	YansWifiChannelHelper wifiChannel;
	wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
	wifiChannel.AddPropagationLoss ("ns3::ThreeLogDistancePropagationLossModel");
	wifiChannel.AddPropagationLoss("ns3::NakagamiPropagationLossModel");

	// All interfaces are placed on the same channel. Makes AP changes easy. Might
	// have to be reconsidered for multiple mobile nodes
	YansWifiPhyHelper wifiPhyHelper = YansWifiPhyHelper::Default ();
	wifiPhyHelper.SetChannel (wifiChannel.Create ());
	wifiPhyHelper.Set("TxPowerStart", DoubleValue(16.0206));
	wifiPhyHelper.Set("TxPowerEnd", DoubleValue(1));

	// Add a simple no QoS based card to the Wifi interfaces
	NqosWifiMacHelper wifiMacHelper = NqosWifiMacHelper::Default ();

	// Create SSIDs for all the APs
	std::vector<Ssid> ssidV;

	NS_LOG_INFO ("------Creating ssids for wireless cards------");

	// We store the Wifi AP mobility models in a map, ordered by the ssid string. Will be easier to manage when
	// calling the modified StaMApWifiMac
	std::map<std::string, Ptr<MobilityModel> > apTerminalMobility;

	for (int i = 0; i < wnodes; i++)
	{
		// Temporary string containing our SSID
		std::string ssidtmp("ap-" + boost::lexical_cast<std::string>(i));

		// Push the newly created SSID into a vector
		ssidV.push_back (Ssid (ssidtmp));

		// Get the mobility model for wnode i
		Ptr<MobilityModel> tmp = (wirelessContainer.Get (i))->GetObject<MobilityModel> ();

		// Store the information into our map
		apTerminalMobility[ssidtmp] = tmp;
	}

	NS_LOG_INFO ("------Assigning mobile terminal wireless cards------");

	NS_LOG_INFO ("Assigning AP wireless cards");
	std::vector<NetDeviceContainer> wifiAPNetDevices;
	for (int i = 0; i < wnodes; i++)
	{
		wifiMacHelper.SetType ("ns3::ApWifiMac",
						   "Ssid", SsidValue (ssidV[i]),
						   "BeaconGeneration", BooleanValue (true),
						   "BeaconInterval", TimeValue (Seconds (0.102)));

		wifiAPNetDevices.push_back (wifi.Install (wifiPhyHelper, wifiMacHelper, wirelessContainer.Get (i)));
	}

	// Create a Wifi station with a modified Station MAC.
	wifiMacHelper.SetType("ns3::StaWifiMac",
			"Ssid", SsidValue (ssidV[0]),
			"ActiveProbing", BooleanValue (true));

	NetDeviceContainer wifiMTNetDevices = wifi.Install (wifiPhyHelper, wifiMacHelper, mobileTerminalContainer);

	// Using the same calculation from the Yans-wifi-Channel, we obtain the Mobility Models for the
	// mobile node as well as all the Wifi capable nodes
	Ptr<MobilityModel> mobileTerminalMobility = (mobileTerminalContainer.Get (0))->GetObject<MobilityModel> ();

	std::vector<Ptr<MobilityModel> > mobileTerminalsMobility;

	// Get the list of mobile node mobility models
	for (int i = 0; i < mobile; i++)
	{
		mobileTerminalsMobility.push_back((mobileTerminalContainer.Get (i))->GetObject<MobilityModel> ());
	}

	char routeType[250];

	// Now install content stores and the rest on the middle node. Leave
	// out clients and the mobile node
	NS_LOG_INFO ("------Installing NDN stack on routers------");
	ndn::StackHelper ndnHelperRouters;

	// Decide what Forwarding strategy to use depending on user command line input
	if (smart) {
		sprintf(routeType, "%s", "smart");
		NS_LOG_INFO ("NDN Utilizing SmartFlooding");
		ndnHelperRouters.SetForwardingStrategy ("ns3::ndn::fw::SmartFlooding::PerOutFaceLimits", "Limit", "ns3::ndn::Limits::Window");
	} else if (bestr) {
		sprintf(routeType, "%s", "bestr");
		NS_LOG_INFO ("NDN Utilizing BestRoute");
		ndnHelperRouters.SetForwardingStrategy ("ns3::ndn::fw::BestRoute::PerOutFaceLimits", "Limit", "ns3::ndn::Limits::Window");
	} else {
		sprintf(routeType, "%s", "flood");
		NS_LOG_INFO ("NDN Utilizing Flooding");
		ndnHelperRouters.SetForwardingStrategy ("ns3::ndn::fw::Flooding::PerOutFaceLimits", "Limit", "ns3::ndn::Limits::Window");
	}

	// Set the Content Stores

	sprintf(buffer, "%d", csSize);

	ndnHelperRouters.SetContentStore ("ns3::ndn::cs::Freshness::Lru", "MaxSize", buffer);
	ndnHelperRouters.SetDefaultRoutes (true);
	// Install on ICN capable routers
	ndnHelperRouters.Install (allNdnNodes);

	// Create a NDN stack for the clients and mobile node
	ndn::StackHelper ndnHelperUsers;
	// These nodes have only one interface, so BestRoute forwarding makes sense
	ndnHelperUsers.SetForwardingStrategy ("ns3::ndn::fw::BestRoute");
	// No Content Stores are installed on these machines
	ndnHelperUsers.SetContentStore ("ns3::ndn::cs::Nocache");
	ndnHelperUsers.SetDefaultRoutes (true);
	ndnHelperUsers.Install (allUserNodes);

	NS_LOG_INFO ("------Installing Producer Application------");

	sprintf(buffer, "Producer Payload size: %d", payLoadsize);
	NS_LOG_INFO (buffer);

	// Create the producer on the mobile node
	ndn::AppHelper producerHelper ("ns3::ndn::Producer");
	producerHelper.SetPrefix ("/waseda/sato");
	producerHelper.SetAttribute ("StopTime", TimeValue (Seconds(endTime-1)));
	// Payload size is in bytes
	producerHelper.SetAttribute ("PayloadSize", UintegerValue(payLoadsize));
	producerHelper.Install (serverNodes);

	NS_LOG_INFO ("------Installing Consumer Application------");

	sprintf(buffer, "Consumer Interest/s frequency: %f", intFreq);
	NS_LOG_INFO (buffer);

	// Create the consumer on the randomly selected node
	ndn::AppHelper consumerHelper ("ns3::ndn::ConsumerCbr");
	consumerHelper.SetPrefix ("/waseda/sato");
	consumerHelper.SetAttribute ("Frequency", DoubleValue (intFreq));
	consumerHelper.SetAttribute ("StartTime", TimeValue (Seconds(1)));
	consumerHelper.SetAttribute ("StopTime", TimeValue (Seconds(endTime-1)));
	consumerHelper.SetAttribute ("RetxTimer", TimeValue (Seconds(retxtime)));
	if (maxSeq > 0)
		consumerHelper.SetAttribute ("MaxSeq", IntegerValue(maxSeq));

	consumerHelper.Install (mobileTerminalContainer);
	if(fake)	consumerHelper.Install (centralContainer);			//change here (normal / fake interest)

	sprintf(buffer, "Ending time! %f", endTime);
	NS_LOG_INFO(buffer);

	// If the variable is set, print the trace files
	if (traceFiles) {
		ifstream inpurFile;
		ofstream plotdata;
		// Filename
		char filename[250];

		// File ID
		char fileId[250];


		// Create the file identifier
		sprintf(fileId, "%s-%02d-%03d-%03d.txt", routeType, mobile, servers, wnodes);

/*		sprintf(filename, "%s/%s/%s/%.0f/clients", results, scenario, speed);

		std::ofstream clientFile;

		clientFile.open (filename);
		for (int i = 0; i < mobileNodeIds.size(); i++)
		{
			clientFile << mobileNodeIds[i] << std::endl;
		}

		clientFile.close();

		// Print server nodes to file
		sprintf(filename, "%s/%s/%s/%.0f/servers", results, scenario, speed);

		std::ofstream serverFile;

		serverFile.open (filename);
		for (int i = 0; i < serverNodeIds.size(); i++)
		{
			serverFile << serverNodeIds[i] << std::endl;
		}

		serverFile.close();
*/
		char mode[7];
		if(fake) sprintf(mode, "fake");
		else sprintf(mode, "normal");

		NS_LOG_INFO ("Installing tracers");
		// NDN Aggregate tracer
		printf ("now I'm writing the files at %s/%s/%s/%.0f/\n", results, scenario, mode, speed);
		sprintf (filename, "%s/%s/%s/%.0f/aggregate-trace", results, scenario, mode, speed);
		inputFile = filename;
		plotdata = "graphs/%s/%s/%s/aggregate-trace", results, scenario, mode);
		ndn::L3AggregateTracer::InstallAll(filename, Seconds (1.0));
		sprintf (filename, "%s/%s/%s/%.0f/MN-aggregate-trace", results, scenario, mode, speed);
		ndn::L3AggregateTracer::Install(mobileTerminalContainer.Get(0), filename, Seconds (1.0));

		// NDN L3 tracer
		sprintf (filename, "%s/%s/%s/%.0f/rate-trace", results, scenario, mode, speed);
		ndn::L3RateTracer::InstallAll (filename, Seconds (1.0));
		sprintf (filename, "%s/%s/%s/%.0f/MN-rate-trace", results, scenario, mode, speed);
		ndn::L3RateTracer::Install (mobileTerminalContainer.Get(0), filename, Seconds (1.0));

		// NDN App Tracer
		sprintf (filename, "%s/%s/%s/%.0f/app-delays", results, scenario, mode, speed);
		ndn::AppDelayTracer::InstallAll (filename);
		sprintf (filename, "%s/%s/%s/%.0f/MN-app-delays", results, scenario, mode, speed);
		ndn::AppDelayTracer::Install (mobileTerminalContainer.Get(0), filename);

		// L2 Drop rate tracer
//		sprintf (filename, "%s/%s/%s/%.0f/drop-trace", results, scenario, mode, speed);
//		L2RateTracer::InstallAll (filename, Seconds (0.5));

		// Content Store tracer
//		sprintf (filename, "%s/%s/%s/%.0f/cs-trace", results, scenario, mode, speed);
//		ndn::CsTracer::InstallAll (filename, Seconds (1));
	}

	NS_LOG_INFO ("------Scheduling events - SSID changes------");

	// Schedule AP Changes
	double apsec = 0.0;
	// How often should the AP check it's distance
	double checkTime = 10.0 / speed;
	double j = apsec;

	while ( j < endTime)
	{
		sprintf(buffer, "Running event at %f", j);
		NS_LOG_INFO(buffer);

		for (int i = 0; i < mobile; i++)
		{
			Simulator::Schedule (Seconds(j), &SetSSIDviaDistance, mobileNodeIds[i], mobileTerminalsMobility[i], apTerminalMobility);
		}

		j += checkTime;
	}

	NS_LOG_INFO ("------Ready for execution!------");

	Simulator::Stop (Seconds (endTime));
	Simulator::Run ();
	Simulator::Destroy ();
}
