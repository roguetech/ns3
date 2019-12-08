/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Joe Kopena <tjkopena@cs.drexel.edu>
 * Modified by: Longhao Zou, Oct 2016 for EE500 <longhao.zou@dcu.ie>
 * 
 
            EE500 Assignment 2016-2017
            Default WiFi Network Topology
  
                WiFi 192.168.0.0
            -------------------------
            |AP (node 0:192.168.0.1)|
            -------------------------
             *         *           *    
            /          |            \
  Traffic 1/  Traffic 2|    ------   \ Traffic N
          /            |              \
      user 1       user 2     ------   user N
   (node 1         (node 2     ------  (node N
   :192.168.0.2    :192.168.0.3 ------ :192.168.0.N+1
   :1000)          :1001)        ------:1000+(N-1))
   
   PS: In this example, I just implemented only 1 WiFi user.
   
 */
  
#include <ctime>

#include <sstream>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/stats-module.h"

#include "wifi-example-apps.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("WiFiExampleSim");




void TxCallback (Ptr<CounterCalculator<uint32_t> > datac,
                 std::string path, Ptr<const Packet> packet) {
  NS_LOG_INFO ("Sent frame counted in " <<
               datac->GetKey ());
  datac->Update ();
  // end TxCallback
}




//----------------------------------------------------------------------
//-- main
//----------------------------------------------
int main (int argc, char *argv[]) {
  //LogComponentEnable ("WiFiDistanceApps", LOG_LEVEL_INFO);
  double distance = 50.0;
  double distance1 = 100.0;
  double numberOfNodes = 2;
  int wifinodes = 0;
  double simTime = 20; //Simulation Running Time (in seconds)
  string format ("db"); //Default as .db format (Please refer to sqlite-data-output.cc and sqlite-data-output.h located in /src/stats/model)
  string experiment ("wifi-example-sim"); //the name of your experiment
  string strategy ("wifi-default"); 
  string input;
  string runID; 

  {
    stringstream sstr;
    sstr << "run-" << time (NULL);
    runID = sstr.str ();
  }

  // Set up command line parameters used to control the experiment.
  CommandLine cmd;
  cmd.AddValue ("distance", "Distance apart to place nodes (in meters).",
                distance);
  cmd.AddValue ("distance1", "Distance apart to place nodes (in meters).",
                distance1);
  cmd.AddValue ("numberOfNodes", "Number of Wifi Nodes",
                numberOfNodes);
  //cmd.AddValue ("format", "Format to use for data output.", 
    //            format);
  cmd.AddValue ("simTime", "Simulation Running Time (in seconds)", simTime);
  cmd.AddValue ("experiment", "Identifier for experiment.",
                experiment);
  cmd.AddValue ("strategy", "Identifier for strategy.",
                strategy);
  cmd.AddValue ("run", "Identifier for run.",
                runID);
  cmd.Parse (argc, argv);

  if (format != "omnet" && format != "db") {
      NS_LOG_ERROR ("Unknown output format '" << format << "'");
      return -1;
    }

  #ifndef STATS_HAS_SQLITE3
  if (format == "db") {
      NS_LOG_ERROR ("sqlite support not compiled in.");
      return -1;
    }
  #endif

  {
    stringstream sstr ("");
    sstr << distance;
    input = sstr.str ();
  }




  //------------------------------------------------------------
  //-- Create nodes and network stacks
  //--------------------------------------------
  NS_LOG_INFO ("Creating nodes.");
  wifinodes = numberOfNodes + 1;
  NodeContainer nodes;
  nodes.Create (wifinodes);

  NS_LOG_INFO ("Installing WiFi and Internet stack.");
  WifiHelper wifi;
  WifiMacHelper wifiMac;
  wifiMac.SetType ("ns3::AdhocWifiMac");
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  wifiPhy.SetChannel (wifiChannel.Create ());
  NetDeviceContainer nodeDevices = wifi.Install (wifiPhy, wifiMac, nodes);

  InternetStackHelper internet;
  internet.Install (nodes);
  Ipv4AddressHelper ipAddrs;
  ipAddrs.SetBase ("192.168.0.0", "255.255.255.0");
  ipAddrs.Assign (nodeDevices);
  
  //------------------------------------------------------------
  //-- Setup physical layout
  //--------------------------------------------
  NS_LOG_INFO ("Installing static mobility; distance " << distance << " .");
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc =
    CreateObject<ListPositionAllocator>();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));
  for ( int i = 1; i <= numberOfNodes; i++) {
    positionAlloc->Add (Vector (0.0, distance, 0.0));
  }
  mobility.SetPositionAllocator (positionAlloc);
  mobility.Install (nodes);


  //------------------------------------------------------------
  //-- Create the traffic between AP and WiFi Users
  //------------------------------------------------------------
  //------------------------------------------------------------
  //-- 1. Create the first traffic for the first WiFi user on WiFi AP (source)
  //--------------------------------------------
  NS_LOG_INFO ("Create traffic source & sink.");
  string appSource = "appSource";
  string sender = "sender";
  for ( int i = 1; i <= numberOfNodes; i++) {
	appSource = "appSource" + std::to_string(i);
  	Ptr<Node> appSource = NodeList::GetNode (0);
	sender = "sender" + std::to_string(i);
  	Ptr<Sender> sender = CreateObject<Sender>();
  	sender->SetAttribute("Port", UintegerValue(1000));//Lisening Port of the first WiFi user
  	sender->SetAttribute("PacketSize", UintegerValue (1000)); //bytes
  	sender->SetAttribute("Interval", StringValue ("ns3::ConstantRandomVariable[Constant=0.008]")); //seconds
  	sender->SetAttribute("NumPackets",UintegerValue (100000000));
  	appSource->AddApplication (sender);
  	sender->SetStartTime (Seconds (0));
  }

  DataCollector dataofuser1;
  dataofuser1.DescribeRun (experiment,
                    strategy,
                    input,
                    runID);

  // Add any information we wish to record about this run.
  dataofuser1.AddMetadata ("author", "EE500-PatrickRoughan"); //Please replace XXX with your first name!

  string receiver;
  string appSink;
  for ( int i = 1; i <= numberOfNodes; i++) {
  	appSink = "appSink";
	appSink = "appSink" + std::to_string(i);
  	Ptr<Node> appSink = NodeList::GetNode (i);
	receiver = "receiver" + std::to_string(i);
	Ptr<Receiver> receiver = CreateObject<Receiver>();	
  	receiver->SetAttribute("Port", UintegerValue(1000));//Lisening Port
  	appSink->AddApplication (receiver);
  	receiver->SetStartTime (Seconds (0));
        
        //int j = i - 1;	
	int k = i + 1;
	string ip = "192.168.0." + std::to_string(k) + "";
       
        //Config::Set ("/NodeList/0/ApplicationList/0/$Sender/Destination",
        //         Ipv4AddressValue ("" + std::to_string(ip) + "")));

	// Here a counter for received packets is directly manipulated by
  	// one of the custom objects in our simulation, the Receiver
  	// Application.  The Receiver object is given a pointer to the
  	// counter and calls its Update() method whenever a packet arrives
        Ptr<CounterCalculator<> > appRx1 =
                CreateObject<CounterCalculator<> >();
        appRx1->SetKey ("receiver-rx-packets");
        appRx1->SetContext ("node[" + std::to_string(i) + "]");  
        receiver->SetCounter (appRx1);
        dataofuser1.AddDataCalculator (appRx1);

	// This is similar, but creates a counter to track how many frames
  	// are received.  Instead of our own glue function, this uses a
  	// method of an adapter class to connect a counter directly to the
  	// trace signal generated by the WiFi MAC.
	Ptr<PacketCounterCalculator> totalRx1 =
    	CreateObject<PacketCounterCalculator>();
  	totalRx1->SetKey ("wifi-rx-frames");
  	totalRx1->SetContext ("node[" + std::to_string(i) + "]");
  	Config::Connect ("/NodeList/" + std::to_string(i) + "/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx",
                   MakeCallback (&PacketCounterCalculator::PacketUpdate,
                                 totalRx1));
  	dataofuser1.AddDataCalculator (totalRx1);

	Ptr<TimeMinMaxAvgTotalCalculator> delayStat =
    	CreateObject<TimeMinMaxAvgTotalCalculator>();
  	delayStat->SetKey ("delay");
  	delayStat->SetContext (".");
  	receiver->SetDelayTracker (delayStat); //nanoseconds
  	dataofuser1.AddDataCalculator (delayStat);

  }
  Config::Set ("/NodeList/0/ApplicationList/0/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.2"));
  Config::Set ("/NodeList/0/ApplicationList/1/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.3"));
  Config::Set ("/NodeList/0/ApplicationList/2/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.4"));
  Config::Set ("/NodeList/0/ApplicationList/3/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.5"));
  Config::Set ("/NodeList/0/ApplicationList/4/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.6"));
  Config::Set ("/NodeList/0/ApplicationList/5/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.7"));
  Config::Set ("/NodeList/0/ApplicationList/6/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.8"));
  Config::Set ("/NodeList/0/ApplicationList/7/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.9"));
  Config::Set ("/NodeList/0/ApplicationList/8/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.10"));
  Config::Set ("/NodeList/0/ApplicationList/9/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.11"));
  Config::Set ("/NodeList/0/ApplicationList/10/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.12"));
  Config::Set ("/NodeList/0/ApplicationList/11/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.13"));
  Config::Set ("/NodeList/0/ApplicationList/12/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.14"));
  Config::Set ("/NodeList/0/ApplicationList/13/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.15"));
  Config::Set ("/NodeList/0/ApplicationList/14/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.16"));
  Config::Set ("/NodeList/0/ApplicationList/15/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.17"));
  Config::Set ("/NodeList/0/ApplicationList/16/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.18"));
  Config::Set ("/NodeList/0/ApplicationList/17/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.19"));
  Config::Set ("/NodeList/0/ApplicationList/18/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.20"));
  Config::Set ("/NodeList/0/ApplicationList/19/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.21"));
  Config::Set ("/NodeList/0/ApplicationList/20/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.22"));
  Config::Set ("/NodeList/0/ApplicationList/21/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.23"));
  Config::Set ("/NodeList/0/ApplicationList/22/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.24"));
  Config::Set ("/NodeList/0/ApplicationList/23/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.25"));
  Config::Set ("/NodeList/0/ApplicationList/24/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.26"));
  Config::Set ("/NodeList/0/ApplicationList/25/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.27"));
  Config::Set ("/NodeList/0/ApplicationList/26/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.28"));
  Config::Set ("/NodeList/0/ApplicationList/27/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.29"));
  Config::Set ("/NodeList/0/ApplicationList/28/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.30"));
  Config::Set ("/NodeList/0/ApplicationList/29/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.31"));
  Config::Set ("/NodeList/0/ApplicationList/30/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.32"));
  Config::Set ("/NodeList/0/ApplicationList/31/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.33"));
  Config::Set ("/NodeList/0/ApplicationList/32/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.34"));
  Config::Set ("/NodeList/0/ApplicationList/33/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.35"));
  Config::Set ("/NodeList/0/ApplicationList/34/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.36"));
  Config::Set ("/NodeList/0/ApplicationList/35/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.37"));
  Config::Set ("/NodeList/0/ApplicationList/36/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.38"));
  Config::Set ("/NodeList/0/ApplicationList/37/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.39"));
  Config::Set ("/NodeList/0/ApplicationList/38/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.40"));
  Config::Set ("/NodeList/0/ApplicationList/39/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.41"));
  Config::Set ("/NodeList/0/ApplicationList/40/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.42"));
  Config::Set ("/NodeList/0/ApplicationList/41/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.43"));
  Config::Set ("/NodeList/0/ApplicationList/42/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.44"));
  Config::Set ("/NodeList/0/ApplicationList/43/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.45"));
  Config::Set ("/NodeList/0/ApplicationList/44/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.46"));
  Config::Set ("/NodeList/0/ApplicationList/45/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.47"));
  Config::Set ("/NodeList/0/ApplicationList/46/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.48"));
  Config::Set ("/NodeList/0/ApplicationList/47/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.49"));
  Config::Set ("/NodeList/0/ApplicationList/48/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.50"));
  Config::Set ("/NodeList/0/ApplicationList/49/$Sender/Destination",
                 Ipv4AddressValue ("192.168.0.51"));
  
  //------------------------------------------------------------
  //-- Setup stats and data collection 
  //--  for the WiFi AP and the first WiFi User
  //--------------------------------------------

  // Create a counter to track how many frames are generated.  Updates
  // are triggered by the trace signal generated by the WiFi MAC model
  // object.  Here we connect the counter to the signal via the simple
  // TxCallback() glue function defined above.
  Ptr<CounterCalculator<uint32_t> > totalTx =
    CreateObject<CounterCalculator<uint32_t> >();
  totalTx->SetKey ("wifi-tx-frames");
  totalTx->SetContext ("node[0]");
  Config::Connect ("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Mac/MacTx",
                   MakeBoundCallback (&TxCallback, totalTx));
  dataofuser1.AddDataCalculator (totalTx);

  Ptr<PacketCounterCalculator> appTx =
    CreateObject<PacketCounterCalculator>();
  appTx->SetKey ("sender-tx-packets");
  appTx->SetContext ("node[0]");
  Config::Connect ("/NodeList/0/ApplicationList/*/$Sender/Tx",
                   MakeCallback (&PacketCounterCalculator::PacketUpdate,
                                 appTx));
  dataofuser1.AddDataCalculator (appTx);

  // This DataCalculator connects directly to the transmit trace
  // provided by our Sender Application.  It records some basic
  // statistics about the sizes of the packets received (min, max,
  // avg, total # bytes), although in this scenaro they're fixed.
  Ptr<PacketSizeMinMaxAvgTotalCalculator> appTxPkts =
    CreateObject<PacketSizeMinMaxAvgTotalCalculator>();
  appTxPkts->SetKey ("tx-pkt-size");
  appTxPkts->SetContext ("node[0]");
  Config::Connect ("/NodeList/0/ApplicationList/*/$Sender/Tx",
                   MakeCallback
                     (&PacketSizeMinMaxAvgTotalCalculator::PacketUpdate,
                     appTxPkts));
  dataofuser1.AddDataCalculator (appTxPkts);


  // Here we directly manipulate another DataCollector tracking min,
  // max, total, and average propagation delays.  Check out the Sender
  // and Receiver classes to see how packets are tagged with
  // timestamps to do this.
  //Ptr<TimeMinMaxAvgTotalCalculator> delayStat =
  //  CreateObject<TimeMinMaxAvgTotalCalculator>();
  //delayStat->SetKey ("delay");
  //delayStat->SetContext (".");
  //receiver->SetDelayTracker (delayStat); //nanoseconds
  //dataofuser1.AddDataCalculator (delayStat);

  wifiPhy.EnablePcapAll ("first");


  //------------------------------------------------------------
  //-- Run the simulation
  //--------------------------------------------
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop(Seconds(simTime));
  Simulator::Run ();
  



  //------------------------------------------------------------
  //-- Generate statistics output.
  //--------------------------------------------

  // Pick an output writer based in the requested format.
  Ptr<DataOutputInterface> output = 0;
  if (format == "omnet") {
      NS_LOG_INFO ("Creating omnet formatted data output.");
      output = CreateObject<OmnetDataOutput>();
    } else if (format == "db") {
    #ifdef STATS_HAS_SQLITE3
      NS_LOG_INFO ("Creating sqlite formatted data output.");
      output = CreateObject<SqliteDataOutput>();
    #endif
    } else {
      NS_LOG_ERROR ("Unknown output format " << format);
    }

  // Finally, have that writer interrogate the DataCollector and save
  // the results.
  if (output != 0)
  {
    output->SetFilePrefix("DataOfUser1");
    output->Output (dataofuser1);
  }
  // Free any memory here at the end of this example.
  
  Simulator::Destroy ();

  // end main
}
