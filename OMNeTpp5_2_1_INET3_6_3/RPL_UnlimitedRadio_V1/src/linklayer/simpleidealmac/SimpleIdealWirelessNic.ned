//
// Copyright (C) 2014 Florian Meier
// Copyright (C) 2013 OpenSim Ltd.
//
//
// *
// * Copyright (C) 2018 Elisa Rojas(1), Hedayat Hosseini(2);
// *                    (1) GIST, University of Alcala, Spain.
// *                    (2) CEIT, Amirkabir University of Technology (Tehran Polytechnic), Iran.
// *                    Develpoed on OMNeT++ 5.2.1 & INET 3.6.3 for using in the IoTorii protocol
//

//*
// * Copyright (C) 2018 Elisa Rojas(1), Hedayat Hosseini(2), Carles Gomez(3);
// *                    (1) GIST, University of Alcala, Spain.
// *                    (2) CEIT, Amirkabir University of Technology (Tehran Polytechnic), Iran.
// *                    (3) UPC, Castelldefels, Spain.
//*                    Develpoed on OMNeT++ 5.2.1 & INET 3.6.3 by using their proposed interfaces to adapt for using in the RPL and other wireless protocols
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

package rpl.src.linklayer.simpleidealmac;

import inet.linklayer.contract.IWirelessNic;
import inet.physicallayer.contract.packetlevel.IRadio;

module SimpleIdealWirelessNIC like IWirelessNic
{
    parameters:
        string interfaceTableModule;
        string radioType = default("SimpleIdealRadio");
        *.interfaceTableModule = default(absPath(interfaceTableModule));
        
        @display("i=block/ifcard");
        @nic; 
        
    gates:
        input upperLayerIn;
        output upperLayerOut;
        input radioIn @labels(IRadioFrame);
    submodules:
        mac: SimpleIdealWirelessMAC {
            parameters:
                @display("p=100,150");
        }
        radio: <radioType> like IRadio {
            parameters:
                @display("p=100,250");
        }
    connections:
        upperLayerIn --> mac.upperLayerIn;
        mac.lowerLayerOut --> radio.upperLayerIn;
        mac.upperLayerOut --> { @display("m=n"); } --> upperLayerOut;
        radioIn --> { @display("m=s"); } --> radio.radioIn;
        radio.upperLayerOut --> mac.lowerLayerIn;
}
