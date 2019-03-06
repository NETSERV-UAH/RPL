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

#ifndef _RPL_SRC_NETWORKLAYER_CONTRACT_RPLDEFS_H_
#define _RPL_SRC_NETWORKLAYER_CONTRACT_RPLDEFS_H_

#define ZERO_LIFETIME 0 // This feature is used for No-Path DAO.

namespace rpl {

enum RPLMOP{
    No_Downward_Routes_maintained_by_RPL = 0,
    Non_Storing_Mode_of_Operation,
    Storing_Mode_of_Operation_with_no_multicast_support,
    Storing_Mode_of_Operation_with_multicast_support,
};

enum messagesTypes {
    UNKNOWN=0,
    DATA,
    Global_REPAIR_TIMER,
    DIO,
    SEND_DIO_TIMER,
    DIS_UNICAST,
    SEND_DIS_UNICAST_TIMER,
    DIS_FLOOD,
    SEND_DIS_FLOOD_TIMER,
    RESET_Global_REPAIR_TIMER,
    DAO,
    SEND_DAO_TIMER,
    DAO_LIFETIME_TIMER,
};


} // namespace rpl

#endif // ifndef _RPL_SRC_NETWORKLAYER_CONTRACT_RPLDEFS_H_
