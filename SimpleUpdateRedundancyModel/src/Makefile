#
# OMNeT++/OMNEST Makefile for SimpleUpdateRedundancyModel
#
# This file was generated with the command:
#  opp_makemake -f --deep
#

# Name of target to be created (-o option)
TARGET = SimpleUpdateRedundancyModel$(EXE_SUFFIX)

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -I. \
    -IbasicSMR \
    -IbasicSMR/data \
    -IbasicSMR/messages \
    -Icommon \
    -IfastSMR \
    -IfastSMR/data \
    -IfastSMR/messages \
    -Iunreliable \
    -Iunreliable/messages

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS =

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = src
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc and .msg files
OBJS = \
    $O/basicSMR/BasicNuSMRNode.o \
    $O/basicSMR/BasicSMRClient.o \
    $O/basicSMR/BasicSMRNode.o \
    $O/basicSMR/BasicSMRRend.o \
    $O/basicSMR/data/BSMRProp.o \
    $O/basicSMR/data/LastState.o \
    $O/basicSMR/data/Ready.o \
    $O/common/Constants.o \
    $O/common/FairLossChannel.o \
    $O/common/GatebasedModule.o \
    $O/common/Purger.o \
    $O/common/sdbm.o \
    $O/common/Util.o \
    $O/fastSMR/FastSMRClient.o \
    $O/fastSMR/FastSMRNode.o \
    $O/fastSMR/FastSMRRend.o \
    $O/fastSMR/data/FSMRLastState.o \
    $O/fastSMR/data/FSMRReady.o \
    $O/unreliable/Client.o \
    $O/unreliable/Rendezvous.o \
    $O/unreliable/UnreliableHost.o \
    $O/basicSMR/messages/BSMRAgree_m.o \
    $O/basicSMR/messages/BSMRDecision_m.o \
    $O/basicSMR/messages/BSMRFinal_m.o \
    $O/basicSMR/messages/BSMRProposal_m.o \
    $O/basicSMR/messages/CCEnd_m.o \
    $O/basicSMR/messages/CCNAck_m.o \
    $O/basicSMR/messages/CCQuery_m.o \
    $O/basicSMR/messages/CCQueryReply_m.o \
    $O/basicSMR/messages/GREnd_m.o \
    $O/basicSMR/messages/GRQuery_m.o \
    $O/basicSMR/messages/GRQueryReply_m.o \
    $O/basicSMR/messages/HBProbe_m.o \
    $O/basicSMR/messages/HBResponse_m.o \
    $O/basicSMR/messages/HBTimeout_m.o \
    $O/fastSMR/messages/ConsensusQuery_m.o \
    $O/fastSMR/messages/CycleEvent_m.o \
    $O/fastSMR/messages/FSMRAgree_m.o \
    $O/fastSMR/messages/FSMRCCEnd_m.o \
    $O/fastSMR/messages/FSMRCCQueryReply_m.o \
    $O/fastSMR/messages/FSMRDecision_m.o \
    $O/fastSMR/messages/FSMRFinal_m.o \
    $O/fastSMR/messages/FSMRGREnd_m.o \
    $O/fastSMR/messages/FSMRProposal_m.o \
    $O/fastSMR/messages/InitJoin_m.o \
    $O/fastSMR/messages/Join_m.o \
    $O/fastSMR/messages/RoundQuery_m.o \
    $O/fastSMR/messages/RoundQueryReply_m.o \
    $O/unreliable/messages/CoordChange_m.o \
    $O/unreliable/messages/CoordChangeResp_m.o \
    $O/unreliable/messages/CoordResponse_m.o \
    $O/unreliable/messages/Event_m.o \
    $O/unreliable/messages/Heartbeat_m.o \
    $O/unreliable/messages/HeartbeatResponse_m.o \
    $O/unreliable/messages/Reconfig_m.o \
    $O/unreliable/messages/ReconfigResp_m.o \
    $O/unreliable/messages/Timeout_m.o \
    $O/unreliable/messages/Update_m.o

# Message files
MSGFILES = \
    basicSMR/messages/BSMRAgree.msg \
    basicSMR/messages/BSMRDecision.msg \
    basicSMR/messages/BSMRFinal.msg \
    basicSMR/messages/BSMRProposal.msg \
    basicSMR/messages/CCEnd.msg \
    basicSMR/messages/CCNAck.msg \
    basicSMR/messages/CCQuery.msg \
    basicSMR/messages/CCQueryReply.msg \
    basicSMR/messages/GREnd.msg \
    basicSMR/messages/GRQuery.msg \
    basicSMR/messages/GRQueryReply.msg \
    basicSMR/messages/HBProbe.msg \
    basicSMR/messages/HBResponse.msg \
    basicSMR/messages/HBTimeout.msg \
    fastSMR/messages/ConsensusQuery.msg \
    fastSMR/messages/CycleEvent.msg \
    fastSMR/messages/FSMRAgree.msg \
    fastSMR/messages/FSMRCCEnd.msg \
    fastSMR/messages/FSMRCCQueryReply.msg \
    fastSMR/messages/FSMRDecision.msg \
    fastSMR/messages/FSMRFinal.msg \
    fastSMR/messages/FSMRGREnd.msg \
    fastSMR/messages/FSMRProposal.msg \
    fastSMR/messages/InitJoin.msg \
    fastSMR/messages/Join.msg \
    fastSMR/messages/RoundQuery.msg \
    fastSMR/messages/RoundQueryReply.msg \
    unreliable/messages/CoordChange.msg \
    unreliable/messages/CoordChangeResp.msg \
    unreliable/messages/CoordResponse.msg \
    unreliable/messages/Event.msg \
    unreliable/messages/Heartbeat.msg \
    unreliable/messages/HeartbeatResponse.msg \
    unreliable/messages/Reconfig.msg \
    unreliable/messages/ReconfigResp.msg \
    unreliable/messages/Timeout.msg \
    unreliable/messages/Update.msg

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc or configuser.vc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" -loppmain$D $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ($(MAKECMDGOALS),depend)
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $O/$(TARGET)
	$(LN) $O/$(TARGET) .

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile
	@$(MKPATH) $O
	$(CXX) $(LDFLAGS) -o $O/$(TARGET)  $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS)

.PHONY: all clean cleanall depend msgheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE)
	@$(MKPATH) $(dir $@)
	$(CXX) -c $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(MSGC) -s _m.cc $(MSGCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

clean:
	-rm -rf $O
	-rm -f SimpleUpdateRedundancyModel SimpleUpdateRedundancyModel.exe libSimpleUpdateRedundancyModel.so libSimpleUpdateRedundancyModel.a libSimpleUpdateRedundancyModel.dll libSimpleUpdateRedundancyModel.dylib
	-rm -f ./*_m.cc ./*_m.h
	-rm -f basicSMR/*_m.cc basicSMR/*_m.h
	-rm -f basicSMR/data/*_m.cc basicSMR/data/*_m.h
	-rm -f basicSMR/messages/*_m.cc basicSMR/messages/*_m.h
	-rm -f common/*_m.cc common/*_m.h
	-rm -f fastSMR/*_m.cc fastSMR/*_m.h
	-rm -f fastSMR/data/*_m.cc fastSMR/data/*_m.h
	-rm -f fastSMR/messages/*_m.cc fastSMR/messages/*_m.h
	-rm -f unreliable/*_m.cc unreliable/*_m.h
	-rm -f unreliable/messages/*_m.cc unreliable/messages/*_m.h

cleanall: clean
	-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cc basicSMR/*.cc basicSMR/data/*.cc basicSMR/messages/*.cc common/*.cc fastSMR/*.cc fastSMR/data/*.cc fastSMR/messages/*.cc unreliable/*.cc unreliable/messages/*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/basicSMR/BasicNuSMRNode.o: basicSMR/BasicNuSMRNode.cc \
	basicSMR/BasicNuSMRNode.h \
	basicSMR/data/LastState.h \
	basicSMR/data/Ready.h \
	basicSMR/messages/BSMRAgree_m.h \
	basicSMR/messages/BSMRDecision_m.h \
	basicSMR/messages/BSMRFinal_m.h \
	basicSMR/messages/BSMRProposal_m.h \
	basicSMR/messages/CCEnd_m.h \
	basicSMR/messages/CCNAck_m.h \
	basicSMR/messages/CCQueryReply_m.h \
	basicSMR/messages/CCQuery_m.h \
	basicSMR/messages/GREnd_m.h \
	basicSMR/messages/GRQueryReply_m.h \
	basicSMR/messages/GRQuery_m.h \
	basicSMR/messages/HBProbe_m.h \
	basicSMR/messages/HBResponse_m.h \
	common/Constants.h \
	common/GatebasedModule.h \
	common/Purger.h \
	common/Util.h \
	common/sdbm.h \
	unreliable/messages/Event_m.h \
	unreliable/messages/Update_m.h
$O/basicSMR/BasicSMRClient.o: basicSMR/BasicSMRClient.cc \
	basicSMR/BasicSMRClient.h \
	common/Constants.h \
	common/GatebasedModule.h \
	common/Purger.h \
	common/Util.h \
	common/sdbm.h \
	unreliable/messages/Event_m.h \
	unreliable/messages/Timeout_m.h \
	unreliable/messages/Update_m.h
$O/basicSMR/BasicSMRNode.o: basicSMR/BasicSMRNode.cc \
	basicSMR/BasicSMRNode.h \
	basicSMR/data/LastState.h \
	basicSMR/data/Ready.h \
	basicSMR/messages/BSMRAgree_m.h \
	basicSMR/messages/BSMRDecision_m.h \
	basicSMR/messages/BSMRFinal_m.h \
	basicSMR/messages/BSMRProposal_m.h \
	basicSMR/messages/CCEnd_m.h \
	basicSMR/messages/CCNAck_m.h \
	basicSMR/messages/CCQueryReply_m.h \
	basicSMR/messages/CCQuery_m.h \
	basicSMR/messages/GREnd_m.h \
	basicSMR/messages/GRQueryReply_m.h \
	basicSMR/messages/GRQuery_m.h \
	basicSMR/messages/HBProbe_m.h \
	basicSMR/messages/HBResponse_m.h \
	common/Constants.h \
	common/GatebasedModule.h \
	common/Purger.h \
	common/Util.h \
	common/sdbm.h \
	unreliable/messages/Event_m.h \
	unreliable/messages/Update_m.h
$O/basicSMR/BasicSMRRend.o: basicSMR/BasicSMRRend.cc \
	basicSMR/BasicSMRRend.h \
	basicSMR/messages/HBProbe_m.h \
	basicSMR/messages/HBResponse_m.h \
	basicSMR/messages/HBTimeout_m.h \
	common/Constants.h \
	common/FairLossChannel.h \
	common/GatebasedModule.h \
	common/Purger.h \
	common/Util.h \
	common/sdbm.h \
	unreliable/messages/Event_m.h \
	unreliable/messages/Update_m.h
$O/basicSMR/data/BSMRProp.o: basicSMR/data/BSMRProp.cc \
	basicSMR/data/BSMRProp.h
$O/basicSMR/data/LastState.o: basicSMR/data/LastState.cc \
	basicSMR/data/LastState.h
$O/basicSMR/data/Ready.o: basicSMR/data/Ready.cc \
	basicSMR/data/Ready.h
$O/basicSMR/messages/BSMRAgree_m.o: basicSMR/messages/BSMRAgree_m.cc \
	basicSMR/messages/BSMRAgree_m.h \
	common/Constants.h
$O/basicSMR/messages/BSMRDecision_m.o: basicSMR/messages/BSMRDecision_m.cc \
	basicSMR/messages/BSMRDecision_m.h \
	common/Constants.h
$O/basicSMR/messages/BSMRFinal_m.o: basicSMR/messages/BSMRFinal_m.cc \
	basicSMR/messages/BSMRFinal_m.h \
	common/Constants.h
$O/basicSMR/messages/BSMRProposal_m.o: basicSMR/messages/BSMRProposal_m.cc \
	basicSMR/messages/BSMRProposal_m.h \
	common/Constants.h
$O/basicSMR/messages/CCEnd_m.o: basicSMR/messages/CCEnd_m.cc \
	basicSMR/messages/CCEnd_m.h \
	common/Constants.h
$O/basicSMR/messages/CCNAck_m.o: basicSMR/messages/CCNAck_m.cc \
	basicSMR/messages/CCNAck_m.h \
	common/Constants.h
$O/basicSMR/messages/CCQueryReply_m.o: basicSMR/messages/CCQueryReply_m.cc \
	basicSMR/messages/CCQueryReply_m.h \
	common/Constants.h
$O/basicSMR/messages/CCQuery_m.o: basicSMR/messages/CCQuery_m.cc \
	basicSMR/messages/CCQuery_m.h \
	common/Constants.h
$O/basicSMR/messages/GREnd_m.o: basicSMR/messages/GREnd_m.cc \
	basicSMR/messages/GREnd_m.h \
	common/Constants.h
$O/basicSMR/messages/GRQueryReply_m.o: basicSMR/messages/GRQueryReply_m.cc \
	basicSMR/messages/GRQueryReply_m.h \
	common/Constants.h
$O/basicSMR/messages/GRQuery_m.o: basicSMR/messages/GRQuery_m.cc \
	basicSMR/messages/GRQuery_m.h \
	common/Constants.h
$O/basicSMR/messages/HBProbe_m.o: basicSMR/messages/HBProbe_m.cc \
	basicSMR/messages/HBProbe_m.h \
	common/Constants.h
$O/basicSMR/messages/HBResponse_m.o: basicSMR/messages/HBResponse_m.cc \
	basicSMR/messages/HBResponse_m.h \
	common/Constants.h
$O/basicSMR/messages/HBTimeout_m.o: basicSMR/messages/HBTimeout_m.cc \
	basicSMR/messages/HBTimeout_m.h
$O/common/Constants.o: common/Constants.cc \
	common/Constants.h
$O/common/FairLossChannel.o: common/FairLossChannel.cc \
	common/Constants.h \
	common/FairLossChannel.h \
	common/Purger.h \
	common/Util.h \
	unreliable/messages/Event_m.h \
	unreliable/messages/Update_m.h
$O/common/GatebasedModule.o: common/GatebasedModule.cc \
	common/Constants.h \
	common/GatebasedModule.h \
	common/Purger.h \
	common/Util.h
$O/common/Purger.o: common/Purger.cc \
	common/Constants.h \
	common/Purger.h \
	common/Util.h
$O/common/Util.o: common/Util.cc \
	common/Util.h
$O/common/sdbm.o: common/sdbm.cc \
	common/sdbm.h
$O/fastSMR/FastSMRClient.o: fastSMR/FastSMRClient.cc \
	common/Constants.h \
	common/GatebasedModule.h \
	common/Purger.h \
	common/Util.h \
	common/sdbm.h \
	fastSMR/FastSMRClient.h \
	fastSMR/messages/InitJoin_m.h \
	fastSMR/messages/Join_m.h \
	unreliable/messages/Event_m.h \
	unreliable/messages/Timeout_m.h \
	unreliable/messages/Update_m.h
$O/fastSMR/FastSMRNode.o: fastSMR/FastSMRNode.cc \
	basicSMR/data/LastState.h \
	basicSMR/data/Ready.h \
	basicSMR/messages/CCNAck_m.h \
	basicSMR/messages/CCQuery_m.h \
	basicSMR/messages/GRQueryReply_m.h \
	basicSMR/messages/GRQuery_m.h \
	basicSMR/messages/HBProbe_m.h \
	basicSMR/messages/HBResponse_m.h \
	common/Constants.h \
	common/GatebasedModule.h \
	common/Purger.h \
	common/Util.h \
	common/sdbm.h \
	fastSMR/FastSMRNode.h \
	fastSMR/data/FSMRLastState.h \
	fastSMR/data/FSMRReady.h \
	fastSMR/messages/ConsensusQuery_m.h \
	fastSMR/messages/CycleEvent_m.h \
	fastSMR/messages/FSMRAgree_m.h \
	fastSMR/messages/FSMRCCEnd_m.h \
	fastSMR/messages/FSMRCCQueryReply_m.h \
	fastSMR/messages/FSMRDecision_m.h \
	fastSMR/messages/FSMRFinal_m.h \
	fastSMR/messages/FSMRGREnd_m.h \
	fastSMR/messages/FSMRProposal_m.h \
	fastSMR/messages/InitJoin_m.h \
	fastSMR/messages/Join_m.h \
	fastSMR/messages/RoundQueryReply_m.h \
	fastSMR/messages/RoundQuery_m.h \
	unreliable/messages/Event_m.h \
	unreliable/messages/Update_m.h
$O/fastSMR/FastSMRRend.o: fastSMR/FastSMRRend.cc \
	basicSMR/messages/HBProbe_m.h \
	basicSMR/messages/HBResponse_m.h \
	basicSMR/messages/HBTimeout_m.h \
	common/Constants.h \
	common/FairLossChannel.h \
	common/GatebasedModule.h \
	common/Purger.h \
	common/Util.h \
	common/sdbm.h \
	fastSMR/FastSMRRend.h \
	unreliable/messages/Event_m.h \
	unreliable/messages/Update_m.h
$O/fastSMR/data/FSMRLastState.o: fastSMR/data/FSMRLastState.cc \
	basicSMR/data/LastState.h \
	fastSMR/data/FSMRLastState.h
$O/fastSMR/data/FSMRReady.o: fastSMR/data/FSMRReady.cc \
	basicSMR/data/Ready.h \
	fastSMR/data/FSMRReady.h
$O/fastSMR/messages/ConsensusQuery_m.o: fastSMR/messages/ConsensusQuery_m.cc \
	common/Constants.h \
	fastSMR/messages/ConsensusQuery_m.h
$O/fastSMR/messages/CycleEvent_m.o: fastSMR/messages/CycleEvent_m.cc \
	common/Constants.h \
	fastSMR/messages/CycleEvent_m.h
$O/fastSMR/messages/FSMRAgree_m.o: fastSMR/messages/FSMRAgree_m.cc \
	common/Constants.h \
	fastSMR/messages/FSMRAgree_m.h
$O/fastSMR/messages/FSMRCCEnd_m.o: fastSMR/messages/FSMRCCEnd_m.cc \
	common/Constants.h \
	fastSMR/messages/FSMRCCEnd_m.h
$O/fastSMR/messages/FSMRCCQueryReply_m.o: fastSMR/messages/FSMRCCQueryReply_m.cc \
	common/Constants.h \
	fastSMR/messages/FSMRCCQueryReply_m.h
$O/fastSMR/messages/FSMRDecision_m.o: fastSMR/messages/FSMRDecision_m.cc \
	common/Constants.h \
	fastSMR/messages/FSMRDecision_m.h
$O/fastSMR/messages/FSMRFinal_m.o: fastSMR/messages/FSMRFinal_m.cc \
	common/Constants.h \
	fastSMR/messages/FSMRFinal_m.h
$O/fastSMR/messages/FSMRGREnd_m.o: fastSMR/messages/FSMRGREnd_m.cc \
	common/Constants.h \
	fastSMR/messages/FSMRGREnd_m.h
$O/fastSMR/messages/FSMRProposal_m.o: fastSMR/messages/FSMRProposal_m.cc \
	common/Constants.h \
	fastSMR/messages/FSMRProposal_m.h
$O/fastSMR/messages/InitJoin_m.o: fastSMR/messages/InitJoin_m.cc \
	fastSMR/messages/InitJoin_m.h
$O/fastSMR/messages/Join_m.o: fastSMR/messages/Join_m.cc \
	fastSMR/messages/Join_m.h
$O/fastSMR/messages/RoundQueryReply_m.o: fastSMR/messages/RoundQueryReply_m.cc \
	common/Constants.h \
	fastSMR/messages/RoundQueryReply_m.h
$O/fastSMR/messages/RoundQuery_m.o: fastSMR/messages/RoundQuery_m.cc \
	common/Constants.h \
	fastSMR/messages/RoundQuery_m.h
$O/unreliable/Client.o: unreliable/Client.cc \
	common/Constants.h \
	common/GatebasedModule.h \
	common/Purger.h \
	common/Util.h \
	common/sdbm.h \
	unreliable/Client.h \
	unreliable/messages/CoordResponse_m.h \
	unreliable/messages/Event_m.h \
	unreliable/messages/Timeout_m.h \
	unreliable/messages/Update_m.h
$O/unreliable/Rendezvous.o: unreliable/Rendezvous.cc \
	common/Constants.h \
	common/FairLossChannel.h \
	common/GatebasedModule.h \
	common/Purger.h \
	common/Util.h \
	common/sdbm.h \
	unreliable/Rendezvous.h \
	unreliable/messages/CoordResponse_m.h \
	unreliable/messages/Event_m.h \
	unreliable/messages/HeartbeatResponse_m.h \
	unreliable/messages/Heartbeat_m.h \
	unreliable/messages/Timeout_m.h \
	unreliable/messages/Update_m.h
$O/unreliable/UnreliableHost.o: unreliable/UnreliableHost.cc \
	common/Constants.h \
	common/GatebasedModule.h \
	common/Purger.h \
	common/Util.h \
	common/sdbm.h \
	unreliable/UnreliableHost.h \
	unreliable/messages/CoordChangeResp_m.h \
	unreliable/messages/CoordChange_m.h \
	unreliable/messages/Event_m.h \
	unreliable/messages/HeartbeatResponse_m.h \
	unreliable/messages/Heartbeat_m.h \
	unreliable/messages/ReconfigResp_m.h \
	unreliable/messages/Reconfig_m.h \
	unreliable/messages/Update_m.h
$O/unreliable/messages/CoordChangeResp_m.o: unreliable/messages/CoordChangeResp_m.cc \
	common/Constants.h \
	unreliable/messages/CoordChangeResp_m.h
$O/unreliable/messages/CoordChange_m.o: unreliable/messages/CoordChange_m.cc \
	common/Constants.h \
	unreliable/messages/CoordChange_m.h
$O/unreliable/messages/CoordResponse_m.o: unreliable/messages/CoordResponse_m.cc \
	unreliable/messages/CoordResponse_m.h
$O/unreliable/messages/Event_m.o: unreliable/messages/Event_m.cc \
	unreliable/messages/Event_m.h
$O/unreliable/messages/HeartbeatResponse_m.o: unreliable/messages/HeartbeatResponse_m.cc \
	common/Constants.h \
	unreliable/messages/HeartbeatResponse_m.h
$O/unreliable/messages/Heartbeat_m.o: unreliable/messages/Heartbeat_m.cc \
	common/Constants.h \
	unreliable/messages/Heartbeat_m.h
$O/unreliable/messages/ReconfigResp_m.o: unreliable/messages/ReconfigResp_m.cc \
	common/Constants.h \
	unreliable/messages/ReconfigResp_m.h
$O/unreliable/messages/Reconfig_m.o: unreliable/messages/Reconfig_m.cc \
	common/Constants.h \
	unreliable/messages/Reconfig_m.h
$O/unreliable/messages/Timeout_m.o: unreliable/messages/Timeout_m.cc \
	unreliable/messages/Timeout_m.h
$O/unreliable/messages/Update_m.o: unreliable/messages/Update_m.cc \
	common/Constants.h \
	unreliable/messages/Update_m.h

