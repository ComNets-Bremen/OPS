//
// The model for the OPS layer
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 01-June-2017
//
//

package ops;

simple KKeetchiLayer
{
     parameters:

        // unique address of the node
        string ownMACAddress;

        // interval for reducing (by 1) the goodness value of data (i.e., age data)
        double agingInterval @unit(s) = default(300s);

        // size of cache in bytes - zero means unlimited caching
        int maximumCacheSize @unit(byte) = default(60000000byte);

        // a value between 0.0 and 1.0 to indicate after which fraction a change
        // in neighbourhood is considered as a significant change
        // e.g. newly arrived nodes in neighbourhood = 6
        //      total number of neighbourhood nodes = 10
        //      ratio = 6/10 -> 0.6
        //      since 0.6 > 0.25 -> significant change
        double neighbourhoodChangeSignificanceThreshold = default(0.25);

        // cool off period when continuous insignificant changes are detected in the neighbourhood
        double coolOffDuration @unit(s) = default(300s);

        // the constant that gives the weight assigned to the current goodness value in cache when
        // computing the new value together with the recently arrived goodness value (in a data or
        // feedback) - a value between 0.0 - 1.0
        double learningConst = default(0.5);
        
        // the factor used to compute wait duration before sending the next data packet from cache
        // when insignificant changes are present in the neighbourhood
        double backoffTimerIncrementFactor = default(1.5);  

        // logging at layer - 0 = disabled, 1 = enabled
        int logging = default(1);

        // icon used
        @display("i=block/transport");

    gates:

        // interface to the upper layer (which may be the application)
        input upperLayerIn;
        output upperLayerOut;

        // interface to the lower layer (which may be the link adaptation layer)
        input lowerLayerIn;
        output lowerLayerOut;

}

