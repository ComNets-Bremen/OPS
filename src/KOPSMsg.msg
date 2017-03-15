//
// Format of a Data packet.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 06-oct-2015
//
//
// bytes -> addresses = 6 each, data name = 64, goodness value = 2, payload size = 2 , payload = 512 -> total 592
//

packet KDataMsg {
    string sourceAddress;
    string destinationAddress;
    string dataName;
    int goodnessValue;
    int realPayloadSize;
    string dummyPayloadContent;
	int msgType;
	double validUntilTime;
        
    int realPacketSize;
    
    string finalDestinationNodeName;
    
}

//
// Format of a Feedback packet.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 06-oct-2015
//
//
// bytes -> addresses = 6 each, data name = 64, goodness value = 2 -> total 78
//

packet KFeedbackMsg {
    string sourceAddress;
    string destinationAddress;
    string dataName;
    int goodnessValue;

    int realPacketSize;
}
