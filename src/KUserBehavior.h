//
// The model is the implementation of the User Behavior layer
// for OPS Simulator
//
// @date   : 13-07-2016
// @author : Anas bin Muslim (anas1@uni-bremen.de)
//
// Change History:
// Asanga Udugama (adu@comnets.uni-bremen.de)
// - Unwanted code, comments cleanup
// - Locations and events files as parameters
//

#ifndef KUSERBEHAVIOR_H_
#define KUSERBEHAVIOR_H_

#include "inet/mobility/single/ExtendedSWIMMobility.h"
#include "KOPSMsg_m.h"
#include "KInternalMsg_m.h"

#if OMNETPP_VERSION >= 0x500
using namespace omnetpp;
#endif

struct event{	
	bool emergencyEvent = false;
	reactionType reaction;               // Reactions : 1=ignore; 2=comment/vote; 3=save
	float radius = 0, popularity = 0;
	double startTime = 0, endTime = 0;
	inet::Coord eventCoord;
	std::vector <std::string> properties;
	std::string rawDescription;
};
	
using namespace std;

class KUserBehavior : public cSimpleModule
{
    protected:
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual int numInitStages() const;
        virtual void finish();
        
        virtual void moveAway();
        
    public:
        virtual bool computePreReactions();
        
        virtual event computeEventReaction(event);
        
        virtual void reactToEvent(event);
        
        virtual void sendReactionMsg(reactionType, std::string);
        
        virtual bool getKeywords();
    private:
		
        int usedRNG;
        int react;                            // User defined value in NED file; 1 = react to events, 2 = don't react to events
        int notificationCount;
        
        double reactToEventThreshold;         // Throws a dice. Reacts if dice gives value more than threshold
        
		std::string isResponder;			  // Reserved for extended functionality
		string appPrefix;                     // Prefix to be appended in the message names so the message will reach its specific app layer application 
        string appNameGenPrefix;              // Same as appPrefix but it is an application Name
		
		std::vector <std::string> eventProperties; 		//stores all keywords that an event can have; used for selecting random properties for user
		std::vector <event> notifications; 				//to compute the reactions based on keyword matching
		std::vector <std::string> properties;			//Priorities of a user
		
        simsignal_t angryBitSignal;
        
        int logging;
        
        string keywordsFilePath;               // path of the keywords file
        string eventsFilePath;                 // path of the events file
        
};

#define KUSERBEHAVIOR_SIMMODULEINFO       " KUserBehavior>!<" << simTime() << ">!<" << this->getParentModule()->getFullName()

#endif /* KUSERBEHAVIOR_H_ */
