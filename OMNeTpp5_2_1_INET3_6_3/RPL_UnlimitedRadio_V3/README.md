# RPL


Copyright (C) 2018 Elisa Rojas(1), Hedayat Hosseini(2), Carles Gomez(3);

                     (1) GIST, University of Alcala, Spain.
                     
                     (2) CEIT, Amirkabir University of Technology (Tehran Polytechnic), Iran.
                     
                     (3) UPC, Castelldefels, Spain.

                     
Developed in OMNet++5.2.1, based on INET framework.

LAST UPDATE OF THE INET FRAMEWORK: inet3.6.3 @ December 22, 2017


In the simulation, the Mac sub layer and the physical layer in the simulation are unlimited in terms of interference, collision, and simultaneously sending and receiving one/some packet(s). The limitation in receiving messages is only the transmission range of a transmitter node.

To read more information about the Kermajani's article, you can use [1].

 We have added the following changes to the new version of the simulation:

1. Customizing the Neighbor Discovery to handle/process the RPL control messages.
1. Creating a separate module for the Parent table. In the previous simulation, all parents were saved by a link list in the RPL routing module.
1. Using the OMNeT++ routing table. In the first simulation, there was not any routing table. In our previous version, we used a map STL to handle the routing table in the RPL routing module.
1. Creating a module to collect the statistics results. In the previous simulation, this part was in the RPL routing module.
1. Splitting other parts of the RPL routing module. For example, the DAO and DIS operations are accommodated in the ICMPv6 module, and we only kept the Upward routing in the RPL routing module and changed its name to RPLUPwardRouting.

                    [1] Kermajani, Hamidreza, and Carles Gomez. "On the network convergence process
                    in RPL over IEEE 802.15. 4 multihop networks: Improvement and trade-offs."
                    Sensors 14.7 (2014): 11993-12022.þ

