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
    double validUntilTime = 0;

    int realPayloadSize;
    int realPacketSize;
    
    int hopsTravelled = 0;

    // destination oriented delivery specific fields
    string originatorNodeName;
    string finalDestinationNodeName;
    bool destinationOriented = false;

    // keetchi specific fields
    int goodnessValue = 50;

    // epidemic routing specific fields
    string messageID;
    int hopCount = 255;
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
}
