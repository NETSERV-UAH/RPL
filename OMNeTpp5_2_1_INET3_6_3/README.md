# RPL

Copyright (C) 2018 Elisa Rojas(1), Hedayat Hosseini(2), Carles Gomez(3);

                     (1) GIST, University of Alcala, Spain.
                     
                     (2) CEIT, Amirkabir University of Technology (Tehran Polytechnic), Iran.
                     
                     (3) UPC, Castelldefels, Spain.

                     
Developed in OMNet++5.2.1, based on INET framework.

LAST UPDATE OF THE INET FRAMEWORK: inet3.6.3 @ December 22, 2017

The repository includes several implementations of the RPL protocol. Details of each implementation are in the README file in the respective folder.

The Upward operation, using DIO and DIS Messages, is according to the Implementation of the IPv6 Routing Protocol for Low power and Lossy Networks (RPL) [1] on the MiXim framework written by Hamidreza Kermajani ((C) 2013 UPC, Castelldefels, Spain). We have adapted the Kermajani's simulation for using on the INET 3.6.3 framework, and also included some changes such as using ICMPv6 messages for transmitting the RPL control messages, the DAO message for applying the downward routes, the interface table, and the lifesycle modules. To read more information about the Kermajani's article, you can use [1].

                    [1] Kermajani, Hamidreza, and Carles Gomez. "On the network convergence process
                    in RPL over IEEE 802.15. 4 multihop networks: Improvement and trade-offs."
                    Sensors 14.7 (2014): 11993-12022.


