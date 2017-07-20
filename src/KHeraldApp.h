//
// The model implementation for the Herald application.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 15-aug-2016
//
//

#ifndef KHERALDAPP_H_
#define KHERALDAPP_H_

#include <omnetpp.h>
#include <cstdlib>
#include <ctime>
#include <list>
#include <string>

#include "KOPSMsg_m.h"
#include "KInternalMsg_m.h"

#if OMNETPP_VERSION >= 0x500
using namespace omnetpp;
#endif


using namespace std;


#define KHERALDAPP_SIMMODULEINFO        " :: " << simTime() << " :: " << getParentModule()->getFullName() << " :: KHeraldApp"
#define TRUE                            1
#define FALSE                           0


class KHeraldApp : public cSimpleModule
{
    protected:
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual int numInitStages() const;
        virtual void finish();

    private:

        double totalSimulationTime;
        char prefixName[128] = "/herald";

        struct NotificationItem {
            int itemNumber;
            char dataName[128];
            char dummyPayloadContent[128];
            char dataMsgName[128];
            char feedbackMsgName[128];

            int goodnessValue;
            int realPayloadSize;
            int dataByteLength;
            int feedbackByteLength;

            double validUntilTime;

            int timesDataMsgReceived;
            int feedbackGenerated;

        };

        int nodeIndex;
        int dataGeneratingNodeIndex;
        double dataGenerationInterval;
        double feedbackGenerationInterval;
        double maxFeedbackGenerationInterval;

        int notificationCount;
        struct NotificationItem notificationItem[100] = {

            {22200, "/herald/item-22200", "Details of item-22200", "D item-22200", "F item-22200", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22201, "/herald/item-22201", "Details of item-22201", "D item-22201", "F item-22201", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22202, "/herald/item-22202", "Details of item-22202", "D item-22202", "F item-22202", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22203, "/herald/item-22203", "Details of item-22203", "D item-22203", "F item-22203", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22204, "/herald/item-22204", "Details of item-22204", "D item-22204", "F item-22204", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22205, "/herald/item-22205", "Details of item-22205", "D item-22205", "F item-22205", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22206, "/herald/item-22206", "Details of item-22206", "D item-22206", "F item-22206", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22207, "/herald/item-22207", "Details of item-22207", "D item-22207", "F item-22207", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22208, "/herald/item-22208", "Details of item-22208", "D item-22208", "F item-22208", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22209, "/herald/item-22209", "Details of item-22209", "D item-22209", "F item-22209", 0, 512, 592, 78, 2071531.0, 0, FALSE},

            {22210, "/herald/item-22210", "Details of item-22210", "D item-22210", "F item-22210", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22211, "/herald/item-22211", "Details of item-22211", "D item-22211", "F item-22211", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22212, "/herald/item-22212", "Details of item-22212", "D item-22212", "F item-22212", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22213, "/herald/item-22213", "Details of item-22213", "D item-22213", "F item-22213", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22214, "/herald/item-22214", "Details of item-22214", "D item-22214", "F item-22214", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22215, "/herald/item-22215", "Details of item-22215", "D item-22215", "F item-22215", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22216, "/herald/item-22216", "Details of item-22216", "D item-22216", "F item-22216", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22217, "/herald/item-22217", "Details of item-22217", "D item-22217", "F item-22217", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22218, "/herald/item-22218", "Details of item-22218", "D item-22218", "F item-22218", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22219, "/herald/item-22219", "Details of item-22219", "D item-22219", "F item-22219", 0, 512, 592, 78, 2071531.0, 0, FALSE},

            {22220, "/herald/item-22220", "Details of item-22220", "D item-22220", "F item-22220", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22221, "/herald/item-22221", "Details of item-22221", "D item-22221", "F item-22221", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22222, "/herald/item-22222", "Details of item-22222", "D item-22222", "F item-22222", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22223, "/herald/item-22223", "Details of item-22223", "D item-22223", "F item-22223", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22224, "/herald/item-22224", "Details of item-22224", "D item-22224", "F item-22224", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22225, "/herald/item-22225", "Details of item-22225", "D item-22225", "F item-22225", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22226, "/herald/item-22226", "Details of item-22226", "D item-22226", "F item-22226", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22227, "/herald/item-22227", "Details of item-22227", "D item-22227", "F item-22227", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22228, "/herald/item-22228", "Details of item-22228", "D item-22228", "F item-22228", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22229, "/herald/item-22229", "Details of item-22229", "D item-22229", "F item-22229", 0, 512, 592, 78, 2071531.0, 0, FALSE},

            {22230, "/herald/item-22230", "Details of item-22230", "D item-22230", "F item-22230", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22231, "/herald/item-22231", "Details of item-22231", "D item-22231", "F item-22231", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22232, "/herald/item-22232", "Details of item-22232", "D item-22232", "F item-22232", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22233, "/herald/item-22233", "Details of item-22233", "D item-22233", "F item-22233", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22234, "/herald/item-22234", "Details of item-22234", "D item-22234", "F item-22234", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22235, "/herald/item-22235", "Details of item-22235", "D item-22235", "F item-22235", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22236, "/herald/item-22236", "Details of item-22236", "D item-22236", "F item-22236", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22237, "/herald/item-22237", "Details of item-22237", "D item-22237", "F item-22237", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22238, "/herald/item-22238", "Details of item-22238", "D item-22238", "F item-22238", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22239, "/herald/item-22239", "Details of item-22239", "D item-22239", "F item-22239", 0, 512, 592, 78, 2071531.0, 0, FALSE},

            {22240, "/herald/item-22240", "Details of item-22240", "D item-22240", "F item-22240", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22241, "/herald/item-22241", "Details of item-22241", "D item-22241", "F item-22241", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22242, "/herald/item-22242", "Details of item-22242", "D item-22242", "F item-22242", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22243, "/herald/item-22243", "Details of item-22243", "D item-22243", "F item-22243", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22244, "/herald/item-22244", "Details of item-22244", "D item-22244", "F item-22244", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22245, "/herald/item-22245", "Details of item-22245", "D item-22245", "F item-22245", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22246, "/herald/item-22246", "Details of item-22246", "D item-22246", "F item-22246", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22247, "/herald/item-22247", "Details of item-22247", "D item-22247", "F item-22247", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22248, "/herald/item-22248", "Details of item-22248", "D item-22248", "F item-22248", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22249, "/herald/item-22249", "Details of item-22249", "D item-22249", "F item-22249", 0, 512, 592, 78, 2071531.0, 0, FALSE},

            {22250, "/herald/item-22250", "Details of item-22250", "D item-22250", "F item-22250", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22251, "/herald/item-22251", "Details of item-22251", "D item-22251", "F item-22251", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22252, "/herald/item-22252", "Details of item-22252", "D item-22252", "F item-22252", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22253, "/herald/item-22253", "Details of item-22253", "D item-22253", "F item-22253", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22254, "/herald/item-22254", "Details of item-22254", "D item-22254", "F item-22254", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22255, "/herald/item-22255", "Details of item-22255", "D item-22255", "F item-22255", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22256, "/herald/item-22256", "Details of item-22256", "D item-22256", "F item-22256", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22257, "/herald/item-22257", "Details of item-22257", "D item-22257", "F item-22257", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22258, "/herald/item-22258", "Details of item-22258", "D item-22258", "F item-22258", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22259, "/herald/item-22259", "Details of item-22259", "D item-22259", "F item-22259", 0, 512, 592, 78, 2071531.0, 0, FALSE},

            {22260, "/herald/item-22260", "Details of item-22260", "D item-22260", "F item-22260", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22261, "/herald/item-22261", "Details of item-22261", "D item-22261", "F item-22261", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22262, "/herald/item-22262", "Details of item-22262", "D item-22262", "F item-22262", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22263, "/herald/item-22263", "Details of item-22263", "D item-22263", "F item-22263", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22264, "/herald/item-22264", "Details of item-22264", "D item-22264", "F item-22264", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22265, "/herald/item-22265", "Details of item-22265", "D item-22265", "F item-22265", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22266, "/herald/item-22266", "Details of item-22266", "D item-22266", "F item-22266", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22267, "/herald/item-22267", "Details of item-22267", "D item-22267", "F item-22267", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22268, "/herald/item-22268", "Details of item-22268", "D item-22268", "F item-22268", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22269, "/herald/item-22269", "Details of item-22269", "D item-22269", "F item-22269", 0, 512, 592, 78, 2071531.0, 0, FALSE},

            {22270, "/herald/item-22270", "Details of item-22270", "D item-22270", "F item-22270", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22271, "/herald/item-22271", "Details of item-22271", "D item-22271", "F item-22271", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22272, "/herald/item-22272", "Details of item-22272", "D item-22272", "F item-22272", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22273, "/herald/item-22273", "Details of item-22273", "D item-22273", "F item-22273", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22274, "/herald/item-22274", "Details of item-22274", "D item-22274", "F item-22274", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22275, "/herald/item-22275", "Details of item-22275", "D item-22275", "F item-22275", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22276, "/herald/item-22276", "Details of item-22276", "D item-22276", "F item-22276", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22277, "/herald/item-22277", "Details of item-22277", "D item-22277", "F item-22277", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22278, "/herald/item-22278", "Details of item-22278", "D item-22278", "F item-22278", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22279, "/herald/item-22279", "Details of item-22279", "D item-22279", "F item-22279", 0, 512, 592, 78, 2071531.0, 0, FALSE},

            {22280, "/herald/item-22280", "Details of item-22280", "D item-22280", "F item-22280", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22281, "/herald/item-22281", "Details of item-22281", "D item-22281", "F item-22281", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22282, "/herald/item-22282", "Details of item-22282", "D item-22282", "F item-22282", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22283, "/herald/item-22283", "Details of item-22283", "D item-22283", "F item-22283", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22284, "/herald/item-22284", "Details of item-22284", "D item-22284", "F item-22284", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22285, "/herald/item-22285", "Details of item-22285", "D item-22285", "F item-22285", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22286, "/herald/item-22286", "Details of item-22286", "D item-22286", "F item-22286", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22287, "/herald/item-22287", "Details of item-22287", "D item-22287", "F item-22287", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22288, "/herald/item-22288", "Details of item-22288", "D item-22288", "F item-22288", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22289, "/herald/item-22289", "Details of item-22289", "D item-22289", "F item-22289", 0, 512, 592, 78, 2071531.0, 0, FALSE},

            {22290, "/herald/item-22290", "Details of item-22290", "D item-22290", "F item-22290", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22291, "/herald/item-22291", "Details of item-22291", "D item-22291", "F item-22291", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22292, "/herald/item-22292", "Details of item-22292", "D item-22292", "F item-22292", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22293, "/herald/item-22293", "Details of item-22293", "D item-22293", "F item-22293", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22294, "/herald/item-22294", "Details of item-22294", "D item-22294", "F item-22294", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22295, "/herald/item-22295", "Details of item-22295", "D item-22295", "F item-22295", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22296, "/herald/item-22296", "Details of item-22296", "D item-22296", "F item-22296", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22297, "/herald/item-22297", "Details of item-22297", "D item-22297", "F item-22297", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22298, "/herald/item-22298", "Details of item-22298", "D item-22298", "F item-22298", 0, 512, 592, 78, 2071531.0, 0, FALSE},
            {22299, "/herald/item-22299", "Details of item-22299", "D item-22299", "F item-22299", 0, 512, 592, 78, 2071531.0, 0, FALSE}

        };

        int lastGeneratedNotification = -1;
        int usedRNG;

        cMessage *appRegistrationEvent;
        cMessage *dataTimeoutEvent;
        cMessage *feedbackTimeoutEvent;

};


#endif /* KHERALDAPP_H_ */
