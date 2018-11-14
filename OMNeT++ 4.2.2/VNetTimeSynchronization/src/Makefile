#
# OMNeT++/OMNEST Makefile for VNetTimeSynchronization
#
# This file was generated with the command:
#  opp_makemake -f --deep -IC:\Workspaces\boost_1_66_0
#

# Name of target to be created (-o option)
TARGET = VNetTimeSynchronization$(EXE_SUFFIX)

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -IC:/Workspaces/boost_1_66_0 \
    -I. \
    -Icommon \
    -Idata \
    -Imessages \
    -Ined \
    -Itelnet

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
    $O/Client.o \
    $O/NodeApp.o \
    $O/NodeCtrl.o \
    $O/Rendezvous.o \
    $O/common/Constants.o \
    $O/common/FairLossChannel.o \
    $O/common/GatebasedModule.o \
    $O/common/Purger.o \
    $O/common/sdbm.o \
    $O/common/Util.o \
    $O/data/LastState.o \
    $O/data/Ready.o \
    $O/telnet/SocketRTScheduler.o \
    $O/messages/Agree_m.o \
    $O/messages/CCEnd_m.o \
    $O/messages/CCNAck_m.o \
    $O/messages/CCQuery_m.o \
    $O/messages/CCQueryReply_m.o \
    $O/messages/ConsensusQuery_m.o \
    $O/messages/CycleEvent_m.o \
    $O/messages/Decision_m.o \
    $O/messages/Event_m.o \
    $O/messages/EventApply_m.o \
    $O/messages/Final_m.o \
    $O/messages/Gossip_m.o \
    $O/messages/GREnd_m.o \
    $O/messages/GRQuery_m.o \
    $O/messages/GRQueryReply_m.o \
    $O/messages/HBProbe_m.o \
    $O/messages/HBResponse_m.o \
    $O/messages/HBTimeout_m.o \
    $O/messages/InitJoin_m.o \
    $O/messages/Join_m.o \
    $O/messages/Proposal_m.o \
    $O/messages/RoundQuery_m.o \
    $O/messages/RoundQueryReply_m.o \
    $O/messages/Timeout_m.o \
    $O/messages/Update_m.o \
    $O/messages/UpdateApply_m.o

# Message files
MSGFILES = \
    messages/Agree.msg \
    messages/CCEnd.msg \
    messages/CCNAck.msg \
    messages/CCQuery.msg \
    messages/CCQueryReply.msg \
    messages/ConsensusQuery.msg \
    messages/CycleEvent.msg \
    messages/Decision.msg \
    messages/Event.msg \
    messages/EventApply.msg \
    messages/Final.msg \
    messages/Gossip.msg \
    messages/GREnd.msg \
    messages/GRQuery.msg \
    messages/GRQueryReply.msg \
    messages/HBProbe.msg \
    messages/HBResponse.msg \
    messages/HBTimeout.msg \
    messages/InitJoin.msg \
    messages/Join.msg \
    messages/Proposal.msg \
    messages/RoundQuery.msg \
    messages/RoundQueryReply.msg \
    messages/Timeout.msg \
    messages/Update.msg \
    messages/UpdateApply.msg

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
	-rm -f VNetTimeSynchronization VNetTimeSynchronization.exe libVNetTimeSynchronization.so libVNetTimeSynchronization.a libVNetTimeSynchronization.dll libVNetTimeSynchronization.dylib
	-rm -f ./*_m.cc ./*_m.h
	-rm -f common/*_m.cc common/*_m.h
	-rm -f data/*_m.cc data/*_m.h
	-rm -f messages/*_m.cc messages/*_m.h
	-rm -f ned/*_m.cc ned/*_m.h
	-rm -f telnet/*_m.cc telnet/*_m.h

cleanall: clean
	-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cc common/*.cc data/*.cc messages/*.cc ned/*.cc telnet/*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/Client.o: Client.cc \
	Client.h \
	common/Constants.h \
	common/GatebasedModule.h \
	common/Purger.h \
	common/Util.h \
	common/sdbm.h \
	messages/Event_m.h \
	messages/InitJoin_m.h \
	messages/Join_m.h \
	messages/Timeout_m.h \
	messages/Update_m.h \
	telnet/SocketRTScheduler.h
$O/NodeApp.o: NodeApp.cc \
	NodeApp.h \
	common/Constants.h \
	common/GatebasedModule.h \
	common/Purger.h \
	common/Util.h \
	common/sdbm.h \
	data/Ready.h \
	messages/EventApply_m.h \
	messages/UpdateApply_m.h \
	telnet/SocketRTScheduler.h
$O/NodeCtrl.o: NodeCtrl.cc \
	NodeCtrl.h \
	common/Constants.h \
	common/GatebasedModule.h \
	common/Purger.h \
	common/Util.h \
	common/sdbm.h \
	data/LastState.h \
	data/Ready.h \
	messages/Agree_m.h \
	messages/CCEnd_m.h \
	messages/CCNAck_m.h \
	messages/CCQueryReply_m.h \
	messages/CCQuery_m.h \
	messages/ConsensusQuery_m.h \
	messages/CycleEvent_m.h \
	messages/Decision_m.h \
	messages/EventApply_m.h \
	messages/Event_m.h \
	messages/Final_m.h \
	messages/GREnd_m.h \
	messages/GRQueryReply_m.h \
	messages/GRQuery_m.h \
	messages/Gossip_m.h \
	messages/HBProbe_m.h \
	messages/HBResponse_m.h \
	messages/InitJoin_m.h \
	messages/Join_m.h \
	messages/Proposal_m.h \
	messages/RoundQueryReply_m.h \
	messages/RoundQuery_m.h \
	messages/UpdateApply_m.h \
	messages/Update_m.h
$O/Rendezvous.o: Rendezvous.cc \
	Rendezvous.h \
	common/Constants.h \
	common/FairLossChannel.h \
	common/GatebasedModule.h \
	common/Purger.h \
	common/Util.h \
	common/sdbm.h \
	messages/Event_m.h \
	messages/HBProbe_m.h \
	messages/HBResponse_m.h \
	messages/HBTimeout_m.h \
	messages/Update_m.h
$O/common/Constants.o: common/Constants.cc \
	common/Constants.h
$O/common/FairLossChannel.o: common/FairLossChannel.cc \
	common/Constants.h \
	common/FairLossChannel.h \
	common/Purger.h \
	common/Util.h \
	messages/Event_m.h \
	messages/Update_m.h
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
$O/data/LastState.o: data/LastState.cc \
	data/LastState.h
$O/data/Ready.o: data/Ready.cc \
	data/Ready.h
$O/messages/Agree_m.o: messages/Agree_m.cc \
	common/Constants.h \
	messages/Agree_m.h
$O/messages/CCEnd_m.o: messages/CCEnd_m.cc \
	common/Constants.h \
	messages/CCEnd_m.h
$O/messages/CCNAck_m.o: messages/CCNAck_m.cc \
	common/Constants.h \
	messages/CCNAck_m.h
$O/messages/CCQueryReply_m.o: messages/CCQueryReply_m.cc \
	common/Constants.h \
	messages/CCQueryReply_m.h
$O/messages/CCQuery_m.o: messages/CCQuery_m.cc \
	common/Constants.h \
	messages/CCQuery_m.h
$O/messages/ConsensusQuery_m.o: messages/ConsensusQuery_m.cc \
	common/Constants.h \
	messages/ConsensusQuery_m.h
$O/messages/CycleEvent_m.o: messages/CycleEvent_m.cc \
	common/Constants.h \
	messages/CycleEvent_m.h
$O/messages/Decision_m.o: messages/Decision_m.cc \
	common/Constants.h \
	messages/Decision_m.h
$O/messages/EventApply_m.o: messages/EventApply_m.cc \
	messages/EventApply_m.h
$O/messages/Event_m.o: messages/Event_m.cc \
	messages/Event_m.h
$O/messages/Final_m.o: messages/Final_m.cc \
	common/Constants.h \
	messages/Final_m.h
$O/messages/GREnd_m.o: messages/GREnd_m.cc \
	common/Constants.h \
	messages/GREnd_m.h
$O/messages/GRQueryReply_m.o: messages/GRQueryReply_m.cc \
	common/Constants.h \
	messages/GRQueryReply_m.h
$O/messages/GRQuery_m.o: messages/GRQuery_m.cc \
	common/Constants.h \
	messages/GRQuery_m.h
$O/messages/Gossip_m.o: messages/Gossip_m.cc \
	common/Constants.h \
	messages/Gossip_m.h
$O/messages/HBProbe_m.o: messages/HBProbe_m.cc \
	common/Constants.h \
	messages/HBProbe_m.h
$O/messages/HBResponse_m.o: messages/HBResponse_m.cc \
	common/Constants.h \
	messages/HBResponse_m.h
$O/messages/HBTimeout_m.o: messages/HBTimeout_m.cc \
	messages/HBTimeout_m.h
$O/messages/InitJoin_m.o: messages/InitJoin_m.cc \
	messages/InitJoin_m.h
$O/messages/Join_m.o: messages/Join_m.cc \
	messages/Join_m.h
$O/messages/Proposal_m.o: messages/Proposal_m.cc \
	common/Constants.h \
	messages/Proposal_m.h
$O/messages/RoundQueryReply_m.o: messages/RoundQueryReply_m.cc \
	common/Constants.h \
	messages/RoundQueryReply_m.h
$O/messages/RoundQuery_m.o: messages/RoundQuery_m.cc \
	common/Constants.h \
	messages/RoundQuery_m.h
$O/messages/Timeout_m.o: messages/Timeout_m.cc \
	messages/Timeout_m.h
$O/messages/UpdateApply_m.o: messages/UpdateApply_m.cc \
	messages/UpdateApply_m.h
$O/messages/Update_m.o: messages/Update_m.cc \
	common/Constants.h \
	messages/Update_m.h
$O/telnet/SocketRTScheduler.o: telnet/SocketRTScheduler.cc \
	telnet/SocketRTScheduler.h

