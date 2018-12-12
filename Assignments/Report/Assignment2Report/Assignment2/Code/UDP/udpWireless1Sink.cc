/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program conducts a simple experiment: It simulates two nodes comunicate
 * over a wireless channel using UDP. One node generates the packets and the
 * other node receives them. The throughput at the received node is calculated
 * every  0.1s and is written to the file 'udpWireless.dat'. ASCII and pcap packet
 * trace are also enable for detail packet exchange analysis.
 * 
 * Author: Quang Le-Dang, Feb 2013 for EE452 <quang.ledang2@mail.dcu.ie>
 * Updated for 802.11ac: Anderson Simiscuka <anderson.simiscuka2@mail.dcu.ie>
 *
 *          EE452 Assignment 2018-2019
 *          Default network topology
 *
 *                  802.11ac
 *     Node 0 ------------------- Node 1
 *       |                          |
 *      UDP   ------------------> UDP
 *  traffic source             traffic sink
 * 
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/applications-module.h"
#include <iostream>
#include "ns3/flow-monitor-module.h"
#include "ns3/netanim-module.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("udpWireless");

uint32_t totalBytesReceived(0), mbs (0);
std::ofstream rdTrace;

//---------------------------------------------------------------------------------------
//-- Callback function is called whenever a packet is received successfully.
//-- This function cumulatively add the size of data packet to totalBytesReceived counter.
//---------------------------------------------------------------------------------------
void
ReceivePacket(std::string context, Ptr <const Packet> p)
{
  totalBytesReceived += p->GetSize();
}

//---------------------------------------------------------------------------------------
//-- Throughput calculating function.
//-- This function calculate the throughput every 0.1s using totalBytesReceived counter
//-- and write the results into a throughput tracefile
//---------------------------------------------------------------------------------------
void
CalculateThroughput()
{
  double mbs = ((totalBytesReceived*8.0)/100000);
  totalBytesReceived =0;
  rdTrace << Simulator::Now ().GetSeconds() << "\t"<< mbs << "\n";
  Simulator::Schedule (Seconds (0.1), &CalculateThroughput);
}

//---------------------------------------------------------------------------------------
// -- main
//---------------------------------------------------------------------------------------
int
main (int argc, char *argv[])
{
  //LogComponentEnable ("PacketSink",LOG_LEVEL_INFO);
  //-----------------------------------------------------
  //-- Open throughput trace file
  //-----------------------------------------------------
  rdTrace.open("udpWireless1Sink.dat", std::ios::out);
  rdTrace << "# Time \t Throughput \n";

  //-----------------------------------------------------
  //-- Creating nodes for simulation
  //-----------------------------------------------------
  NodeContainer nodes;
  nodes.Create (2);

  //-----------------------------------------------------
  //-- Creating Wifi channels for created nodes
  //-----------------------------------------------------
  // Change in default constructor from ns3.25 to ns3.29
  // For use in ns3.25 uncomment lines conatining "::Default()", comment lines using "()"
  WifiHelper wifi = WifiHelper::Default ();
  wifi.WifiHelper::SetStandard (WIFI_PHY_STANDARD_80211ac);
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");
  
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
  channel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  channel.AddPropagationLoss ("ns3::NakagamiPropagationLossModel");
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default();
  wifiPhy.SetChannel (channel.Create());
  
  NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default();
  wifiMac.SetType ("ns3::AdhocWifiMac");
  
  NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, nodes);
  
  //-----------------------------------------------------
  //-- Creating mobility models for created wireless nodes
  //-----------------------------------------------------
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc -> Add (Vector (0.0, 0.0, 0.0));
  positionAlloc -> Add (Vector (5.0, 0.0, 0.0));
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);
  
  //-----------------------------------------------------
  //-- Install TCP/IP stack and assign IP address for nodes to communicate
  //-----------------------------------------------------
  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  //-----------------------------------------------------
  //-- Create, setup and install ON/OFF traffic generator
  //-----------------------------------------------------
  OnOffHelper onoff ("ns3::UdpSocketFactory",
                     Address (InetSocketAddress (interfaces.GetAddress (1), 9)));
  onoff.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]")); 
  onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
  onoff.SetAttribute ("PacketSize", StringValue ("1024"));
  onoff.SetAttribute ("DataRate", StringValue ("10000kb/s"));
  
  ApplicationContainer apps = onoff.Install (nodes.Get (0));
  apps.Start (Seconds (2.0));
  apps.Stop (Seconds (10.0));

  //-----------------------------------------------------
  //-- Creating a sink to receive the packets from the traffic generator
  //-----------------------------------------------------
  PacketSinkHelper sinkHelper ("ns3::UdpSocketFactory",
                               InetSocketAddress (Ipv4Address::GetAny (), 9));
  ApplicationContainer sinkApp = sinkHelper.Install (nodes.Get(1));
  sinkApp.Start (Seconds (2.0));
  sinkApp.Stop (Seconds (10.0));
  
  //-----------------------------------------------------
  //-- Connect MacRx event at MAC layer of sink node to ReceivePacket function
  //-- for throughput calculation.
  //-----------------------------------------------------
  Config::Connect ("/NodeList/1/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx", MakeCallback(&ReceivePacket) );
  
  //-----------------------------------------------------
  //-- Enable ASCII and PCAP tracing
  //-----------------------------------------------------
  AsciiTraceHelper ascii;
  wifiPhy.EnableAsciiAll (ascii.CreateFileStream("udpWireless1Sink.tr"));
  wifiPhy.EnablePcapAll("udpWireless1Sink");
  
  //-----------------------------------------------------
  //-- Trigger CalculateThroughput function, it will schedule itself afterwards
  //-----------------------------------------------------
  Simulator::Schedule (Seconds(0.0),&CalculateThroughput);
  
  //-----------------------------------------------------
  //-- Flow Monitor Setup
  //-----------------------------------------------------
  Ptr<FlowMonitor>flowMonitor;
  FlowMonitorHelper flowHelper;
  flowMonitor = flowHelper.InstallAll();

  //-----------------------------------------------------
  //-- Schedule a stop time at 60s and start the simulation
  //-----------------------------------------------------
  Simulator::Stop (Seconds (60));
  Simulator::Run ();
  flowMonitor->SerializeToXmlFile("udpWireless1Sink.xml", true, true);
  Simulator::Destroy ();
  
  return 0;
}
