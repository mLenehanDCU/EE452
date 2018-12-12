/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program conducts a simple experiment: It simulates two nodes comunicate
 * over a wireless channel using TCP. One node generates the packets and the
 * other node receives them. The throughput at the received node is calculated
 * every  0.1s and is written to the file 'tcpWireless.dat'. ASCII and pcap packet
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
 *      TCP   ------------------> TCP
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
  rdTrace.open("tcpWireless15Sink.dat", std::ios::out);
  rdTrace << "# Time \t Throughput \n";

  //-----------------------------------------------------
  //-- Creating nodes for simulation
  //-----------------------------------------------------
  NodeContainer nodes;
  nodes.Create (16);

  //-----------------------------------------------------
  //-- Creating Wifi channels for created nodes
  //-----------------------------------------------------
  WifiHelper wifi = WifiHelper::Default ();
  wifi.SetStandard (WIFI_PHY_STANDARD_80211ac);
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
  positionAlloc -> Add (Vector (1.0, 0.0, 0.0));
  positionAlloc -> Add (Vector (0.0, 1.0, 0.0));
  positionAlloc -> Add (Vector (0.0, 0.0, 1.0));
  positionAlloc -> Add (Vector (1.0, 1.0, 1.0));
  positionAlloc -> Add (Vector (1.0, 0.0, 1.0));
  positionAlloc -> Add (Vector (1.0, 1.0, 0.0));
  positionAlloc -> Add (Vector (0.0, 1.0, 1.0));
  positionAlloc -> Add (Vector (1.5, 0.0, 0.0));
  positionAlloc -> Add (Vector (0.0, 1.5, 0.0));
  positionAlloc -> Add (Vector (0.0, 0.0, 1.5));
  positionAlloc -> Add (Vector (1.5, 1.5, 1.5));
  positionAlloc -> Add (Vector (1.5, 0.0, 1.5));
  positionAlloc -> Add (Vector (1.5, 1.5, 0.0));
  positionAlloc -> Add (Vector (0.0, 1.5, 1.5));
  positionAlloc -> Add (Vector (2.5, 2.5, 2.5));
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
  uint16_t port = 50000;
  OnOffHelper onoff1 ("ns3::TcpSocketFactory",
                     Address (InetSocketAddress (interfaces.GetAddress (1), port)));
  onoff1.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
  onoff1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
  onoff1.SetAttribute ("PacketSize", StringValue ("1024"));
  onoff1.SetAttribute ("DataRate", StringValue ("10000kb/s"));
  
  ApplicationContainer apps1 = onoff1.Install (nodes.Get (0));
  apps1.Start (Seconds (2.0));
  apps1.Stop (Seconds (10.0));

  OnOffHelper onoff2 ("ns3::TcpSocketFactory",
                     Address (InetSocketAddress (interfaces.GetAddress (2), port)));
  onoff2.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
  onoff2.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
  onoff2.SetAttribute ("PacketSize", StringValue ("1024"));
  onoff2.SetAttribute ("DataRate", StringValue ("10000kb/s"));
  
  ApplicationContainer apps2 = onoff2.Install (nodes.Get (0));
  apps2.Start (Seconds (2.0));
  apps2.Stop (Seconds (10.0));

  OnOffHelper onoff3 ("ns3::TcpSocketFactory",
                     Address (InetSocketAddress (interfaces.GetAddress (3), port)));
  onoff3.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
  onoff3.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
  onoff3.SetAttribute ("PacketSize", StringValue ("1024"));
  onoff3.SetAttribute ("DataRate", StringValue ("10000kb/s"));
  
  ApplicationContainer apps3 = onoff3.Install (nodes.Get (0));
  apps3.Start (Seconds (2.0));
  apps3.Stop (Seconds (10.0));

  OnOffHelper onoff4 ("ns3::TcpSocketFactory",
                     Address (InetSocketAddress (interfaces.GetAddress (4), port)));
  onoff4.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
  onoff4.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
  onoff4.SetAttribute ("PacketSize", StringValue ("1024"));
  onoff4.SetAttribute ("DataRate", StringValue ("10000kb/s"));
  
  ApplicationContainer apps4 = onoff4.Install (nodes.Get (0));
  apps4.Start (Seconds (2.0));
  apps4.Stop (Seconds (10.0));

  OnOffHelper onoff5 ("ns3::TcpSocketFactory",
                     Address (InetSocketAddress (interfaces.GetAddress (5), port)));
  onoff5.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
  onoff5.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
  onoff5.SetAttribute ("PacketSize", StringValue ("1024"));
  onoff5.SetAttribute ("DataRate", StringValue ("10000kb/s"));
  
  ApplicationContainer apps5 = onoff5.Install (nodes.Get (0));
  apps5.Start (Seconds (2.0));
  apps5.Stop (Seconds (10.0));

  OnOffHelper onoff6 ("ns3::TcpSocketFactory",
                     Address (InetSocketAddress (interfaces.GetAddress (6), port)));
  onoff6.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
  onoff6.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
  onoff6.SetAttribute ("PacketSize", StringValue ("1024"));
  onoff6.SetAttribute ("DataRate", StringValue ("10000kb/s"));
  
  ApplicationContainer apps6 = onoff6.Install (nodes.Get (0));
  apps6.Start (Seconds (2.0));
  apps6.Stop (Seconds (10.0));

  OnOffHelper onoff7 ("ns3::TcpSocketFactory",
                     Address (InetSocketAddress (interfaces.GetAddress (7), port)));
  onoff7.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
  onoff7.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
  onoff7.SetAttribute ("PacketSize", StringValue ("1024"));
  onoff7.SetAttribute ("DataRate", StringValue ("10000kb/s"));
  
  ApplicationContainer apps7 = onoff7.Install (nodes.Get (0));
  apps7.Start (Seconds (2.0));
  apps7.Stop (Seconds (10.0));

  OnOffHelper onoff8 ("ns3::TcpSocketFactory",
                     Address (InetSocketAddress (interfaces.GetAddress (8), port)));
  onoff8.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
  onoff8.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
  onoff8.SetAttribute ("PacketSize", StringValue ("1024"));
  onoff8.SetAttribute ("DataRate", StringValue ("10000kb/s"));
  
  ApplicationContainer apps8 = onoff8.Install (nodes.Get (0));
  apps8.Start (Seconds (2.0));
  apps8.Stop (Seconds (10.0));

  OnOffHelper onoff9 ("ns3::TcpSocketFactory",
                     Address (InetSocketAddress (interfaces.GetAddress (9), port)));
  onoff9.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
  onoff9.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
  onoff9.SetAttribute ("PacketSize", StringValue ("1024"));
  onoff9.SetAttribute ("DataRate", StringValue ("10000kb/s"));
  
  ApplicationContainer apps9 = onoff9.Install (nodes.Get (0));
  apps9.Start (Seconds (2.0));
  apps9.Stop (Seconds (10.0));

  OnOffHelper onoff10 ("ns3::TcpSocketFactory",
                     Address (InetSocketAddress (interfaces.GetAddress (10), port)));
  onoff10.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
  onoff10.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
  onoff10.SetAttribute ("PacketSize", StringValue ("1024"));
  onoff10.SetAttribute ("DataRate", StringValue ("10000kb/s"));
  
  ApplicationContainer apps10 = onoff10.Install (nodes.Get (0));
  apps10.Start (Seconds (2.0));
  apps10.Stop (Seconds (10.0));

  OnOffHelper onoff11 ("ns3::TcpSocketFactory",
                     Address (InetSocketAddress (interfaces.GetAddress (11), port)));
  onoff11.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
  onoff11.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
  onoff11.SetAttribute ("PacketSize", StringValue ("1024"));
  onoff11.SetAttribute ("DataRate", StringValue ("10000kb/s"));
  
  ApplicationContainer apps11 = onoff11.Install (nodes.Get (0));
  apps11.Start (Seconds (2.0));
  apps11.Stop (Seconds (10.0));

  OnOffHelper onoff12 ("ns3::TcpSocketFactory",
                     Address (InetSocketAddress (interfaces.GetAddress (12), port)));
  onoff12.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
  onoff12.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
  onoff12.SetAttribute ("PacketSize", StringValue ("1024"));
  onoff12.SetAttribute ("DataRate", StringValue ("10000kb/s"));
  
  ApplicationContainer apps12 = onoff12.Install (nodes.Get (0));
  apps12.Start (Seconds (2.0));
  apps12.Stop (Seconds (10.0));

  OnOffHelper onoff13 ("ns3::TcpSocketFactory",
                     Address (InetSocketAddress (interfaces.GetAddress (13), port)));
  onoff13.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
  onoff13.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
  onoff13.SetAttribute ("PacketSize", StringValue ("1024"));
  onoff13.SetAttribute ("DataRate", StringValue ("10000kb/s"));
  
  ApplicationContainer apps13 = onoff13.Install (nodes.Get (0));
  apps13.Start (Seconds (2.0));
  apps13.Stop (Seconds (10.0));

  OnOffHelper onoff14 ("ns3::TcpSocketFactory",
                     Address (InetSocketAddress (interfaces.GetAddress (14), port)));
  onoff14.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
  onoff14.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
  onoff14.SetAttribute ("PacketSize", StringValue ("1024"));
  onoff14.SetAttribute ("DataRate", StringValue ("10000kb/s"));
  
  ApplicationContainer apps14 = onoff14.Install (nodes.Get (0));
  apps14.Start (Seconds (2.0));
  apps14.Stop (Seconds (10.0));

  OnOffHelper onoff15 ("ns3::TcpSocketFactory",
                     Address (InetSocketAddress (interfaces.GetAddress (15), port)));
  onoff15.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
  onoff15.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
  onoff15.SetAttribute ("PacketSize", StringValue ("1024"));
  onoff15.SetAttribute ("DataRate", StringValue ("10000kb/s"));
  
  ApplicationContainer apps15 = onoff15.Install (nodes.Get (0));
  apps15.Start (Seconds (2.0));
  apps15.Stop (Seconds (10.0));

  //-----------------------------------------------------
  //-- Creating a sink to receive the packets from the traffic generator
  //-----------------------------------------------------
  PacketSinkHelper sinkHelper1 ("ns3::TcpSocketFactory",
                               InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApp1 = sinkHelper1.Install (nodes.Get(1));
  sinkApp1.Start (Seconds (2.0));
  sinkApp1.Stop (Seconds (10.0));
  
  
  PacketSinkHelper sinkHelper2 ("ns3::TcpSocketFactory",
                               InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApp2 = sinkHelper2.Install (nodes.Get(2));
  sinkApp2.Start (Seconds (2.0));
  sinkApp2.Stop (Seconds (10.0));

  PacketSinkHelper sinkHelper3 ("ns3::TcpSocketFactory",
                               InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApp3 = sinkHelper3.Install (nodes.Get(3));
  sinkApp3.Start (Seconds (2.0));
  sinkApp3.Stop (Seconds (10.0));

  PacketSinkHelper sinkHelper4 ("ns3::TcpSocketFactory",
                               InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApp4 = sinkHelper4.Install (nodes.Get(4));
  sinkApp4.Start (Seconds (2.0));
  sinkApp4.Stop (Seconds (10.0));

  PacketSinkHelper sinkHelper5 ("ns3::TcpSocketFactory",
                               InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApp5 = sinkHelper5.Install (nodes.Get(5));
  sinkApp5.Start (Seconds (2.0));
  sinkApp5.Stop (Seconds (10.0));

  PacketSinkHelper sinkHelper6 ("ns3::TcpSocketFactory",
                               InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApp6 = sinkHelper6.Install (nodes.Get(6));
  sinkApp6.Start (Seconds (2.0));
  sinkApp6.Stop (Seconds (10.0));
  
  
  PacketSinkHelper sinkHelper7 ("ns3::TcpSocketFactory",
                               InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApp7 = sinkHelper7.Install (nodes.Get(7));
  sinkApp7.Start (Seconds (2.0));
  sinkApp7.Stop (Seconds (10.0));

  PacketSinkHelper sinkHelper8 ("ns3::TcpSocketFactory",
                               InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApp8 = sinkHelper8.Install (nodes.Get(8));
  sinkApp8.Start (Seconds (2.0));
  sinkApp8.Stop (Seconds (10.0));

  PacketSinkHelper sinkHelper9 ("ns3::TcpSocketFactory",
                               InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApp9 = sinkHelper9.Install (nodes.Get(9));
  sinkApp9.Start (Seconds (2.0));
  sinkApp9.Stop (Seconds (10.0));

  PacketSinkHelper sinkHelper10 ("ns3::TcpSocketFactory",
                               InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApp10 = sinkHelper10.Install (nodes.Get(10));
  sinkApp10.Start (Seconds (2.0));
  sinkApp10.Stop (Seconds (10.0));

  PacketSinkHelper sinkHelper11 ("ns3::TcpSocketFactory",
                               InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApp11 = sinkHelper11.Install (nodes.Get(11));
  sinkApp11.Start (Seconds (2.0));
  sinkApp11.Stop (Seconds (10.0));
  
  PacketSinkHelper sinkHelper12 ("ns3::TcpSocketFactory",
                               InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApp12 = sinkHelper12.Install (nodes.Get(12));
  sinkApp12.Start (Seconds (2.0));
  sinkApp12.Stop (Seconds (10.0));

  PacketSinkHelper sinkHelper13 ("ns3::TcpSocketFactory",
                               InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApp13 = sinkHelper13.Install (nodes.Get(13));
  sinkApp13.Start (Seconds (2.0));
  sinkApp13.Stop (Seconds (10.0));

  PacketSinkHelper sinkHelper14 ("ns3::TcpSocketFactory",
                               InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApp14 = sinkHelper14.Install (nodes.Get(14));
  sinkApp14.Start (Seconds (2.0));
  sinkApp14.Stop (Seconds (10.0));

  PacketSinkHelper sinkHelper15 ("ns3::TcpSocketFactory",
                               InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApp15 = sinkHelper15.Install (nodes.Get(15));
  sinkApp15.Start (Seconds (2.0));
  sinkApp15.Stop (Seconds (10.0));

  //-----------------------------------------------------
  //-- Connect MacRx event at MAC layer of sink node to ReceivePacket function
  //-- for throughput calculation.
  //-----------------------------------------------------
  Config::Connect ("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx", MakeCallback(&ReceivePacket) );
  Config::Connect ("/NodeList/1/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx", MakeCallback(&ReceivePacket) );
  Config::Connect ("/NodeList/2/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx", MakeCallback(&ReceivePacket) );
  Config::Connect ("/NodeList/3/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx", MakeCallback(&ReceivePacket) );
  Config::Connect ("/NodeList/4/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx", MakeCallback(&ReceivePacket) );
  Config::Connect ("/NodeList/5/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx", MakeCallback(&ReceivePacket) );
  Config::Connect ("/NodeList/6/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx", MakeCallback(&ReceivePacket) );
  Config::Connect ("/NodeList/7/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx", MakeCallback(&ReceivePacket) );
  Config::Connect ("/NodeList/8/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx", MakeCallback(&ReceivePacket) );
  Config::Connect ("/NodeList/9/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx", MakeCallback(&ReceivePacket) );
  Config::Connect ("/NodeList/10/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx", MakeCallback(&ReceivePacket) );
  Config::Connect ("/NodeList/11/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx", MakeCallback(&ReceivePacket) );
  Config::Connect ("/NodeList/12/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx", MakeCallback(&ReceivePacket) );
  Config::Connect ("/NodeList/13/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx", MakeCallback(&ReceivePacket) );
  Config::Connect ("/NodeList/14/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx", MakeCallback(&ReceivePacket) );
  Config::Connect ("/NodeList/15/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx", MakeCallback(&ReceivePacket) );

  //-----------------------------------------------------
  //-- Enable ASCII and PCAP tracing
  //-----------------------------------------------------
  AsciiTraceHelper ascii;
  wifiPhy.EnableAsciiAll (ascii.CreateFileStream("tcpWireless15Sink.tr"));
  wifiPhy.EnablePcapAll("tcpWireless15Sink");
  
  //-----------------------------------------------------
  //-- Trigger CalculateThroughput function, it will schedule itself afterwards
  //-----------------------------------------------------
  Simulator::Schedule (Seconds(0.0),&CalculateThroughput);
  Ptr<FlowMonitor>flowMonitor;
  FlowMonitorHelper flowHelper;
  flowMonitor = flowHelper.InstallAll();

  //-----------------------------------------------------
  //-- Schedule a stop time at 60s and start the simulation
  //-----------------------------------------------------
  Simulator::Stop (Seconds (60));
  Simulator::Run ();
  flowMonitor->SerializeToXmlFile("tcpWireless15Sink.xml", true, true);
  Simulator::Destroy ();
  
  return 0;
}
