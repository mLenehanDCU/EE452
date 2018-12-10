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

In order to make the provided code compatible with ns-3.29, the constructors
used for the "WifiHelper" and NqosWifiMacHelper" classes must be changed. The
RAA used must also be changed, as the provided option "AarfWifiManager" is no
longer supported for "HT" rates in ns-3.29. As such, the MinstrlHTWifiManager is
used.

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

There are a test 

### Modified TCP with multiple Nodes Code Snippets

There are a

# Results

## Analysis of TCP and UDP

### Listed 802.11ac data rates

The advertised physical layer data rate of 802.11ac is approximately 7Gbps
(at 5GHz). Single users may achieve MAC throughput of 500Mbps, with multiple
users achievingrates of 1Gbps in total [@inproceedings]. There are a number of
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
\caption{}
\label{}
\end{figure}

The following figure displays the throughput graph for a system using the UDP
protocol, consisting of a single source node, and a single sink node.

\begin{figure}[H]
\includegraphics{Images/udp1SinkTPGraph.png}
\centering
\caption{}
\label{}
\end{figure}

### UDP Deviation

### TCP Deviation

### Trace

## Impact on Load on Performance

### Throughput for All nodes in All Examples

#### TCP 5 Sinks

The following figure displays the total throughput graph for a system using the
TCP protocol, consisting of a single source node, and five sink nodes.

The individual node throughputs may be seen in the following graph, giving a
more detailed view on the impact of multiple sink nodes on throughput.

#### TCP 10 Sinks

The following figure displays the total throughput graph for a system using the
TCP protocol, consisting of a single source node, and ten sink node.

#### TCP 15 Sinks

The following figure displays the total throughput graph for a system using the
TCP protocol, consisting of a single source node, and fifteen sink node.

#### UDP 5 Sinks

The following figure displays the total throughput graph for a system using the
UDP protocol, consisting of a single source node, and five sink node.

\begin{figure}[H]
\includegraphics{Images/udp5SinkTPGraph.png}
\centering
\caption{}
\label{}
\end{figure}

#### UDP 10 Sinks

The following figure displays the total throughput graph for a system using the
UDP protocol, consisting of a single source node, and ten sink node.

\begin{figure}[H]
\includegraphics{Images/udp10SinkTPGraph.png}
\centering
\caption{}
\label{}
\end{figure}


#### UDP 15 Sinks

The following figure displays the total throughput graph for a system using the
UDP protocol, consisting of a single source node, and fifteen sink node.

\begin{figure}[H]
\includegraphics{Images/udp15SinkTPGraph.png}
\centering
\caption{}
\label{}
\end{figure}

### Comparison of Results

# Conclusion

# Appendix

## Code

The following code was used in the completion of this assignment:

### TCP

#### tcpWireless1Sink


#### tcpWireless5Sink


#### tcpWireless10Sink


#### tcpWireless15Sink

### UDP

#### udpWireless1Sink


#### udpWireless5Sink


####udpWireless10Sink


### udpWireless15Sink


# Bibliography
