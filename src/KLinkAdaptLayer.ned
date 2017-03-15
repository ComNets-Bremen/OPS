//
// The model for link adaptations between the used link 
// technology (below) and the upper layer.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 12-oct-2015
//
//

package ops;

simple KLinkAdaptLayer
{
    parameters:
        
        // own address
        string ownMACAddress;
        
		// signals for statistics received
		@signal[dataSendSignal](type="int");
		@signal[dataReceiveSignal](type="int");
		@signal[feedbackSendSignal](type="int");
		@signal[feedbackReceiveSignal](type="int");
		
		// statistics collected  
		@statistic[dataSendSignal](title="Data Sent"; source="dataSendSignal"; record=vector);
		@statistic[dataReceiveSignal](title="Data Received"; source="dataReceiveSignal"; record=vector);
		@statistic[feedbackSendSignal](title="Feedback Sent"; source="feedbackSendSignal"; record=vector);
		@statistic[feedbackReceiveSignal](title="Feedback Received"; source="feedbackReceiveSignal"; record=vector);

      	@display("i=block/layer");
        
    gates:
        // interface to the upper layer (forwarding layer)
        input upperLayerIn;
        output upperLayerOut;
        
        // interface to the lower layer (which may be the link technology used, e.g., emulated, WiFi adhoc, WiFi direct, etc.)
        input lowerLayerIn;
        output lowerLayerOut;
    
}
