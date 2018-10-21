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

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

// Display channel strings
extern const char* DISPLAY_STR_CH_TEMP;
extern const char* DISPLAY_STR_CH_HIDDEN;
extern const char* DISPLAY_STR_CH_LINK;

// Display message strings
extern const char* DISPLAY_STR_MSG_GROUP;
extern const char* DISPLAY_STR_MSG_GROUP_CC;
extern const char* DISPLAY_STR_MSG_GROUP_GR;
extern const char* DISPLAY_STR_MSG_GROUP_EH;
extern const char* DISPLAY_STR_MSG_HIDDEN;

// Display module strings
extern const char* DISPLAY_STR_MOD_HIDDEN;

// Channel type strings
extern const char* CH_TYPE_FAIR_LOSS;

// message names
namespace msg {
// main group communication protocols
extern const char* COORD_CHANGE;
extern const char* CC_NACK;
extern const char* COORD_CHANGE_REPLY;
extern const char* LOAD_COORD;
extern const char* GROUP_RECONFIG;
extern const char* RECONFIG_READY;
extern const char* LOAD_CONFIG;
extern const char* PROPOSAL;
extern const char* DECISION;
extern const char* AGREEMENT;
extern const char* FINAL;
extern const char* CYCLE_EVENT;
extern const char* ROUND_QUERY;
extern const char* ROUND_QUERY_REPLY;
extern const char* CONSENSUS_QUERY;

// messages between client and hosts
extern const char* EVENT;
extern const char* UPDATE;
extern const char* INIT_JOIN;
extern const char* JOIN;

// initialization
extern const char* HOST_INIT;
extern const char* HEARTBEAT_INIT;
extern const char* RENDEZVOUS_INIT;
extern const char* CLIENT_INIT;
extern const char* PURGER_INIT;
extern const char* GROUP_INIT;

// timeout event
extern const char* HEARTBEAT_TIMEOUT;
extern const char* EH_TIMEOUT;
extern const char* EVENT_CYCLE;
extern const char* FAILURE;
extern const char* APPLY_CYCLE;
extern const char* NOTIFICATION;
// start of event collection
extern const char* EC_START;
extern const char* INIT_JOIN_TIMEOUT;
extern const char* EVENT_TIMEOUT;

// messages between hosts and Rendezvous
extern const char* HEARTBEAT;
extern const char* HEARTBEAT_RESPONSE;

}

#endif /* CONSTANTS_H_ */
