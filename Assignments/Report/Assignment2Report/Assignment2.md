---
title: Assignment 2
authors: Michael Lenehan
left-header: EE452 Wireless/Mobile Communications
right-header: \today
---

# Introduction

The aim of this assignment is to analyse the 802.11ac Distributed Coordinator
Function MAC level implementation, and the throughput of the 802.11ac standard.
The throughput will be measured using both the TCP and UDP transfer protocols,
each using one source node and one sink node. In order to demonstrate the
effects on throughput of transmission to multiple clients, extra sink nodes are 
added to each example.

# Procedure

## Analysis of TCP and UDP

In order to analyse the difference between the throughput of 802.11ac in the TCP
and UDP protocols, two systems must be set up, one using the TCP protocol, and
one using the UDP protocol. Each system must have an equal number of nodes, an
equal number of sources, and an equal number of sinks. Sink nodes must be in the
same positions in both the TCP and UDP system.

For this section of the assignment, systems consisting of one source node and
one sink node were chosen. This allowed for the analysis to be completed without
the need to consider the effects of interference.

Due to an error caused by the RAA's available for use with HT rates in ns-3.29,
the FlowMonitor output XML file could not be obtained using ns-3.29. As such,
the provided code was modified and executed using ns-3.25.

### Setup for UDP Throughput Analysis

With the provided code, in order to correctly analyse the throughput, a
FlowMonitor object is created. This object is used to output an XML file,
containing all relevant information from the packet transfers between the source
and sink nodes.

### Setup for TCP Throughput Analysis

As with the UDP system, a FlowMonitor object must be created in order to output
an XML used for analysis. Also required in the TCP system is the addition of the
"Config::Connect()" method must be called, with the source node as an argument
to the method. This is due to the fact that the source also receives packets in
TCP, in the form of Acknowledgements.

## Impact of Load on Performance

The impact of load on the performance of the 802.11ac standard may be analysed
by adding multiple sink nodes to the TCP and UDP systems. For the purpose of
this assignment, it was chosen to use a 5 Sink, 10 Sink and 15 Sink system for
the comparison of each protocol. 

The provided "ReceivePacket()" and "CalculateThroughput()" methods are used
as arguments within "Config::Connect()" and Simulator::Schedule()" methods
respectively. However, adding multiple sinks, while calling the same methods for
each, will calculate total system throughput. In order to calculate throughput
separately for each sink, separate "ReceivePacket()" and "CalculateThroughput()"
methods must be implemented.

### Modified UDP with multiple Nodes Code Snippets

In order to add multiple sink nodes to the provided "udpWireless.cc" file, the
"Create" methods argument must be increased from 2 (one source and one sink) to
6, 11, or 16 for 5, 10 or 15 sink nodes respectively. Position vectors must then
be added for each of the new nodes. The distances used are as follows:

#### Position Vectors for 1 Source and 5 Sink Nodes

```cpp
positionAlloc -> Add (Vector (0.0, 0.0, 0.0));
positionAlloc -> Add (Vector (1.0, 0.0, 0.0));
positionAlloc -> Add (Vector (0.0, 1.0, 0.0));
positionAlloc -> Add (Vector (0.0, 0.0, 1.0));
positionAlloc -> Add (Vector (1.0, 1.0, 1.0));
positionAlloc -> Add (Vector (1.0, 0.0, 1.0));
```

#### Position Vectors for 1 Source and 10 Sink Nodes

```cpp
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
```

#### Position Vectors for 1 Source and 15 Sink Nodes

```cpp
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
```

In order to transmit data from the source to the sink node, an additional
"OnOffHelper" object must be created for each added sink node. The address of
the added sink node is passed as an argument to the "interfaces.GetAddress()"
method passed to the objects constructor. This "OnOffHelper" object is then set
to transmit from the source node (address 0) using the "Install(nodes.Get())"
method on the object.

#### Source Node Configuration for Additional Nodes

```cpp
OnOffHelper onoff3 ("ns3::TcpSocketFactory",
                   Address (InetSocketAddress (interfaces.GetAddress (3),
      	             port)));
```
\vdots
```cpp
ApplicationContainer apps3 = onoff3.Install (nodes.Get (0));
```

In order to create the additional sink nodes, "SinkHelper" objects must be
initialised, and then installed on the node at the required address.

#### Sink Node Configuration for Additional Nodes

```cpp
PacketSinkHelper sinkHelper3 ("ns3::TcpSocketFactory",
                             InetSocketAddress (Ipv4Address::GetAny (),
    			       port));
ApplicationContainer sinkApp3 = sinkHelper3.Install (nodes.Get(3));
```

Additional "Config::Connect()" methods are required, with the additional sink
nodes passed in as arguments.

#### "Config::Connect()" Method for Added Nodes (5 Sink Example)

```cpp
Config::Connect ("/NodeList/1/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx",
  MakeCallback(&ReceivePacket) );
Config::Connect ("/NodeList/2/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx",
  MakeCallback(&ReceivePacket) );
Config::Connect ("/NodeList/3/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx",
  MakeCallback(&ReceivePacket) );
Config::Connect("/NodeList/4/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx",
  MakeCallback(&ReceivePacket) );
Config::Connect("/NodeList/5/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx",
  MakeCallback(&ReceivePacket) );
```

### Modified TCP with multiple Nodes Code Snippets

In order to add multiple sink nodes to the provided "tcpWireless.cc" file, the
"Create" methods argument must be increased from 2 (one source and one sink) to
6, 11, or 16 for 5, 10 or 15 sink nodes respectively. Position vectors must then
be added for each of the new nodes. The distances used are as follows:

#### Position Vectors for 1 Source and 5 Sink Nodes

```cpp
positionAlloc -> Add (Vector (0.0, 0.0, 0.0));
positionAlloc -> Add (Vector (1.0, 0.0, 0.0));
positionAlloc -> Add (Vector (0.0, 1.0, 0.0));
positionAlloc -> Add (Vector (0.0, 0.0, 1.0));
positionAlloc -> Add (Vector (1.0, 1.0, 1.0));
positionAlloc -> Add (Vector (1.0, 0.0, 1.0));
```

#### Position Vectors for 1 Source and 10 Sink Nodes

```cpp
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
```

#### Position Vectors for 1 Source and 15 Sink Nodes

```cpp
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
```

As can be seen in the code snippets, the position vectors used for the TCP
systems are the same as those used in the UDP systems.

The required modifications for the TCP system code are the same as those made in
the UDP system code. One difference exists however, in that a
"Config::Connect()" method must be added with the source node (at address 0) as
the argument:

#### "Config::Connect()" Method for Added Nodes (5 Sink Example)

```cpp
Config::Connect ("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx",
  MakeCallback(&ReceivePacket) );
Config::Connect ("/NodeList/1/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx",
  MakeCallback(&ReceivePacket) );
Config::Connect ("/NodeList/2/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx",
 MakeCallback(&ReceivePacket) );
Config::Connect("/NodeList/3/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx",
  MakeCallback(&ReceivePacket) );
Config::Connect ("/NodeList/4/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx",
  MakeCallback(&ReceivePacket) );
Config::Connect ("/NodeList/5/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx",
  MakeCallback(&ReceivePacket) );
```

# Results

## Analysis of TCP and UDP

### Listed 802.11ac data rates

The advertised physical layer data rate of 802.11ac is approximately 7Gbps
(at 5GHz). Single users may achieve MAC throughput of 500Mbps, with multiple
users achieving rates of 1Gbps in total [@inproceedings]. There are a number of
factors within the protocol which influence the increases in throughput from
the previous standards. Enhancements have been made to a number of the 802.11n
standards specifications, with channel bonding (combining channels to increase
throughput) being implemented both statically and dynamically, and the
implementation of MU-MIMO, allowing for the increased data rates. Beamforming
for MU-MIMO is also a specification of the 802.11ac standard, and the number
of spatial streams, i.e. antennas, increased to eight from the previous standard
of four [@Bejarano2013].

### Obtained Throughput

The following figure displays the throughput graph for a system using the TCP
protocol, consisting of a single source node, and a single sink node.

\begin{figure}[H]
\includegraphics{Images/tcp1SinkTPGraph.png}
\centering
\caption{Total Throughput for TCP System with 1 source and 1 Sink Node}
\label{}
\end{figure}

The following figure displays the throughput graph for a system using the UDP
protocol, consisting of a single source node, and a single sink node.

\begin{figure}[H]
\includegraphics{Images/udp1SinkTPGraph.png}
\centering
\caption{Total Throughput for UDP System with 1 Source and 1 Sink Node}
\label{}
\end{figure}

The throughput represented in the above graphs is for the total of the systems,
i.e. the average across all nodes at the sampled point in time. As such the
separate throughput for each node must be calculated separately, using the
output XML from the FlowMonitor.

The obtained throughput for each of the above was calculated using the following
formula:

\begin{center}
$Throughput = \frac{rxBytes\times8}{timeLastRxPacket-timeFirstRxPacket]}$
\end{center}

The output values obtained for the TCP system were as follows:

#### Source Node

$Throughput = 0.0215Mbps$

#### Sink Node

$Throughput = 4.68Mbps$

The output values obtained for the UDP system were as follows:

#### Sink Node

$Throughput = 5.62Mbps$

### UDP Deviation

Deviations in UDP throughput may be cause by a number of issues. Distance from
the source node will cause a change in throughput. 802.11ac is highly susceptible
to decreases in throughput due to increasing distances. As the sink node in the
code is 5 meters from the source, this may be the main cause for the deviation
between the advertised and simulated throughput.

### TCP Deviation

Deviations in TCP throughput may be caused by a number of issues. As with UDP,
distance from the source node will cause a change in throughput. The sink node
is placed at the same distance from the source node as in the UDP system, which,
again, may be the main cause of deviation between the advertised and simulated
throughput.

### Trace

The following timing diagram was developed using Wireshark, and the output trace
file from the UDP system described above:

\begin{figure}[H]
\includegraphics{Images/udpTimingDiagram.png}
\centering
\caption{Wireshark Timing Diagram for the transmission of UDP packets}
\label{}
\end{figure}

The transmission of a single 1024 byte UDP packet can be seen in the timing
diagram below, with the transmission (Tx) and receive (Rx) times listed below
the source and sink nodes.

\begin{figure}[H]
\includegraphics{Images/udpTimingDiagram2.png}
\centering
\caption{Timing Diagram for the transmission of one 1024 byte UDP packet}
\label{}
\end{figure}

Distributed Coordination Function (DCF) is enabled by default within ns-3.25 for
802.11ac UDP packets. DCF allows for collision avoidance, using Request-To-Send
and Clear-To-Send (RTS/CTS), which allows each node monitor the channel for
transmissions, and, when the channel is idle, transmit data[@Chang2012].

## Impact on Load on Performance

### Measured TCP and UDP Throughput with Multiple Nodes

#### TCP 5 Sinks

The following figure displays the total throughput graph for a system using the
TCP protocol, consisting of a single source node, and five sink nodes.

\begin{figure}[H]
\includegraphics{Images/tcp5SinkTPGraph.png}
\centering
\caption{Total Throughput for TCP System with 1 Source and 5 Sink Nodes}
\label{}
\end{figure}

The individual node throughputs were calculated as shown below:

#### Sink Node 1

$Throughput = 0.07832Mbps$

#### Sink Node 2

$Throughput = 0.00004Mbps$

#### Sink Node 3

$Throughput = 4.724Mbps$

#### Sink Node 4

$Throughput = 0.73Mbps$

#### Sink Node 5

$Throughput = 0.0772Mbps$

#### Source Node

$Average Throughput = 0.0455Mbps$

#### TCP 10 Sinks

The following figure displays the total throughput graph for a system using the
TCP protocol, consisting of a single source node, and ten sink node.

\begin{figure}[H]
\includegraphics{Images/tcp10SinkTPGraph.png}
\centering
\caption{Total Throughput for TCP System with 1 Source and 10 Sink Nodes}
\label{}
\end{figure}

The individual node throughputs were calculated as shown below:

#### Sink Node 1

$Throughput = 0.0748Mbps$

#### Sink Node 2

$Throughput = 4.7156Mbps$

#### Sink Node 3

$Throughput = 0.0722Mbps$

#### Sink Node 4

$Throughput = 0.0722Mbps$

#### Sink Node 5

$Throughput = 0.0723Mbps$

#### Sink Node 6

$Throughput = 0.00004Mbps$

#### Sink Node 7

$Throughput = 0.00004Mbps$

#### Sink Node 8

$Throughput = 0.00004Mbps$

#### Sink Node 9

$Throughput = 0.0904Mbps$

#### Sink Node 10

$Throughput = 0.00004Mbps$

#### Source Node

$Average Throughput =0.0234 Mbps$

#### TCP 15 Sinks

The following figure displays the total throughput graph for a system using the
TCP protocol, consisting of a single source node, and fifteen sink node.

\begin{figure}[H]
\includegraphics{Images/tcp15SinkTPGraph.png}
\centering
\caption{Total Throughput for TCP System with 1 Source and 15 Sink Nodes}
\label{}
\end{figure}

The individual node throughputs were calculated as shown below:

#### Sink Node 1

$Throughput = 0.0722Mbps$

#### Sink Node 2

$Throughput = 4.653Mbps$

#### Sink Node 3

$Throughput = 0.0729Mbps$

#### Sink Node 4

$Throughput = 0.0672Mbps$

#### Sink Node 5

$Throughput = 0.0653Mbps$

#### Sink Node 6

$Throughput = 0.07Mbps$

#### Sink Node 7

$Throughput = 0.066Mbps$

#### Sink Node 8

$Throughput = 0.0691Mbps$

#### Sink Node 9

$Throughput = 0.00004Mbps$

#### Sink Node 10

$Throughput = 0.069Mbps$

#### Sink Node 11

$Throughput = 0.069Mbps$

#### Sink Node 12

$Throughput = 0.065Mbps$

#### Sink Node 13

$Throughput = 0.00004Mbps$

#### Sink Node 14

$Throughput = 0.066Mbps$

#### Sink Node 15

$Throughput = 0.07Mbps$

#### Source Node

$Average Throughput = 0.017Mbps$

#### UDP 5 Sinks

The following figure displays the total throughput graph for a system using the
UDP protocol, consisting of a single source node, and five sink node.

\begin{figure}[H]
\includegraphics{Images/udp5SinkTPGraph.png}
\centering
\caption{Total Throughput for UDP System with 1 Source and 5 Sink Nodes}
\label{}
\end{figure}

The individual node throughputs were calculated as shown below:

#### Sink Node 1

$Throughput = 0Mbps$

#### Sink Node 2

$Throughput = 5.62Mbps$

#### Sink Node 3

$Throughput = 0Mbps$

#### Sink Node 4

$Throughput = 0Mbps$

#### Sink Node 5

$Throughput = 0.598Mbps$

#### UDP 10 Sinks

The following figure displays the total throughput graph for a system using the
UDP protocol, consisting of a single source node, and ten sink node.

\begin{figure}[H]
\includegraphics{Images/udp10SinkTPGraph.png}
\centering
\caption{Total Throughput for UDP System with 1 Source and 10 Sink Nodes}
\label{}
\end{figure}

The individual node throughputs were calculated as shown below:

#### Sink Node 1

$Throughput = 2.3Mbps$

#### Sink Node 2

$Throughput = 1.68Mbps$

#### Sink Node 3

$Throughput = 1.61Mbps$

#### Sink Node 4

$Throughput = 0.454Mbps$

#### Sink Node 5

$Throughput = 5.9Mbps$

#### Sink Node 6 

$Throughput = 0.1Mbps$

#### Sink Node 7

$Throughput = 5.46Mbps$

#### Sink Node 8

$Throughput = 0Mbps$

#### Sink Node 9

$Throughput = 0Mbps$

#### Sink Node 10

$Throughput = 0Mbps$

#### UDP 15 Sinks

The following figure displays the total throughput graph for a system using the
UDP protocol, consisting of a single source node, and fifteen sink node.

\begin{figure}[H]
\includegraphics{Images/udp15SinkTPGraph.png}
\centering
\caption{Total Throughput for UDP System with 1 Source and 15 Sink Nodes}
\label{}
\end{figure}

The individual node throughputs were calculated as shown below:

#### Sink Node 1

$Throughput = 2.1Mbps$

#### Sink Node 2

$Throughput = 1.61Mbps$

#### Sink Node 3

$Throughput = 1.6Mbps$

#### Sink Node 4

$Throughput = 0.517Mbps$

#### Sink Node 5

$Throughput = 6.03Mbps$

#### Sink Node 6 

$Throughput = 0.085Mbps$

#### Sink Node 7

$Throughput = 0Mbps$

#### Sink Node 8

$Throughput = 5.73Mbps$

#### Sink Node 9

$Throughput = 3.04Mbps$

#### Sink Node 10

$Throughput = 0Mbps$

#### Sink Node 11

$Throughput = 5.77Mbps$

#### Sink Node 12

$Throughput = 0Mbps$

#### Sink Node 13

$Throughput = 5.77Mbps$

#### Sink Node 14

$Throughput = 0Mbps$

#### Sink Node 15

$Throughput = 0Mbps$

### Comparison of Results

#### 5 Sink

The highest throughput experienced in the TCP system was approximately 4.7Mbps,
with the lowest throughput experienced being 0.00004Mbps. The highest throughput
in the UDP system was approximately 5.6Mbps, with the lowest throughput
experienced being 0Mbps. While the highest throughput experienced was in the UDP
system, there were three nodes in the UDP system which had zero throughput. The
TCP system experienced better throughput per node than the UDP system.

The total throughput of the TCP system was approximately 5.7Mbps, giving an
average throughput of 0.95Mbps. The total throughput of the UDP system was
approximately 6.2Mbps, giving an average throughput of 1.03Mbps.

#### 10 Sink

The highest throughput experienced in the TCP system was approximately 4.72Mbps,
with the lowest throughput experienced being 0.00004Mbps. The highest throughput
in the UDP system was approximately 5.9Mbps, with the lowest throughput
experienced being 0Mbps. The maximum experienced throughput in UDP was 1.18Mbps
higher than that in the TCP system. Again, the TCP system had more consistent
throughput, with the UDP system having 0Mbps on three nodes.

The total throughput of the TCP system was approximately 5.05Mbps, giving an
average throughput of 0.46Mbps. The total throughput of the UDP system was
approximately 17.5Mbps, giving an average of throughput of 1.59Mbps.

#### 15 Sink

The highest throughput experienced in the TCP system was approximately 4.7Mbps,
with the lowest throughput experienced being 0.00004Mbps. The highest throughput
in the UDP system was approximately 6.03Mbps, with the lowest throughput
experienced being 0Mbps. The maximum throughput in the UDP system was 1.31Mbps
higher than in the TCP system, however, the TCP system experienced more
consistent throughput, with the UDP system having 0Mbps on five nodes.

The total throughput of the TCP system was approximately 5.4Mbps, giving an
average throughput of 0.34Mbps. The total throughput of the UDP system was
approximately 32.3Mbps, giving an average throughput of 2.02Mbps.


It can be seen that the number of nodes in a system causes more nodes in the UDP
system to receive no throughput, while in the TCP system, increases the number
of nodes receiving very low throughput. As can be seen from the average
throughput per node, the TCP system reduces as more nodes are added. The UDP
system average throughput increases as more nodes are added, however, this is
due to certain nodes having higher throughput, however, there is less
consistency to the throughput, as mentioned above, with an increasing number of
nodes receiving no throughput.

# Conclusion

Within the single node systems, it can be seen that there is higher throughput
experienced within the UDP system. The lower throughput of the TCP system  may
be due to the acknowledgements which must be transmit from the sink to the
source node. There are also a number of factors, such as distance or different
forms of interference which cause the simulated throughput to fall below the
advertised throughputs for 802.11ac.

From this results shown above, it is obvious that the number of sink nodes has
an impact on the throughput of the system. Increasing the number of nodes in the
TCP system reduces the average throughput, however the total throughput of the
system remains relatively constant. Within the UDP system, both the total and
average throughput increases, however, there is no  consistency to which nodes
receive data.

The lack of throughput for certain nodes of the TCP and UDP systems may be
partially explained by the collision avoidance utilised in the 802.11ac
standard. This may be blocking transmission to nodes while transmitting to
others.

# Appendix

## Code

The code used in the completion of this assignment can be found in the .zip file
containing this report, or at the following address:
<https://github.com/mLenehanDCU/EE452/tree/master/Assignments/Code/Assignment2>

## Throughput Calculations

All values used in the following throughput calculations can be found in the
.zip file containing this report, or at the following address:
<https://github.com/mLenehanDCU/EE452/tree/master/Assignments/Data/Assignment2>

### TCP

#### TCP 1 Node

##### Source Node

\begin{center}
$rxBytes = 221680$

$timeLastRxPacket = \num{10247203463e-9}$

$timeFirstRxPacket = \num{2016057160e-9}$

$Throughput = \num{215454.8}(bps)$
\end{center}

##### Sink Node

\begin{center}
$rxBytes = 4821824$

$timeLastRxPacket = \num{10245900448e-9}$

$timeFirstRxPacket = \num{2009436112e-9}$

$Throughput = \num{4683392.1}(bps)$
\end{center}

#### TCP 5 Nodes

##### Sink Node 1

\begin{center}
$rxBytes = 147636$

$timeLastRxPacket = \num{18011184774e-9}$

$timeFirstRxPacket = \num{2014264069e-9}$

$Throughput = \num{73832.20945}(bps)$
\end{center}

##### Sink Node 2

\begin{center}
$rxBytes = 224$

$timeLastRxPacket = \num{47000152003e-9}$

$timeFirstRxPacket = \num{2005074053e-9}$

$Throughput = \num{39.82657841}(bps)$
\end{center}

##### Sink Node 3

\begin{center}
$rxBytes = 4862432$

$timeLastRxPacket = \num{10241978392e-9}$

$timeFirstRxPacket = \num{2007382021e-9}$

$Throughput = \num{4723905.611}(bps)$
\end{center}

##### Sink Node 4

\begin{center}
$rxBytes = 145340$

$timeLastRxPacket = \num{17944828787e-9}$

$timeFirstRxPacket = \num{2024382139e-9}$

$Throughput = \num{73033.12688}(bps)$
\end{center}

##### Sink Node 5

\begin{center}
$rxBytes = 152056$

$timeLastRxPacket = \num{17765945047e-9}$

$timeFirstRxPacket = \num{2003436028e-9}$

$Throughput = \num{77173.50065}(bps)$
\end{center}

##### Source Node

\begin{center}
$rxBytes1 = 6868$

$rxBytes2 = 0$

$rxBytes3 = 223552$

$rxBytes4 = 6712$

$rxBytes5 = 6556$

$timeLastRxPacket1 = \num{17747449044e-9}$

$timeLastRxPacket2 = \num{0.0e-9}$

$timeLastRxPacket3 = \num{10243938080e-9}$

$timeLastRxPacket4 = \num{18010222772e-9}$

$timeLastRxPacket5 = \num{17924059780e-9}$

$timeFirstRxPacket1 = \num{2014752080e-9}$

$timeFirstRxPacket2 = \num{0.0e-9}$

$timeFirstRxPacket3 = \num{2008976030e-9}$

$timeFirstRxPacket4 = \num{2023724124e-9}$

$timeFirstRxPacket5 = \num{2046052184e-9}$

$Throughput1 = \num{3492.344645}(bps)$

$Throughput2 = \num{0}(bps)$

$Throughput3 = \num{217173.557}(bps)$

$Throughput4 = \num{3358.8343}(bps)$

$Throughput5 = \num{3303.185219}(bps)$
\end{center}

#### TCP 10 Node

##### Sink Node 1

\begin{center}
$rxBytes = 152056$

$timeLastRxPacket = \num{18274599526e-9}$

$timeFirstRxPacket = \num{2011658214e-9}$

$Throughput = \num{74798.76959}(bps)$
\end{center}

##### Sink Node 2

\begin{center}
$rxBytes = 4850024$

$timeLastRxPacket = \num{10233756865e-9}$

$timeFirstRxPacket = \num{2005641069e-9}$

$Throughput = \num{4715562.221}(bps)$
\end{center}

##### Sink Node 3

\begin{center}
$rxBytes = 148724$

$timeLastRxPacket = \num{18495646619e-9}$

$timeFirstRxPacket = \num{2016226308e-9}$

$Throughput = \num{72198.65611}(bps)$
\end{center}

##### Sink Node 4

\begin{center}
$rxBytes = 148724$

$timeLastRxPacket = \num{18494739612e-9}$

$timeFirstRxPacket = \num{2014044268e-9}$

$Throughput = \num{72193.07045}(bps)$
\end{center}

##### Sink Node 5

\begin{center}
$rxBytes = 148724$

$timeLastRxPacket = \num{18473200603e-9}$

$timeFirstRxPacket = \num{2016947323e-9}$

$Throughput = \num{72300.29702}(bps)$
\end{center}

##### Sink Node 6

\begin{center}
$rxBytes = 224$

$timeLastRxPacket = \num{47000152004e-9}$

$timeFirstRxPacket = \num{2012460229e-9}$

$Throughput = \num{39.83311722}(bps)$
\end{center}

##### Sink Node 7

\begin{center}
$rxBytes = 224$

$timeLastRxPacket = \num{47000434012e-9}$

$timeFirstRxPacket = \num{2003436028e-9}$

$Throughput = \num{39.824879}(bps)$
\end{center}

##### Sink Node 8

\begin{center}
$rxBytes = 224$

$timeLastRxPacket = \num{47000707021e-9}$

$timeFirstRxPacket = \num{2014801288e-9}$

$Throughput = \num{39.83469869}(bps)$
\end{center}

##### Sink Node 9

\begin{center}
$rxBytes = 148724$

$timeLastRxPacket = \num{18171333499e-9}$

$timeFirstRxPacket = \num{2017596342e-9}$

$Throughput = \num{90452.77291}(bps)$
\end{center}

##### Sink Node 10

\begin{center}
$rxBytes = 224$

$timeLastRxPacket = \num{47001061031e-9}$

$timeFirstRxPacket = \num{2021719407e-9}$

$Throughput = \num{39.84051201}(bps)$
\end{center}

##### Source Node

\begin{center}
$rxBytes1 = 0$

$rxBytes2 = 222928$

$rxBytes3 = 6868$

$rxBytes4 = 0$

$rxBytes5 = 6712$

$rxBytes6 = 0$

$rxBytes7 = 6712$

$rxBytes8 = 6712$

$rxBytes9 = 6712$

$rxBytes10 = 0$

$timeLastRxPacket1 = \num{0.0e-9}$

$timeLastRxPacket2 = \num{10234208867e-9}$

$timeLastRxPacket3 = \num{18170949495e-9}$

$timeLastRxPacket4 = \num{0.0e-9}$

$timeLastRxPacket5 = \num{18473434611e-9}$

$timeLastRxPacket6 = \num{0.0e-9}$

$timeLastRxPacket7 = \num{18494394607e-9}$

$timeLastRxPacket8 = \num{18457199585e-9}$

$timeLastRxPacket9 = \num{18106577503e-9}$

$timeLastRxPacket10 = \num{0.0e-9}$

$timeFirstRxPacket1 = \num{0.0e-9}$

$timeFirstRxPacket2 = \num{2015379299e-9}$

$timeFirstRxPacket3 = \num{2022198418e-9}$

$timeFirstRxPacket4 = \num{0.0e-9}$

$timeFirstRxPacket5 = \num{2028614480e-9}$

$timeFirstRxPacket6 = \num{0.0e-9}$

$timeFirstRxPacket7 = \num{2028287470e-9}$

$timeFirstRxPacket8 = \num{2026280459e-9}$

$timeFirstRxPacket9 = \num{2025492447e-9}$

$timeFirstRxPacket10 = \num{0.0e-9}$

$Throughput1 = \num{0}(bps)$

$Throughput2 = \num{216992.4544}(bps)$

$Throughput3 = \num{3402.368377}(bps)$

$Throughput4 = \num{0}(bps)$

$Throughput5 = \num{3265.222701}(bps)$

$Throughput6 = \num{0}(bps)$

$Throughput7 = \num{3261.001496}(bps)$

$Throughput8 = \num{3267.985168}(bps)$

$Throughput9 = \num{3339.078166}(bps)$

$Throughput10 = \num{0}(bps)$
\end{center}

#### TCP 15 Nodes

##### Sink Node 1

\begin{center}
$rxBytes = 152056$

$timeLastRxPacket = \num{18861420105e-9}$

$timeFirstRxPacket = \num{2021949484e-9}$

$Throughput = \num{72237.90031}(bps)$
\end{center}

##### Sink Node 2

\begin{center}
$rxBytes = 4775576$

$timeLastRxPacket = \num{10215291769e-9}$

$timeFirstRxPacket = \num{2003983077e-9}$

$Throughput = \num{4652681.982}(bps)$
\end{center}

##### Sink Node 3

\begin{center}
$rxBytes = 148724$

$timeLastRxPacket = \num{18341489046e-9}$

$timeFirstRxPacket = \num{2028024648e-9}$

$Throughput = \num{72933.12879}(bps)$
\end{center}

##### Sink Node 4

\begin{center}
$rxBytes = 152056$

$timeLastRxPacket = \num{20136770746e-9}$

$timeFirstRxPacket = \num{2022823502e-9}$

$Throughput = \num{67155.32422}(bps)$
\end{center}

##### Sink Node 5

\begin{center}
$rxBytes = 148764$

$timeLastRxPacket = \num{20249145860e-9}$

$timeFirstRxPacket = \num{2026770625e-9}$

$Throughput = \num{65310.47598}(bps)$
\end{center}

##### Sink Node 6

\begin{center}
$rxBytes = 157696$

$timeLastRxPacket = \num{20137902760e-9}$

$timeFirstRxPacket = \num{2010512304e-9}$

$Throughput = \num{69594.57309}(bps)$
\end{center}

##### Sink Node 7

\begin{center}
$rxBytes = 148724$

$timeLastRxPacket = \num{20061665667e-9}$

$timeFirstRxPacket = \num{2029296673e-9}$

$Throughput = \num{65980.90358}(bps)$
\end{center}

##### Sink Node 8

\begin{center}
$rxBytes = 157736$

$timeLastRxPacket = \num{20283688899e-9}$

$timeFirstRxPacket = \num{2017822384e-9}$

$Throughput = \num{69084.48603}(bps)$
\end{center}

##### Sink Node 9

\begin{center}
$rxBytes = 224$

$timeLastRxPacket = \num{47000152005e-9}$

$timeFirstRxPacket = \num{2001436035e-9}$

$Throughput = \num{39.82335854}(bps)$
\end{center}

##### Sink Node 10

\begin{center}
$rxBytes = 157696$

$timeLastRxPacket = \num{20337504967e-9}$

$timeFirstRxPacket = \num{2018543404e-9}$

$Throughput = \num{68866.78569}(bps)$
\end{center}

##### Sink Node 11

\begin{center}
$rxBytes = 157736$

$timeLastRxPacket = \num{20336945955e-9}$

$timeFirstRxPacket = \num{2012159344e-9}$

$Throughput = \num{68862.35713}(bps)$
\end{center}

##### Sink Node 12

\begin{center}
$rxBytes = 148764$

$timeLastRxPacket = \num{20203527820e-9}$

$timeFirstRxPacket = \num{2024756568e-9}$

$Throughput = \num{65467.13106}(bps)$
\end{center}

##### Sink Node 13

\begin{center}
$rxBytes = 224$

$timeLastRxPacket = \num{47000425017e-9}$

$timeFirstRxPacket = \num{2014909455e-9}$

$Throughput = \num{39.83504418}(bps)$
\end{center}

##### Sink Node 14

\begin{center}
$rxBytes = 152056$

$timeLastRxPacket = \num{20338130978e-9}$

$timeFirstRxPacket = \num{2021273468e-9}$

$Throughput = \num{6641.39176}(bps)$
\end{center}

##### Sink Node 15

\begin{center}
$rxBytes = 157696$

$timeLastRxPacket = \num{20136002728e-9}$

$timeFirstRxPacket = \num{2014042420e-9}$

$Throughput = \num{69615.42673}(bps)$
\end{center}

##### Source Node

\begin{center}
$rxBytes1 = 0$

$rxBytes2 = 219548$

$rxBytes3 = 7128$

$rxBytes4 = 7180$

$rxBytes5 = 7128$

$rxBytes6 = 0$

$rxBytes7 = 7180$

$rxBytes8 = 7128$

$rxBytes9 = 6868$

$rxBytes10 = 6868$

$rxBytes11 = 6868$

$rxBytes12 = 6764$

$rxBytes13 = 6764$

$rxBytes14 = 6712$

$rxBytes15 = 6712$

$timeLastRxPacket1 = \num{0.0e-9}$

$timeLastRxPacket2 = \num{10215826771e-9}$

$timeLastRxPacket3 = \num{19940296569e-9}$

$timeLastRxPacket4 = \num{20311571935e-9}$

$timeLastRxPacket5 = \num{19868978577e-9}$

$timeLastRxPacket6 = \num{0.0e-9}$

$timeLastRxPacket7 = \num{20169734778e-9}$

$timeLastRxPacket8 = \num{20328542941e-9}$

$timeLastRxPacket9 = \num{20336256948e-9}$

$timeLastRxPacket10 = \num{18756475991e-9}$

$timeLastRxPacket11 = \num{19879104578e-9}$

$timeLastRxPacket12 = \num{20061182664e-9}$

$timeLastRxPacket13 = \num{20113158689e-9}$

$timeLastRxPacket14 = \num{18245016940e-9}$

$timeLastRxPacket15 = \num{19735197626e-9}$

$timeFirstRxPacket1 = \num{0.0e-9}$

$timeFirstRxPacket2 = \num{2023320513e-9}$

$timeFirstRxPacket3 = \num{2025885599e-9}$

$timeFirstRxPacket4 = \num{2025361591e-9}$

$timeFirstRxPacket5 = \num{2023889541e-9}$

$timeFirstRxPacket6 = \num{0.0e-9}$

$timeFirstRxPacket7 = \num{2028566661e-9}$

$timeFirstRxPacket8 = \num{2027249639e-9}$

$timeFirstRxPacket9 = \num{2036703733e-9}$

$timeFirstRxPacket10 = \num{2033011684e-9}$

$timeFirstRxPacket11 = \num{2040222769e-9}$

$timeFirstRxPacket12 = \num{2052118856e-9}$

$timeFirstRxPacket13 = \num{2051531842e-9}$

$timeFirstRxPacket14 = \num{2042882780e-9}$

$timeFirstRxPacket15 = \num{2056135886e-9}$

$Throughput1 = \num{0}(bps)$

$Throughput2 = \num{214389.0947}(bps)$

$Throughput3 = \num{3183.135638}(bps)$

$Throughput4 = \num{3141.164786}(bps)$

$Throughput5 = \num{3195.501008}(bps)$

$Throughput6 = \num{0}(bps)$

$Throughput7 = \num{3166.27902}(bps)$

$Throughput8 = \num{3115.84537}(bps)$

$Throughput9 = \num{3002.477676}(bps)$

$Throughput10 = \num{3285.443673}(bps)$

$Throughput11 = \num{3080.01368}(bps)$

$Throughput12 = \num{3004.709216}(bps)$

$Throughput13 = \num{2995.964896}(bps)$

$Throughput14 = \num{3314.131303}(bps)$

$Throughput15 = \num{3037.265257}(bps)$
\end{center}

### UDP

#### UDP 1 Node

##### Sink Node

\begin{center}
$rxBytes = 5962736$

$timeLastRxPacket = \num{10499950632e-9}$

$timeFirstRxPacket = \num{2011588311e-9}$

$Throughput = \num{5619680.946}(bps)$
\end{center}

#### UDP 5 Nodes

##### Sink Node 1

\begin{center}
$rxBytes = 0$

$timeLastRxPacket = \num{0.0e-9}$

$timeFirstRxPacket = \num{0.0e-9}$

$Throughput = \num{0}(bps)$
\end{center}

##### Sink Node 2

\begin{center}
$rxBytes = 5282092$

$timeLastRxPacket = \num{10350144473e-9}$

$timeFirstRxPacket = \num{2830805797e-9}$

$Throughput = \num{5619741.02}(bps)$
\end{center}

##### Sink Node 3

\begin{center}
$rxBytes = 0$

$timeLastRxPacket = \num{0.0e-9}$

$timeFirstRxPacket = \num{0.0e-9}$

$Throughput = \num{0}(bps)$
\end{center}

##### Sink Node 4

\begin{center}
$rxBytes = 0$

$timeLastRxPacket = \num{0.0e-9}$

$timeFirstRxPacket = \num{0.0e-9}$

$Throughput = \num{0}(bps)$
\end{center}

##### Sink Node 5

\begin{center}
$rxBytes = 629096$

$timeLastRxPacket = \num{10422673852e-9}$

$timeFirstRxPacket = \num{2005588227e-9}$

$Throughput = \num{597922.8707}(bps)$
\end{center}

#### UDP 10 Nodes

##### Sink Node 1

\begin{center}
$rxBytes = 2158704$

$timeLastRxPacket = \num{10345247176e-9}$

$timeFirstRxPacket = \num{2838753882e-9}$

$Throughput = \num{2300625.781}(bps)$
\end{center}

##### Sink Node 2

\begin{center}
$rxBytes = 1696876$

$timeLastRxPacket = \num{10429399243e-9}$

$timeFirstRxPacket = \num{2343259613e-9}$

$Throughput = \num{1678799.603}(bps)$
\end{center}

##### Sink Node 3

\begin{center}
$rxBytes = 1331832$

$timeLastRxPacket = \num{10109976026e-9}$

$timeFirstRxPacket = \num{3509337193e-9}$

$Throughput = \num{1614185.577}(bps)$
\end{center}

##### Sink Node 4

\begin{center}
$rxBytes = 368200$

$timeLastRxPacket = \num{10218544119e-9}$

$timeFirstRxPacket = \num{3731402303e-9}$

$Throughput = \num{454067.4589}(bps)$
\end{center}

##### Sink Node 5

\begin{center}
$rxBytes = 19988$

$timeLastRxPacket = \num{10375109541e-9}$

$timeFirstRxPacket = \num{10348026523e-9}$

$Throughput = \num{5904216.436}(bps)$
\end{center}

##### Sink Node 6 

\begin{center}
$rxBytes = 98888$

$timeLastRxPacket = \num{10396974893e-9}$

$timeFirstRxPacket = \num{2718324791e-9}$

$Throughput = \num{103026.4421}(bps)$
\end{center}

##### Sink Node 7

\begin{center}
$rxBytes = 225128$

$timeLastRxPacket = \num{2335194470e-9}$

$timeFirstRxPacket = \num{2005588227e-9}$

$Throughput = \num{5464168.347}(bps)$
\end{center}

##### Sink Node 8

\begin{center}
$rxBytes = 0$

$timeLastRxPacket = \num{0.0e-9}$

$timeFirstRxPacket = \num{0.0e-9}$

$Throughput = \num{0}(bps)$
\end{center}

##### Sink Node 9

\begin{center}
$rxBytes = 0$

$timeLastRxPacket = \num{0.0e-9}$

$timeFirstRxPacket = \num{0.0e-9}$

$Throughput = \num{0}(bps)$
\end{center}

##### Sink Node 10

\begin{center}
$rxBytes = 0$

$timeLastRxPacket = \num{0.0e-9}$

$timeFirstRxPacket = \num{0.0e-9}$

$Throughput = \num{0}(bps)$
\end{center}

#### UDP 15 Nodes

##### Sink Node 1

\begin{center}
$rxBytes = 2045088$

$timeLastRxPacket = \num{10371878030e-9}$

$timeFirstRxPacket = \num{2537698188e-9}$

$Throughput = \num{2088374.831}(bps)$
\end{center}

##### Sink Node 2

\begin{center}
$rxBytes = 1614820$

$timeLastRxPacket = \num{10033606514e-9}$

$timeFirstRxPacket = \num{2022176564e-9}$

$Throughput = \num{1612516.128}(bps)$
\end{center}

##### Sink Node 3

\begin{center}
$rxBytes = 1444396$

$timeLastRxPacket = \num{10205138589e-9}$

$timeFirstRxPacket = \num{2960660382e-9}$

$Throughput = \num{1595031.094}(bps)$
\end{center}

##### Sink Node 4

\begin{center}
$rxBytes = 457620$

$timeLastRxPacket = \num{10275035984e-9}$

$timeFirstRxPacket = \num{3193419499e-9}$

$Throughput = \num{516966.7134}(bps)$
\end{center}

##### Sink Node 5

\begin{center}
$rxBytes = 14728$

$timeLastRxPacket = \num{10394119389e-9}$

$timeFirstRxPacket = \num{10374566044e-9}$

$Throughput = \num{6025772.061}(bps)$
\end{center}

##### Sink Node 6 

\begin{center}
$rxBytes = 87316$

$timeLastRxPacket = \num{10412710066e-9}$

$timeFirstRxPacket = \num{2216792806e-9}$

$Throughput = \num{85228.77646}(bps)$
\end{center}

##### Sink Node 7

\begin{center}
$rxBytes = 0$

$timeLastRxPacket = \num{0.0e-9}$

$timeFirstRxPacket = \num{0.0e-9}$

$Throughput = \num{0}(bps)$
\end{center}

##### Sink Node 8

\begin{center}
$rxBytes = 62068$

$timeLastRxPacket = \num{2535602174e-9}$

$timeFirstRxPacket = \num{2448903103e-9}$

$Throughput = \num{5727212.463}(bps)$
\end{center}

##### Sink Node 9

\begin{center}
$rxBytes = 78900$

$timeLastRxPacket = \num{2210883695e-9}$

$timeFirstRxPacket = \num{2003588234e-9}$

$Throughput = \num{3044929.189}(bps)$
\end{center}

##### Sink Node 10

\begin{center}
$rxBytes = 0$

$timeLastRxPacket = \num{0.0e-9}$

$timeFirstRxPacket = \num{0.0e-9}$

$Throughput = \num{0}(bps)$
\end{center}

##### Sink Node 11

\begin{center}
$rxBytes = 44184$

$timeLastRxPacket = \num{2384957981e-9}$

$timeFirstRxPacket = \num{2323677899e-9}$

$Throughput = \num{5768138.496}(bps)$
\end{center}

##### Sink Node 12

\begin{center}
$rxBytes = 0$

$timeLastRxPacket = \num{0.0e-9}$

$timeFirstRxPacket = \num{0.0e-9}$

$Throughput = \num{0}(bps)$
\end{center}

##### Sink Node 13

\begin{center}
$rxBytes = 43132$

$timeLastRxPacket = \num{2446834081e-9}$

$timeFirstRxPacket = \num{2387027010e-9}$

$Throughput = \num{5769485.016}(bps)$
\end{center}

##### Sink Node 14

\begin{center}
$rxBytes = 0$

$timeLastRxPacket = \num{0.0e-9}$

$timeFirstRxPacket = \num{0.0e-9}$

$Throughput = \num{0}(bps)$
\end{center}

##### Sink Node 15

\begin{center}
$rxBytes = 0$

$timeLastRxPacket = \num{0.0e-9}$

$timeFirstRxPacket = \num{0.0e-9}$

$Throughput = \num{0}(bps)$
\end{center}

# Bibliography
