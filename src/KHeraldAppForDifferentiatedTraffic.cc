//
// The model implementation for the Herald application.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de), Anna Förster (afoerster@uni-bremen.de)
// @date   : 15-aug-2016, updated 6-febr-2018
//
// HeralApp extended to handle differentiate traffic based on
// patterns, either based on random time frames or based
// the original traffic pattern in the traces.
//
// @author: Thenuka Karunatilake (thenukaramesh@gmail.com)
// @date: 01-06-2019
//

#include "KHeraldAppForDifferentiatedTraffic.h"
#include <iostream> 
#include <fstream>
#include <string>
#include "KBaseNodeInfo.h"
#include <algorithm>
#include <map>

Define_Module(KHeraldAppForDifferentiatedTraffic);

//vector<int> popularityList;
std::map<int, int> popularityList1;
std::map<int, int> myLikenesses1;

vector<KBaseNodeInfo*> heraldForDifferentiatedTrafficNodeInfoList;

int nextGenerationNotification = 0; //UniqueID for the generated msgs

void KHeraldAppForDifferentiatedTraffic::initialize(int stage)
{
    if (stage == 0) {
        nodeIndex = par("nodeIndex");
        ownMACAddress = par("ownMACAddress").stringValue();
        usedRNG = par("usedRNG");
        dataGenerationInterval = par("dataGenerationInterval");
        popularityAssignmentPercentage = par("popularityAssignmentPercentage");
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
                EV_FATAL <<  KHERALDAPPFORDIFFERENTIATEDTRAFFIC_SIMMODULEINFO << " " 
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
        
        // add own address to global list to use for random destination selections
        ownNodeInfo = new KBaseNodeInfo();
        ownNodeInfo->nodeAddress = ownMACAddress;
        ownNodeInfo->nodeHeraldAppForDifferentiatedTrafficModule = this;
        heraldForDifferentiatedTrafficNodeInfoList.push_back(ownNodeInfo);

        // setup prefix
        strcpy(prefixName, "/herald");

        // The following procedure is followed when assigning goodness values to each notification
        // - introduce popularity of messages. The “pop” value represents the percentage of the complete
        //   network which will love this packet (e.g. joke).
        // - at initialisation, create pop-values for all messages. 90% of the messages get a pop-value of
        //   0, 10% of the messages get a random pop-value between 1 and 20.
        // - after creating all the messages with their pop-values, allow each user to decide whether she
        //   likes the message or not. There are only two decisions possible: IGNORE (corresponds to a value
        //   of 0) and LOVE (correponds to a value of 100).
        // - the decision itself is taken according to Fig. 3b and Equation (2) of the UBM paper. The
        //   resulting value is compared to 90: if the value is greater or equal to 90, LOVE (100). If not, IGNORE (0).
        // - The final “like” values (only 0 and 100s possible) are stored and used later at simulation time
        //   to represent the reaction of the user and to send to Keetchi as goodness values.


        

    } else if (stage == 1) {


    } else if (stage == 2) {

        // create and setup app registration trigger
        appRegistrationEvent = new cMessage("App Registration Event");
        appRegistrationEvent->setKind(KHERALDAPPFORDIFFERENTIATEDTRAFFIC_REGISTRATION_EVENT);
        scheduleAt(simTime(), appRegistrationEvent);

        // this is a round-robin scheduling of traffic: in a row, everybody gets a chance to send the next packet.
        dataTimeoutEvent = new cMessage("Data Timeout Event");
        dataTimeoutEvent->setKind(KHERALDAPPFORDIFFERENTIATEDTRAFFIC_DATASEND_EVENT);
        // add 0.1 secs to the first sending to avoid the simulation to send one more message than expected.

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
        EV_FATAL << KHERALDAPPFORDIFFERENTIATEDTRAFFIC_SIMMODULEINFO << "Something is radically wrong\n";

    }

}

int KHeraldAppForDifferentiatedTraffic::numInitStages() const
{
    return 3;
}

void KHeraldAppForDifferentiatedTraffic::handleMessage(cMessage *msg)
{
    
    // check message
    if (msg->isSelfMessage() && msg->getKind() == KHERALDAPPFORDIFFERENTIATEDTRAFFIC_REGISTRATION_EVENT) {
        
        // send app registration message the forwarding layer
        KRegisterAppMsg *regAppMsg = new KRegisterAppMsg("Herald App Registration");
        regAppMsg->setAppName("Herald");
        regAppMsg->setPrefixName(prefixName);

        send(regAppMsg, "lowerLayerOut");

    } else if (msg->isSelfMessage() && msg->getKind() == KHERALDAPPFORDIFFERENTIATEDTRAFFIC_DATASEND_EVENT) {

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

    
        // mark this message as received by this node
        timesMessagesReceived1.push_back(nextGenerationNotification);

        // assign popularity values for the popularityAssignmentPercentage of notificationCount
        double perc = uniform(0.0, 100.0, usedRNG);
        int popularity = 0;

        if (perc <= popularityAssignmentPercentage) {
            popularity = 100;
        }

        popularityList1[nextGenerationNotification]=popularity;
    
        int like = 0;
        if (popularityList1[nextGenerationNotification] > 90) {
            like = 100;
        }
        myLikenesses1[nextGenerationNotification]=like;

        //setup the data message for sending down to forwarding layer
        char tempString[128];
        sprintf(tempString, "D item-%0d", (KHERALDAPPFORDIFFERENTIATEDTRAFFIC_START_ITEM_ID 
                                            + nextGenerationNotification));
    
        KDataMsg *dataMsg = new KDataMsg(tempString);

        //setup the data message for sending down to forwarding layer
        dataMsg->setSourceAddress("");
        dataMsg->setDestinationAddress("");

        sprintf(tempString, "/herald/item-%0d", (KHERALDAPPFORDIFFERENTIATEDTRAFFIC_START_ITEM_ID
                                                    + nextGenerationNotification));
    
        dataMsg->setDataName(tempString);
        dataMsg->setGoodnessValue(myLikenesses1[nextGenerationNotification]);
        dataMsg->setRealPayloadSize(dataSizeInBytes);
        dataMsg->setMsgUniqueID(nextGenerationNotification);
        dataMsg->setInitialInjectionTime(simTime());

        sprintf(tempString, "Details of item-%0d", (KHERALDAPPFORDIFFERENTIATEDTRAFFIC_START_ITEM_ID
                                                     + nextGenerationNotification));
        dataMsg->setDummyPayloadContent(tempString);

        dataMsg->setByteLength(dataSizeInBytes);
        dataMsg->setMsgType(0);
        dataMsg->setValidUntilTime(ttl);

        dataMsg->setInitialOriginatorAddress(ownMACAddress.c_str());
        dataMsg->setDestinationOriented(false);
        dataMsg->setFinalDestinationAddress("");

        send(dataMsg, "lowerLayerOut");

        cout<<nextGenerationNotification<<endl;
        nextGenerationNotification = nextGenerationNotification + 1;

        // generate receivable stat update to other nodes
        bool likedData = false;
        if (dataMsg->getGoodnessValue() >= 100) {
          likedData = true;
        }
        for (int i = 0; i < heraldForDifferentiatedTrafficNodeInfoList.size(); i++) {
            if (heraldForDifferentiatedTrafficNodeInfoList[i] == ownNodeInfo) {
                continue;
            }
            KStatisticsMsg *statMsg = new KStatisticsMsg("StatMsg");
            statMsg->setLikedData(likedData);
            statMsg->setDataCountReceivable(1);
            statMsg->setDataBytesReceivable(dataSizeInBytes);
            sendDirect(statMsg,
                    heraldForDifferentiatedTrafficNodeInfoList[i]->nodeHeraldAppForDifferentiatedTrafficModule,
                    "statIn");
        }

    } else if (dynamic_cast<KDataMsg*>(msg) != NULL) {
        // message received from outside so, process received data message
        KDataMsg *dataMsg = check_and_cast<KDataMsg *>(msg);
        int i = dataMsg->getMsgUniqueID();
        
        // compute stats
        if (std::find(timesMessagesReceived1.begin(),timesMessagesReceived1.end(),i)!=timesMessagesReceived1.end()) { 
            emit(duplicateDataCountReceivedSignal, (int) 1);
            emit(totalDataCountReceivedSignal, (int) 1);
            emit(duplicateDataBytesReceivedSignal, dataMsg->getRealPayloadSize());
            emit(totalDataBytesReceivedSignal, dataMsg->getRealPayloadSize());

        } else {
            timesMessagesReceived1.push_back(i); //mark this msg as received
            simtime_t diff = simTime() - dataMsg->getInitialInjectionTime();
            if (myLikenesses1[i] >= 100) {
                emit(likedDataCountReceivedSignal, (int) 1);
                emit(likedDataBytesReceivedSignal, dataMsg->getRealPayloadSize());
                emit(likedDataReceivedDelaySignal, diff);
                emit(likedDataCountReceivedForAvgDelayCompSignal, (int) 1);
                emit(likedDataCountReceivedForRatioCompSignal, (int) 1);

                emit(likedDataHopsForAvgHopsCompSignal, dataMsg->getHopsTravelled());
                emit(likedDataHopsCountForAvgHopsCompSignal, (int) 1);

                emit(likedDataReceivedDelaySignal2, diff);

            } else {
                emit(nonLikedDataCountReceivedSignal, (int) 1);
                emit(nonLikedDataBytesReceivedSignal, dataMsg->getRealPayloadSize());
                emit(nonLikedDataReceivedDelaySignal, diff);
                emit(nonLikedDataCountReceivedForAvgDelayCompSignal, (int) 1);
                emit(nonLikedDataCountReceivedForRatioCompSignal, (int) 1);

                emit(nonLikedDataHopsForAvgHopsCompSignal, dataMsg->getHopsTravelled());
                emit(nonLikedDataHopsCountForAvgHopsCompSignal, (int) 1);

                emit(nonLikedDataReceivedDelaySignal2, diff);

            }
            emit(totalDataCountReceivedSignal, (int) 1);
            emit(totalDataBytesReceivedSignal, dataMsg->getRealPayloadSize());
            emit(totalDataReceivedDelaySignal, diff);
            emit(totalDataCountReceivedForAvgDelayCompSignal, (int) 1);
            emit(totalDataCountReceivedForRatioCompSignal, (int) 1);

            emit(totalDataHopsForAvgHopsCompSignal, dataMsg->getHopsTravelled());
            emit(totalDataHopsCountForAvgHopsCompSignal, (int) 1);

            emit(totalDataReceivedDelaySignal2, diff);

            emit(totalDataCountReceivedSignal2, (int) 1);
            emit(totalDataBytesReceivedSignal2, dataMsg->getRealPayloadSize());
        }

        delete msg;

    } else if (dynamic_cast<KStatisticsMsg*>(msg) != NULL) {
        
        // message received from outside so, process received statistics message
        KStatisticsMsg *statMsg = check_and_cast<KStatisticsMsg *>(msg);

        // update receivables
        if (statMsg->getLikedData()) {
            
            emit(likedDataCountMaxReceivableSignal, statMsg->getDataCountReceivable());
            emit(likedDataBytesMaxReceivableSignal, statMsg->getDataBytesReceivable());
            emit(likedDataCountMaxReceivableForRatioCompSignal, statMsg->getDataCountReceivable());
        } else {
            
            emit(nonLikedDataCountMaxReceivableSignal, statMsg->getDataCountReceivable());
            emit(nonLikedDataBytesMaxReceivableSignal, statMsg->getDataBytesReceivable());
            emit(nonLikedDataCountMaxReceivableForRatioCompSignal, statMsg->getDataCountReceivable());
        }
        emit(totalDataCountMaxReceivableSignal, statMsg->getDataCountReceivable());
        emit(totalDataBytesMaxReceivableSignal, statMsg->getDataBytesReceivable());
        emit(totalDataCountMaxReceivableForRatioCompSignal, statMsg->getDataCountReceivable());

        delete statMsg;

    } else {

        EV_INFO << KHERALDAPPFORDIFFERENTIATEDTRAFFIC_SIMMODULEINFO << ">!<Received unexpected packet \n";
        delete msg;
    }
}

void KHeraldAppForDifferentiatedTraffic::finish()
{
    delete appRegistrationEvent;

    cancelEvent(dataTimeoutEvent);
    delete dataTimeoutEvent;

//    myLikenesses.clear();
    myLikenesses1.clear();
    popularityList1.clear();
//    if (popularityList.size() > 0) {
//        popularityList.clear();
//    }
    timesMessagesReceived.clear();
    timesMessagesReceived1.clear();

    if (heraldForDifferentiatedTrafficNodeInfoList.size() > 0) {
        for (int i = 0; i < heraldForDifferentiatedTrafficNodeInfoList.size(); i++) {
            KBaseNodeInfo* nodeInfo = heraldForDifferentiatedTrafficNodeInfoList[i];
            delete nodeInfo;
        }
        heraldForDifferentiatedTrafficNodeInfoList.clear();
    }
}
