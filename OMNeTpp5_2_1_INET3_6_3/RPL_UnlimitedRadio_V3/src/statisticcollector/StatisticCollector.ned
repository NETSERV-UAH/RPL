// Copyright (C) 2018 Elisa Rojas(1), Hedayat Hosseini(2), Carles Gomez(3);
//                    (1) GIST, University of Alcala, Spain.
//                    (2) CEIT, Amirkabir University of Technology (Tehran Polytechnic), Iran.
//                    (3) UPC, Castelldefels, Spain.
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

package rpl.src.statisticcollector;

// This module is an UDP flow traffic generator. You can configure a generation frequency (lambda) and 
// this will determine every time a new flow is created by the generator. The generator operates
// with the UDPFlowHost modules at every host (StandardHostNew) in the network. Every time a new 
// flow is created, the generator indicates the source host to start the flow (and its parameters)

simple StatisticCollector
{
    parameters:
        @class(rpl::StatisticCollector);
        @display("i=block/cogwheel_s");
        @labels(node);
}
