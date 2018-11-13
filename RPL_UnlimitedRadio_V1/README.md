# RPL

Copyright (C) 2018 Elisa Rojas(1), Hedayat Hosseini(2), Carles Gomez(3);

                     (1) GIST, University of Alcala, Spain.
                     
                     (2) CEIT, Amirkabir University of Technology (Tehran Polytechnic), Iran.
                     
                     (3) UPC, Castelldefels, Spain.

                     
Developed in OMNet++5.2.1, based on INET framework.

LAST UPDATE OF THE INET FRAMEWORK: inet3.6.3 @ December 22, 2017


In the simulation, the Mac sub layer and the physical layer in the simulation are unlimited in terms of interference, collision, and simultaneously sending and receiving one/some packet(s). The limitation in receiving messages is only the transmission range of a transmitter node. This simulation just implements the Upward routes.

The Upward operation, using DIO and DIS Messages, is according to the Implementation of the IPv6 Routing Protocol for Low power and Lossy Networks (RPL) [1] on the MiXim framework written by Hamidreza Kermajani ((C) 2013 UPC, Castelldefels, Spain). We have adapted the Kermajani's simulation for using on the INET 3.6.3 framework, and also included some changes such as using ICMPv6 messages for transmitting the RPL control messages, the DAO message for applying the Downward routes, the interface table, and the lifesycle modules. To read more information about the Kermajani's article, you can use [1].

                    [1] Kermajani, Hamidreza, and Carles Gomez. "On the network convergence process
                    in RPL over IEEE 802.15. 4 multihop networks: Improvement and trade-offs."
                    Sensors 14.7 (2014): 11993-12022.


### Compile and run ###

To compile and run the project, you can follow the next steps.

1. After extracting and before installing OMNeT++, exchange "PREFER_CLANG=yes" to "PREFER_CLANG=no" in the "configure.user" file in your OMNeT++ installation folder.
1. Install OMNeT++.
1. To check if your OMNeT++ correctly work, run an example of OMNeT++ such as dyna, aloha, tictoc, or etc.
1. Install and build INET.
1. To check if your INET correctly work, run an example of INET such as inet/examples/adhoc/ieee80211, or etc.
1. Since our files only include implementation code, the project may not be probably imported by IDE, and you must manually create a new OMNeT++ project by the same name. So, do the next substeps.
   1. Click on the menu of "File".
   1. Select "New".
   1. Select "OMNeT++ Project".
   1. Type the name of project.
   1. Click on the "Next".
   1. Select "Empty Project".
   1. click on the "Finish".
1. Copy all the files/folders in this folder to the project folder you created.
1. You must introduce INET to your project as a reference/library, so you must do the next substeps.
   1. Right-click on the project in "Project Explorer" window.
   1. Select "Properties".
   1. Select "Project Reference" in the left list.
   1. Select "INET" in the right list.
   1. Click on the "Apply and close".
1. Build and run the project.
