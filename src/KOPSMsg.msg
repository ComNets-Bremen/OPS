//
// Format of a Data packet.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 06-oct-2015
//
// example sizes in bytes
//
//               addrs | data name | goodness value | payload size | payload | valid until | final dest | hop count | total  |
//               s & d | or msg ID |  (1 byte int)  | (2 byte int) | (bytes) | (4 byte int)| (dest orie)|  (1 byte) | (bytes)|
// Keetchi  ->   6 + 6     64             1               2           512          4             6           -         601
// RRS      ->   6 + 6     64             -               2           512          4             6           -         600
// Epidemic ->   6 + 6     64             -               2           512          4             6           1         601
//

packet KDataMsg {

    // common fields
    string sourceAddress;
    string destinationAddress;
    string dataName;
    string dummyPayloadContent;
    int msgType;
    simtime_t validUntilTime = 0;
    int msgUniqueID;
    simtime_t initialInjectionTime;

    int realPayloadSize;
    int realPacketSize;

    int hopsTravelled = 0;

    // destination oriented delivery specific fields
//    string originatorNodeName;
//    string finalDestinationNodeName;
    string initialOriginatorAddress;
    string finalDestinationAddress;
    bool destinationOriented = false;

    // keetchi specific fields
    int goodnessValue = 50;

    // epidemic routing specific fields
    string messageID;
    int hopCount = 255;

    // spray & wait specific fields
    int duplicates;
}

//
// Format of a Feedback packet.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 06-oct-2015
//
//
// example sizes in bytes
//
//               addrs | data name | goodness value | total  |
//               s & d | or msg ID |  (1 byte int)  | (bytes)|
// Keetchi  ->   6 + 6     64             1             77
//

packet KFeedbackMsg {

    // common fields
    string sourceAddress;
    string destinationAddress;
    string dataName;

    int realPacketSize;

    // keetchi specific fields
    int goodnessValue;
    int feedbackType;
}

//
// Summary vector exchange message (used with Epidemic Routing).
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 02-may-2017
//
//
// example sizes in bytes
//
//                addrs | msg ID hashes  | total  |
//                s & d | 2 bytes x count| (bytes)|
// Epidemic  ->   6 + 6     2 x 10           32

packet KSummaryVectorMsg {

    // common fields
    string sourceAddress;
    string destinationAddress;

    int realPacketSize;

    // epidemic routing specific fields
    string messageIDHashVector[];  // summary vector
}

//
// Data request message (used with Epidemic Routing).
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 02-may-2017
//
//
// example sizes in bytes
//
//                addrs | msg ID hashes  | total  |
//                s & d | 2 bytes x count| (bytes)|
// Epidemic  ->   6 + 6     2 x 5            32

packet KDataRequestMsg {

    // common fields
    string sourceAddress;
    string destinationAddress;

    int realPacketSize;

    // epidemic routing specific fields
    string messageIDHashVector[];  // request vector

    //spay & wait specific
    //string originatorNodeName;
    string initialOriginatorAddress;
}

//
// Event Reaction Message (used with UBM and reactive mobility).
//
// @author : Anas bin Muslim (anas1@uni-bremen.de)
// @date   : 19-sep-2017
//
//
// example sizes in bytes
//
//               addrs | data name | reaction type  | total  |
//               s & d | or msg ID |  (1 byte int)  | (bytes)|
// Keetchi  ->   6 + 6     64             1             77

enum reactionType {
    ignore = 1;
    comment = 2;
    save = 3;
};

packet KReactionMsg {

    // common fields
    string sourceAddress;
    string destinationAddress;
    string dataName;

    // UBM specific fields
    int reaction @enum(reactionType);
}

//
// DP table request message (used with Prophet Routing).
//
//
//
//
//
// example sizes in bytes
//
//                addrs |  total  |
//                s & d | (bytes) |
// Prophet  ->    6 + 6      12


packet KDPtableRequestMsg {

    // common fields
    string sourceAddress;
    string destinationAddress;

    int realPacketSize;
}


//
// Dp table message (used with Prophet Routing).
//
//
//
//
// example sizes in bytes
//
//                addrs | dplist         | total  |
//                s & d | 14 bytes x count| (bytes)|
// Prophet  ->   6 + 6     (6+6+2)           26
struct MsgDelivPredictability {
   string nodeMACAddress;
   double nodeDP;
};

packet KDPtableDataMsg {

    // common fields
    string sourceAddress;
    string destinationAddress;

    MsgDelivPredictability dpList[];

    int realPacketSize;
}


// message used to send statistic information between models
packet KStatisticsMsg {


    // liked or non-liked data receivable info (based on bool)
    bool likedData;
    int dataCountReceivable;
    int dataBytesReceivable;

}
