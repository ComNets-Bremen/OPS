//
// The model is the implementation of the Notification Generator
// for Keetchi Simulator
//
// @date   : 10-07-2016
// @author : Anas bin Muslim (anas1@uni-bremen.de)
//
//
package ops;

simple KNotificationGenerator
{
	parameters:
	
	    // NOTIFICATION GENERATION DISTRIBUTION
        // 1 = No Distribution - Constant Intervals
        // 2 = Exponential Distribution (Mean Value = notificationGenInterval)
        // 3 = Uniform Distribution
        int notificationGenDistribution = default(1);
         
	    // How often a new notification is generated
	    int interEventTimeDuration @unit(s) = default(7200s);
	    
	    // Events are to be read from file or to be created by Notification generator
	    bool readEventsFromFile = default(true);
	    
	    // Radius of location to create cicle  (Set same value as of radius in mobility model)
	    int radius @unit(m) = default(50m);
	    
	    // OMNeT RNG which is used for generation of random numbers
	    int usedRNG = default(0);
	    
	    // logging at layer - 0 = disabled, 1 = enabled
        int logging = default(1);
	    
	    // prefix assigned to the app
    	string appPrefix = default("/notifGen");

		// prefix used to generate names appended with a number, e.g., message-000328     	
    	string appNameGenPrefix = default("/message-");
    	
        // icon
        @display("i=misc/globe");
        
}

