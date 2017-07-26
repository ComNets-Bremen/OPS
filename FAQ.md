# Frequently Asked Questions

There have been many questions asked by users when setting up and using OPS. We have made
a list of the most widely asked questions below and the answers to them. 


`1.` Why do I get the following error when I run `./ops-simu-run.sh -m cmdenv`?

`<!> Error: Cannot open output redirection file ...log2.txt`

Was the `simulations/out` created?


`2.` The output shown on the terminal does not point to any errors, but why does the simulation end quickly?

Check the log created in `simulations/out`. It may give hints to where the problem is.


`3.` The simulation uses other models or other parameters instead of what I set. Why is that?

Was the `.opsSettings` file created in home directory? If it was created, is it pointing to
another `.ini` file? If `.opsSettings` was not created, then it uses the `.ini` file in
`settings.default`. The actual `.ini` used is shown when running `./ops-simu-run.sh` script. 


`4.` Must the given OMNeT and INET versions be used?

We have developed, tested and run the OPS models using only the given versions of OMNeT and INET. You are
on your own if you decide to use other versions. Having said that, we like to mention - you are encouraged 
to try out newer versions and tell us if there are problems. Even better - if you can find fixes for these
problems and inform us, we will be very glad to publish them and acknowledge you.


`5.` Can OPS be installed and run in Microsoft Windows environments?

NO, OPS can not be installed and run in MS Windows environments. Of course OMNeT and any model developed 
for OMNeT can be run on MS Windows, but OPS is a simulator that has been setup only to be installed and
run in `*nix` based system (e.g., Mac OSX, Linux). 


`6.` Can the result parsers (in the `parsers/` folder) be used as they are? 

Yes, they can be used as they are, provided that the text output in the log files made by the OPS models in 
`simulations/out` have not been changed. A general rule to keep in mind about the parsers is as follows.

The existing parsers we developed were meant to obtain results that WE wanted. There is a possibility that 
these results and hence, the parsers, may not suit other users' requirements for results. Therefore, 
we recommend that you use these parsers to only get an idea to develop your own parsers that serve your 
own requirements.

One more thing - the current parsers are developed using Python. You are free to develop them in any language 
you like.


`7.` Why are no packets communicated between nodes?

If you have introduced a new node model (an example of an existing node model is `KPromoteNode`), then that name
must be added to the `expectedNodeTypes` parameter in the `KWirelessInterface.ned`.


`8.` Why is it that destinations are not identified even though the destination oriented parameter is set in 
`KPromoteApp`?

If you have introduced a new node model (an example of an existing node model is `KPromoteNode`), then that name
must be added to the `expectedNodeTypes` parameter in the `KPromoteApp.ned`.


`9.` I pulled a newer version of OPS from Github (i.e., through git clone or git pull). But now, why is building of OPS
failing?

One possible reason may be that the newer version has a newer `settings.default` (i.e., a changed `settings.default`). 
If you have copied the `settings.default` of the previous version to your home folder as `.opsSettings`, remove
`.opsSettings` and copy the newer version. Caveat: this is only one possibility. 


`10.` Where can I find the `50_traces.gpx.movements` file used with the BonnMotion Mobility Model?

Send us an email if you want a copy of this file.


`11.` Why does the simulation fail/crash when using `omnetpp.ini` file?

The given `.ini` files were used in our simulations and therefore, they may not have the right parameters for 
your simulation. If you want to just get OPS running, use the `omnetpp-promote-expon.ini` file. All the 
other `.ini` files may require some changes to use.


`12.` Why does the simulation crash saying `KeetchiLib` is unavailable?

This means that the building of the `KeetchiLib` failed. This is usually due to not setting up the build 
environment properly. Read the [README](https://github.com/ComNets-Bremen/KeetchiLib/blob/master/README.md) of 
[KeetchiLib](https://github.com/ComNets-Bremen/KeetchiLib) to setup your environment to build this library. 

