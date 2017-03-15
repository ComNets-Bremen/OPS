//
// Internal messages used to pass information between different
// layers.
//
// @author : Asanga Udugama (adu@comnets.uni-bremen.de)
// @date   : 16-nov-2015
//

// Message to register an application
packet KRegisterAppMsg {
    string appName;
    string prefixName;
}

// Message to provide the current neighbour list
packet KNeighbourListMsg {
    string neighbourNameList[];
    int neighbourNameCount;
}

