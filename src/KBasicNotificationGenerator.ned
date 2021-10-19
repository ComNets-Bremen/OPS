//
// The model is the implementation of the Notification Generator
// for Keetchi Simulator
//
// @date   : 10-07-2016
// @author : Anas bin Muslim (anas1@uni-bremen.de)
//
// Change History:
// Asanga Udugama (adu@comnets.uni-bremen.de)
// - Unwanted code, comments cleanup
// - Locations and events files as parameters
// - use of interface
// - change of class name
// - data size parameter
//
package ops;

simple KBasicNotificationGenerator like INotificationGenerator
{
	parameters:
	
	    // NOTIFICATION GENERATION DISTRIBUTION
        // 1 = No Distribution - Constant Intervals
        // 2 = Exponential Distribution (Mean Value = notificationGenInterval)
        // 3 = Uniform Distribution (Mean Value = notificationGenInterval)
        int notificationGenDistribution = default(1);
         
	    // How often a new notification is generated
	    int interEventTimeDuration @unit(s) = default(7200s);
	    
	    // Radius of location to create cicle  (Set same value as of radius in mobility model)
	    int radius @unit(m) = default(50m);
	    
	    // OMNeT RNG which is used for generation of random numbers
	    int usedRNG = default(0);
	    
//	    // logging at layer - 0 = disabled, 1 = enabled
//        int logging = default(1);
	    
	    // prefix assigned to the app
    	string appPrefix = default("/notifGen");

		// prefix used to generate names appended with a number, e.g., message-000328     	
    	string appNameGenPrefix = default("/message-");
        
        // file paths of locations and events files
        string locationsFilePath = default("locations.txt");
        string eventsFilePath = default("events.txt");

        // data size
        int dataSizeInBytes = default(10000);
    	
        // icon
        @display("i=misc/globe");

        // statistics and signals
        @signal[genTotalReceivable](type=long);




}

