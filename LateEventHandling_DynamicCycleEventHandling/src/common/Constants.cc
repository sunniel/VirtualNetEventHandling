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

#include "Constants.h"

const char* DISPLAY_STR_CH_TEMP = "m=auto;ls=red,3,dashed";
const char* DISPLAY_STR_CH_HIDDEN = "ls=,0";
const char* DISPLAY_STR_CH_LINK = "m=auto;ls=black,1,solid";

const char* DISPLAY_STR_MSG_GROUP = "b=10,10,oval,black,kind,0";
const char* DISPLAY_STR_MSG_GROUP_CC = "b=10,10,oval,blue,kind,0";
const char* DISPLAY_STR_MSG_GROUP_GR = "b=10,10,oval,yellow,kind,0";
const char* DISPLAY_STR_MSG_GROUP_EH = "b=10,10,oval,cyan,kind,0";
const char* DISPLAY_STR_MSG_HIDDEN = "b=-1,-1,,green,kind,3";

const char* DISPLAY_STR_MOD_HIDDEN = "i=block/downarrow";

const char* CH_TYPE_FAIR_LOSS = "common.FairLossChannel";

const char* event::MISS_EVENT = "0x22A5";
const char* event::EMPTY_EVENT = "Empty";
const char* event::HANDSHAKE = "HANDSHAKE";
const char* event::HANDSHAKE_REPLY = "HANDSHAKE_REPLY";

// consensus service
const char* msg::COORD_CHANGE = "COORD_CHANGE";
const char* msg::CC_NACK = "CC_NACK";
const char* msg::COORD_CHANGE_REPLY = "COORD_CHANGE_REPLY";
const char* msg::LOAD_COORD = "LOAD_COORD";
const char* msg::GROUP_RECONFIG = "GROUP_RECONFIG";
const char* msg::RECONFIG_READY = "RECONFIG_READY";
const char* msg::LOAD_CONFIG = "LOAD_CONFIG";
const char* msg::PROPOSAL = "PROPOSAL";
const char* msg::DECISION = "DECISION";
const char* msg::AGREEMENT = "AGREEMENT";
const char* msg::FINAL = "FINAL";
const char* msg::CYCLE_EVENT = "CYCLE_EVENT";
const char* msg::ROUND_QUERY = "ROUND_QUERY";
const char* msg::ROUND_QUERY_REPLY = "ROUND_QUERY_REPLY";
const char* msg::CONSENSUS_QUERY = "CONSENSUS_QUERY";
const char* msg::GOSSIP_GC = "GOSSIP_GC";
const char* msg::EVENT_APPLY = "EVENT_APPLY";
const char* msg::UPDATE_APPLY = "UPDATE_APPLY";

// messages between client and hosts
const char* msg::EVENT = "EVENT";
const char* msg::UPDATE = "UPDATE";
const char* msg::JOIN = "JOIN";
const char* msg::INIT_JOIN = "INIT_JOIN";

const char* msg::RT_EVENT = "RT_EVENT";

// initialization
const char* msg::HOST_INIT = "HOST_INIT";
const char* msg::HEARTBEAT_INIT = "HEARTBEAT_INIT";
const char* msg::RENDEZVOUS_INIT = "RENDEZVOUS_INIT";
const char* msg::CLIENT_INIT = "CLIENT_INIT";
const char* msg::PURGER_INIT = "PURGER_INIT";
const char* msg::GROUP_INIT = "GROUP_INIT";

// timeout event
const char* msg::HEARTBEAT_TIMEOUT = "HEARTBEAT_TIMEOUT";
const char* msg::EH_TIMEOUT = "EH_TIMEOUT";
const char* msg::EVENT_CYCLE = "EVENT_CYCLE";
const char* msg::CONNECT_TIMEOUT = "CONNECT_TIMEOUT";
const char* msg::FAILURE = "FAILURE";
const char* msg::UPDATE_CYCLE = "UPDATE_CYCLE";
const char* msg::NOTIFICATION = "NOTIFICATION";
const char* msg::EC_START = "EC_START";
const char* msg::INIT_JOIN_TIMEOUT = "INIT_JOIN_TIMEOUT";
const char* msg::EVENT_TIMEOUT = "EVENT_TIMEOUT";
const char* msg::GC_TIMEOUT = "GC_TIMEOUT";

// messages between hosts and Rendezvous
const char* msg::HEARTBEAT = "HEARTBEAT";
const char* msg::HEARTBEAT_RESPONSE = "HEARTBEAT_RESPONSE";

// operations
const char* operation::UP = "\033[A";
const char* operation::LEFT = "\033[D";
const char* operation::DOWN = "\033[B";
const char* operation::RIGHT = "\033[C";
