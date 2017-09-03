//
// The model for the Event Notifier (Bruit) application.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 18-sep-2015
//
//

package ops;

simple KBruitApp
{
     parameters:
         
         // assigned ID for the node
         int nodeIndex;
         
        // operation modes is used to configure different data generation modes
        // 1 = normal mode -> where each node randomly generates data and feedback
        // 2 = a single node generates a given number of data items and all randomly 
        //     generate feedback
        int operationMode = default(1);
        
        // parameters for operationMode 2
        // ID of the node that generates the data
        int dataGeneratingNodeIndex = default(-1);
        
        // number of data items generated
        int numberOfData = default(5);
         
		// prefix assigned to the app
    	string appPrefix = default("/bruit");

		// prefix used to generate names appended with a number, e.g., /bruit/item-000328     	
    	string appNameGenPrefix = default("item-");
    	
    	// how often a new name (e.g., /bruit/item-000328) is generated
        volatile double dataGenerationInterval  @unit(s) = default(5s);

    	// how often a feedback is generated for a previously recived data 
        volatile double feedbackGenerationInterval  @unit(s) = default(60s);
        
        // logging at layer - 0 = disabled, 1 = enabled
        int logging = default(1);

		// signals for statistics received
		@signal[dataSendSignal](type="int");
		@signal[dataReceiveSignal](type="int");
		@signal[feedbackSendSignal](type="int");
		
		// statistics collected  
		@statistic[dataSendSignal](title="Data Sent"; source="dataSendSignal"; record=vector);
		@statistic[dataReceiveSignal](title="Data Received"; source="dataReceiveSignal"; record=vector);
		@statistic[feedbackSendSignal](title="Feedback Sent"; source="feedbackSendSignal"; record=vector);
		
        // icon used
      	@display("i=block/app");
        
	gates:
	    
	    // interfaces to the lower layer
	    input lowerLayerIn;
	    output lowerLayerOut;

}
