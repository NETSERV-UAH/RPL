//
// Copyright (C) 2018 Elisa Rojas(1), Hedayat Hosseini(2), Carles Gomez(3);
//                    (1) GIST, University of Alcala, Spain.
//                    (2) CEIT, Amirkabir University of Technology (Tehran Polytechnic), Iran.
//                    (3) UPC, Castelldefels, Spain.
//                    INET 3.6.3, adapted for using the RPL routing protocol
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package rpl.simulations.article;

import inet.networklayer.configurator.ipv6.FlatNetworkConfigurator6;
import inet.physicallayer.ieee802154.packetlevel.Ieee802154NarrowbandScalarRadioMedium;
import inet.visualizer.contract.IIntegratedVisualizer;
import rpl.src.simulationManager.managerRPL;

import rpl.src.node.rpl.RPLRouter;
//import rpl.src.simulationmodels.flowmodels.UDPFlowGenerator;


network Net802154
{
    parameters:
        int numHosts;
    submodules:
        visualizer: <default("IntegratedCanvasVisualizer")> like IIntegratedVisualizer if hasVisualizer() {
            parameters:
                @display("p=100,10");
        }
    //    generator: UDPFlowGenerator {
      //      @display("p=100,20");
       // }
        configurator: FlatNetworkConfigurator6 {
            parameters:
                @display("p=100,20");
        }
        managerRPL: managerRPL {
            parameters:
                @display("p=100,30");
        }

        radioMedium: Ieee802154NarrowbandScalarRadioMedium {
            parameters:
                @display("p=100,40");
        }
        host[numHosts]: RPLRouter {
            parameters:
                @display("r=,,#707070;p=146,58");
        }
}
