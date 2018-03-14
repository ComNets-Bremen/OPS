//
// The model is the implementation of the User Behavior layer
// for Keetchi Simulator
//
// @date   : 13-07-2016
// @author : Anas bin Muslim (anas1@uni-bremen.de)
//
// Change History:
// Asanga Udugama (adu@comnets.uni-bremen.de)
// - Unwanted code, comments cleanup
// - Keywords and events files as parameters
//

package ops;

simple KUserBehavior
{
     parameters:
     
        int avoidLocationTimer @unit(s) = default(5s);
        //string isResponder = default("no");				// TO BE IMPLEMENTED/USED LATER
		int usedRNG = default(0);
		 
		// Switch to turning event driven mobility ON of OFF   (0 = OFF, 1 = ON)
		int react = default(1);
		 
		double reactToEventThreshold = default(0.5);
		 
		// prefix assigned to the app
    	string appPrefix = default("/ubm");

		// prefix used to generate names appended with a number, e.g., message-000328     	
    	string appNameGenPrefix = default("/event-");
		
		// logging at layer - 0 = disabled, 1 = enabled
        int logging = default(1);
        
        // file paths of keywords and events files
        string keywordsFilePath = default("properties.txt");
        string eventsFilePath = default("events.txt");
        
         
      	// signals for statistics received
		@signal[angryBitSignal](type="int");
		
      	// statistics collected  
        @statistic[angryBitSignal](title="Angry Bit Set"; source="angryBitSignal"; record=vector);
        
        // icon used
      	 @display("i=block/app");
      	 
	gates:
	    
	    // interfaces to the lower layer (which is application layer)
	    input lowerLayerIn;
	    output lowerLayerOut;
	    
	    input notificationGeneratorIn;

	    output mobilityOut;
}
