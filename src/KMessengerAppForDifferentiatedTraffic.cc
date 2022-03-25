//
// The model implementation for the Messenger application.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de), Anna Förster (afoerster@uni-bremen.de)
// @date   : 15-aug-2016, updated 6-febr-2018
//
// MessengerApp extended to handle differentiate traffic based on
// patterns, either based on random time frames or based
// the original traffic pattern in the traces.
//
// @author: Thenuka Karunatilake (thenukaramesh@gmail.com)
// @date: 01-03-2021
//

#include "KMessengerAppForDifferentiatedTraffic.h"

#include "KBaseNodeInfo.h"
#include <iostream> 
#include <fstream>
#include <string>
#include <algorithm>
#include <map>

Define_Module(KMessengerAppForDifferentiatedTraffic);

vector<KBaseNodeInfo*> messengerForDifferentiatedTrafficNodeInfoList;
int nextGenerationIndex = 0; //UniqueID for the generated msgs

void KMessengerAppForDifferentiatedTraffic::initialize(int stage)
{
    if (stage == 0) {

        // get parameters
        nodeIndex = par("nodeIndex");
        ownMACAddress = par("ownMACAddress").stringValue();
        usedRNG = par("usedRNG");
        dataGenerationInterval = par("dataGenerationInterval");
        dataSizeInBytes = par("dataSizeInBytes");
        ttl = par("ttl");
        totalSimulationTime = SimTime::parse(getEnvir()->getConfig()->getConfigValue("sim-time-limit")).dbl();
		notificationCount = totalSimulationTime/dataGenerationInterval;
		totalNumNodes = getParentModule()->getParentModule()->par("numNodes");
	dataGenerationIntervalMode = par("dataGenerationIntervalMode").stringValue();
        trafficInfoPath = par("trafficInfoPath").stringValue();

	if (dataGenerationIntervalMode == "random") {

            // Identifying the group category
            string fileName = trafficInfoPath + "/"+ "data.txt";
            ifstream ip(fileName);
            if(!ip.is_open()) {
                EV_FATAL <<  KMESSENGERAPPFORDIFFERENTIATEDTRAFFIC_SIMMODULEINFO << " " 
                            << fileName << " not found - check trafficInfoPath parameter\n";
                endSimulation();
            }
            string strfirstnumber;
            string strlastnumber;
            string strgroup;

            while(ip.good()){
                getline(ip,strfirstnumber,',');
                getline(ip,strlastnumber,',');
                getline(ip,strgroup,'\n');
                int firstnumber=std::stoi(strfirstnumber);
                int lastnumber=std::stoi(strlastnumber);
                int group=std::stoi(strgroup);
                if(nodeIndex<=lastnumber){
                    groupId=group;
                    break;
                }
            }
            ip.close();
        }

		specificDestination = par("specificDestination");
		specificDestinationNodeName = par("specificDestinationNodeName").stringValue();

        // add own address to global list to use for random destination selections
        ownNodeInfo = new KBaseNodeInfo();
        ownNodeInfo->nodeAddress = ownMACAddress;
        ownNodeInfo->nodeMessengerAppForDifferentiatedTrafficModule = this;
        ownNodeInfo->nodeName = getParentModule()->getFullName();

        messengerForDifferentiatedTrafficNodeInfoList.push_back(ownNodeInfo);

        


    } else if (stage == 1) {

        // find the destination node, if specific destination is selected
        if (specificDestination && strstr(ownNodeInfo->nodeName.c_str(), specificDestinationNodeName.c_str()) != NULL) {
            ownNodeIsDestination = TRUE;
            destinationNodeInfo = NULL;

        } else if (specificDestination) {
            ownNodeIsDestination = FALSE;
            bool found = FALSE;
            for(int i = 0; i < messengerForDifferentiatedTrafficNodeInfoList.size(); i++) {
                if (strstr(messengerForDifferentiatedTrafficNodeInfoList[i]->nodeName.c_str(), specificDestinationNodeName.c_str()) != NULL) {
                    destinationNodeInfo = messengerForDifferentiatedTrafficNodeInfoList[i];
                    found = TRUE;
                    break;
                }
            }
            if (!found) {
                EV_FATAL << KMESSENGERAPPFORDIFFERENTIATEDTRAFFIC_SIMMODULEINFO << "The node " << specificDestinationNodeName
                        << " given in specificDestinationNodeName does not exist\n";
                endSimulation();
            }
        } else {
            destinationNodeInfo = NULL;

        }

    } else if (stage == 2) {

        // create and setup app registration trigger
        appRegistrationEvent = new cMessage("App Registration Event");
        appRegistrationEvent->setKind(KMESSENGERAPPFORDIFFERENTIATEDTRAFFIC_REGISTRATION_EVENT);
        scheduleAt(simTime(), appRegistrationEvent);

        // set scheduler to send data but only if specific destination is false or
        // if specific destination is true and this node is the selected destination
        if (!specificDestination || (specificDestination && !ownNodeIsDestination)) {

            // this is a round-robin scheduling of traffic: in a row, everybody gets a chance to send the next packet.
            dataTimeoutEvent = new cMessage("Data Timeout Event");
            dataTimeoutEvent->setKind(KMESSENGERAPPFORDIFFERENTIATEDTRAFFIC_DATASEND_EVENT);
            	// Setting first transmission event
        if (dataGenerationIntervalMode == "trace") {
            string fileName = trafficInfoPath + "/"+ "packet_generation_new.txt";
            ifstream ip(fileName);
            if(!ip.is_open()) {
                EV_FATAL <<  KHERALDAPPFORDIFFERENTIATEDTRAFFIC_SIMMODULEINFO << " " 
                                    << fileName << " not found - check trafficInfoPath parameter\n";
                endSimulation();
            }
            string strnodeid;
            string strgenerationtime;
            while(ip.good()) {
                getline(ip,strnodeid,' ');
                getline(ip,strgenerationtime,'\n');
                int nodeid = std::stoi(strnodeid);
                int generationtime=std::stoi(strgenerationtime);
                if(nodeIndex<nodeid){
                    break;
                }
                else if(nodeIndex==nodeid){
                    scheduleAt(generationtime, dataTimeoutEvent);
                    break;
                }
            }
            ip.close();
        } else {
            scheduleAt(simTime() + (dataGenerationInterval * nodeIndex) + 0.1, dataTimeoutEvent);
        }
        }

        // registering statistic signals
        likedDataBytesReceivedSignal = registerSignal("appLikedDataBytesReceived");
        nonLikedDataBytesReceivedSignal = registerSignal("appNonLikedDataBytesReceived");
        duplicateDataBytesReceivedSignal = registerSignal("appDuplicateDataBytesReceived");
        totalDataBytesReceivedSignal = registerSignal("appTotalDataBytesReceived");

        likedDataBytesMaxReceivableSignal = registerSignal("appLikedDataBytesMaxReceivable");
        nonLikedDataBytesMaxReceivableSignal = registerSignal("appNonLikedDataBytesMaxReceivable");
        totalDataBytesMaxReceivableSignal = registerSignal("appTotalDataBytesMaxReceivable");

        likedDataCountReceivedSignal = registerSignal("appLikedDataCountReceived");
        nonLikedDataCountReceivedSignal = registerSignal("appNonLikedDataCountReceived");
        duplicateDataCountReceivedSignal = registerSignal("appDuplicateDataCountReceived");
        totalDataCountReceivedSignal = registerSignal("appTotalDataCountReceived");

        likedDataCountMaxReceivableSignal = registerSignal("appLikedDataCountMaxReceivable");
        nonLikedDataCountMaxReceivableSignal = registerSignal("appNonLikedDataCountMaxReceivable");
        totalDataCountMaxReceivableSignal = registerSignal("appTotalDataCountMaxReceivable");

        likedDataReceivedDelaySignal = registerSignal("appLikedDataReceivedDelay");
        nonLikedDataReceivedDelaySignal = registerSignal("appNonLikedDataReceivedDelay");
        totalDataReceivedDelaySignal = registerSignal("appTotalDataReceivedDelay");

        likedDataCountReceivedForAvgDelayCompSignal = registerSignal("appLikedDataCountReceivedForAvgDelayComp");
        nonLikedDataCountReceivedForAvgDelayCompSignal = registerSignal("appNonLikedDataCountReceivedForAvgDelayComp");
        totalDataCountReceivedForAvgDelayCompSignal = registerSignal("appTotalDataCountReceivedForAvgDelayComp");

        likedDataCountReceivedForRatioCompSignal = registerSignal("appLikedDataCountReceivedForRatioComp");
        nonLikedDataCountReceivedForRatioCompSignal = registerSignal("appNonLikedDataCountReceivedForRatioComp");
        totalDataCountReceivedForRatioCompSignal = registerSignal("appTotalDataCountReceivedForRatioComp");

        likedDataCountMaxReceivableForRatioCompSignal = registerSignal("appLikedDataCountMaxReceivableForRatioComp");
        nonLikedDataCountMaxReceivableForRatioCompSignal = registerSignal("appNonLikedDataCountMaxReceivableForRatioComp");
        totalDataCountMaxReceivableForRatioCompSignal = registerSignal("appTotalDataCountMaxReceivableForRatioComp");

        likedDataHopsForAvgHopsCompSignal = registerSignal("appLikedDataHopsForAvgHopsComp");
        nonLikedDataHopsForAvgHopsCompSignal = registerSignal("appNonLikedDataHopsForAvgHopsComp");
        totalDataHopsForAvgHopsCompSignal = registerSignal("appTotalDataHopsForAvgHopsComp");

        likedDataHopsCountForAvgHopsCompSignal = registerSignal("appLikedDataHopsCountForAvgHopsComp");
        nonLikedDataHopsCountForAvgHopsCompSignal = registerSignal("appNonLikedDataHopsCountForAvgHopsComp");
        totalDataHopsCountForAvgHopsCompSignal = registerSignal("appTotalDataHopsCountForAvgHopsComp");

        likedDataReceivedDelaySignal2 = registerSignal("appLikedDataReceivedDelay2");
        nonLikedDataReceivedDelaySignal2 = registerSignal("appNonLikedDataReceivedDelay2");
        totalDataReceivedDelaySignal2 = registerSignal("appTotalDataReceivedDelay2");

        totalDataBytesReceivedSignal2 = registerSignal("appTotalDataBytesReceived2");
        totalDataCountReceivedSignal2 = registerSignal("appTotalDataCountReceived2");

    } else {
        EV_FATAL << KMESSENGERAPPFORDIFFERENTIATEDTRAFFIC_SIMMODULEINFO << "Something is radically wrong\n";
    }
}

int KMessengerAppForDifferentiatedTraffic::numInitStages() const
{
    return 3;
}

void KMessengerAppForDifferentiatedTraffic::handleMessage(cMessage *msg)
{

    // check message
    if (msg->isSelfMessage() && msg->getKind() == KMESSENGERAPPFORDIFFERENTIATEDTRAFFIC_REGISTRATION_EVENT) {
        // send app registration message the forwarding layer
        KRegisterAppMsg *regAppMsg = new KRegisterAppMsg("Messenger App Registration");
        regAppMsg->setAppName("Messenger");
        regAppMsg->setPrefixName(prefixName);

        send(regAppMsg, "lowerLayerOut");

    } else if (msg->isSelfMessage() && msg->getKind() == KMESSENGERAPPFORDIFFERENTIATEDTRAFFIC_DATASEND_EVENT) {

        // if messages possible to be sent to multiple destinations, then
        // find a random destination
        if (!specificDestination) {

            // find random destination to send
            bool found = FALSE;
            destinationNodeInfo = messengerForDifferentiatedTrafficNodeInfoList[0];
            while (!found) {
                int destIndex = intuniform(0, (messengerForDifferentiatedTrafficNodeInfoList.size() - 1), usedRNG);
                destinationNodeInfo = messengerForDifferentiatedTrafficNodeInfoList[destIndex];
                if (destinationNodeInfo->nodeAddress != ownMACAddress) {
                    found = TRUE;
                }
            }
        }

	//Setting next data generation event
        if (dataGenerationIntervalMode == "random") {
            string groupname = to_string(groupId);
            string filename=trafficInfoPath + "/" + "Group" + groupname + ".txt";
            fstream ip(filename);

            if(!ip.is_open()) {
                EV_FATAL <<  KHERALDAPPFORDIFFERENTIATEDTRAFFIC_SIMMODULEINFO << " "
                                << filename << " not found - check trafficInfoPath parameter\n";
                endSimulation();
            }
            string sim1, sim2, byte1, byte2, TTL1, TTL2, time1, time2;
            while(ip.good()) {
                getline(ip,sim1,',');
                getline(ip,sim2,',');
                getline(ip,byte1,',');
                getline(ip,byte2,',');
                getline(ip,TTL1,',');
                getline(ip,TTL2,',');
                getline(ip,time1,',');
                getline(ip,time2,'\n');
                int sim1number=std::stoi(sim1);
                double sim2number=std::stoi(sim2);
                int byte1number=std::stoi(byte1);
                int byte2number=std::stoi(byte2);
                int TTL1number=std::stoi(TTL1);
                int TTL2number=std::stoi(TTL2);
                int time1number=std::stoi(time1);
                int time2number=std::stoi(time2);

                if(simTime()<=sim2number) {
                    dataSizeInBytes=(int)uniform(byte1number, byte2number, usedRNG);
                    ttl=(int)uniform(TTL1number, TTL2number, usedRNG);
                    dataGenerationInterval=(int)uniform(time1number, time2number, usedRNG);
                    break;
                }

            }
            ip.close();
            scheduleAt(simTime() + dataGenerationInterval , msg);
        } else {
            string filename=trafficInfoPath + "/" + "packet_generation_new.txt";
            ifstream ip(filename);
            if(!ip.is_open()) {
                EV_FATAL <<  KHERALDAPPFORDIFFERENTIATEDTRAFFIC_SIMMODULEINFO << " " 
                            << filename << " not found - check trafficInfoPath parameter\n";
                endSimulation();
            }
            string strid, strtime;
            dataSizeInBytes=(int)uniform(10000, 100000, usedRNG);
            ttl=(int)uniform(7200, 72000, usedRNG);
            while(ip.good()){
                getline(ip,strid,' ');
                getline(ip,strtime,'\n');
                int nodeidnow=std::stoi(strid);
                double generationtimenow=std::stoi(strtime);
                if(nodeIndex<nodeidnow) {
                    break;
                } else if(nodeIndex==nodeidnow && generationtimenow>simTime().dbl()) {
                    dataGenerationInterval=generationtimenow;
                    scheduleAt(dataGenerationInterval , msg);
                    break;
                }
            }
            ip.close();
        }

        //setup the data message for sending down to forwarding layer
        char tempString[128];
        sprintf(tempString, "D item-%0d", KMESSENGERAPPFORDIFFERENTIATEDTRAFFIC_START_ITEM_ID + nextGenerationIndex);

        KDataMsg *dataMsg = new KDataMsg(tempString);

        dataMsg->setSourceAddress("");
        dataMsg->setDestinationAddress("");

        sprintf(tempString, "/messenger/item-%0d", KMESSENGERAPPFORDIFFERENTIATEDTRAFFIC_START_ITEM_ID + nextGenerationIndex);

        dataMsg->setDataName(tempString);
        dataMsg->setGoodnessValue(100);
        dataMsg->setRealPayloadSize(dataSizeInBytes);
        dataMsg->setMsgUniqueID(nextGenerationIndex);
        dataMsg->setInitialInjectionTime(simTime());

        sprintf(tempString, "Details of item-%0d", KMESSENGERAPPFORDIFFERENTIATEDTRAFFIC_START_ITEM_ID + nextGenerationIndex);
        dataMsg->setDummyPayloadContent(tempString);
		
        dataMsg->setByteLength(dataSizeInBytes);
        dataMsg->setMsgType(0);
        dataMsg->setValidUntilTime(ttl);

        dataMsg->setInitialOriginatorAddress(ownMACAddress.c_str());
        dataMsg->setDestinationOriented(true);
        dataMsg->setFinalDestinationAddress(destinationNodeInfo->nodeAddress.c_str());

        send(dataMsg, "lowerLayerOut");

		// schedule again
		//nextGenerationIndex += totalNumNodes;	
	    //scheduleAt(simTime() + (dataGenerationInterval * totalNumNodes), msg);
        
        // make receivable node generate stats
        KStatisticsMsg *statMsg = new KStatisticsMsg("StatMsg");
        statMsg->setLikedData(true);
        statMsg->setDataCountReceivable(1);
        statMsg->setDataBytesReceivable(dataSizeInBytes);
        sendDirect(statMsg, destinationNodeInfo->nodeMessengerAppForDifferentiatedTrafficModule, "statIn");

	nextGenerationIndex = nextGenerationIndex + 1;

        //cout << KMESSENGERAPPFORDIFFERENTIATEDTRAFFIC_SIMMODULEINFO << " sending: " << (nextGenerationIndex - totalNumNodes) << " own addr " << ownMACAddress << " dest addr " << selectedNodeInfo->nodeAddress << "\n";

    } else if (dynamic_cast<KDataMsg*>(msg) != NULL) {

        // message received from outside so, process received data message
        KDataMsg *dataMsg = check_and_cast<KDataMsg *>(msg);

        // check if it is a duplicate receipt
        int i = dataMsg->getMsgUniqueID();
        //timesMessagesReceived[i]++;

        // compute stats
        simtime_t diff = 0.0;
         if (std::find(timesMessagesReceived1.begin(),timesMessagesReceived1.end(),i)!=timesMessagesReceived1.end()) {
            emit(duplicateDataCountReceivedSignal, (int) 1);
            emit(totalDataCountReceivedSignal, (int) 1);
            emit(duplicateDataBytesReceivedSignal, dataMsg->getRealPayloadSize());
            emit(totalDataBytesReceivedSignal, dataMsg->getRealPayloadSize());

        } else {
            timesMessagesReceived1.push_back(i); //mark this msg as received
            emit(likedDataCountReceivedSignal, (int) 1);
            emit(totalDataCountReceivedSignal, (int) 1);
            emit(likedDataBytesReceivedSignal, dataMsg->getRealPayloadSize());
            emit(totalDataBytesReceivedSignal, dataMsg->getRealPayloadSize());

            diff = simTime() - dataMsg->getInitialInjectionTime();
            emit(likedDataReceivedDelaySignal, diff);
            emit(totalDataReceivedDelaySignal, diff);
            emit(likedDataCountReceivedForAvgDelayCompSignal, (int) 1);
            emit(totalDataCountReceivedForAvgDelayCompSignal, (int) 1);

            emit(likedDataCountReceivedForRatioCompSignal, (int) 1);
            emit(totalDataCountReceivedForRatioCompSignal, (int) 1);

            emit(likedDataHopsForAvgHopsCompSignal, dataMsg->getHopsTravelled());
            emit(totalDataHopsForAvgHopsCompSignal, dataMsg->getHopsTravelled());

            emit(likedDataHopsCountForAvgHopsCompSignal, (int) 1);
            emit(totalDataHopsCountForAvgHopsCompSignal, (int) 1);

            emit(likedDataReceivedDelaySignal2, diff);
            emit(totalDataReceivedDelaySignal2, diff);

            emit(totalDataCountReceivedSignal2, (int) 1);
            emit(totalDataBytesReceivedSignal2, dataMsg->getRealPayloadSize());

        }


        //cout << KMESSENGERAPPFORDIFFERENTIATEDTRAFFIC_SIMMODULEINFO << " receiving: " << dataMsg->getMsgUniqueID() << " own addr " << ownMACAddress << " dest addr " << dataMsg->getFinalDestinationAddress() << " msg delay " << diff << "\n";

        delete msg;
        
    } else if (dynamic_cast<KStatisticsMsg*>(msg) != NULL) {
        // message received from outside so, process received statistics message
        KStatisticsMsg *statMsg = check_and_cast<KStatisticsMsg *>(msg);

        // update receivables
        emit(likedDataCountMaxReceivableSignal, statMsg->getDataCountReceivable());
        emit(totalDataCountMaxReceivableSignal, statMsg->getDataCountReceivable());

        emit(likedDataBytesMaxReceivableSignal, statMsg->getDataBytesReceivable());
        emit(totalDataBytesMaxReceivableSignal, statMsg->getDataBytesReceivable());

        emit(likedDataCountMaxReceivableForRatioCompSignal, statMsg->getDataCountReceivable());
        emit(totalDataCountMaxReceivableForRatioCompSignal, statMsg->getDataCountReceivable());

        //cout << KMESSENGERAPPFORDIFFERENTIATEDTRAFFIC_SIMMODULEINFO << " receivable stat update: " << " own addr " << ownMACAddress << "\n";

        delete statMsg;

    } else {

        EV_INFO << KMESSENGERAPPFORDIFFERENTIATEDTRAFFIC_SIMMODULEINFO << ">!<Received unexpected packet \n";
        delete msg;
    }

}

void KMessengerAppForDifferentiatedTraffic::finish()
{
    delete appRegistrationEvent;
	timesMessagesReceived1.clear();

    if (!specificDestination || (specificDestination && !ownNodeIsDestination)) {
        cancelEvent(dataTimeoutEvent);
        delete dataTimeoutEvent;
    }

    if (messengerForDifferentiatedTrafficNodeInfoList.size() > 0) {
        for (int i = 0; i < messengerForDifferentiatedTrafficNodeInfoList.size(); i++) {
            KBaseNodeInfo* nodeInfo = messengerForDifferentiatedTrafficNodeInfoList[i];
            delete nodeInfo;
        }
        messengerForDifferentiatedTrafficNodeInfoList.clear();
    }
}
