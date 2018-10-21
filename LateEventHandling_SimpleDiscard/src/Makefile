#
# OMNeT++/OMNEST Makefile for LateEventHandling_SimpleDiscard
#
# This file was generated with the command:
#  opp_makemake -f --deep -IC:\Workspaces\boost_1_66_0
#

# Name of target to be created (-o option)
TARGET = LateEventHandling_SimpleDiscard$(EXE_SUFFIX)

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -IC:/Workspaces/boost_1_66_0 \
    -I. \
    -Icommon \
    -IfastSMR \
    -IfastSMR/data \
    -IfastSMR/messages

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
    $O/fastSMR/data/LastState.o \
    $O/fastSMR/data/Ready.o \
    $O/fastSMR/messages/CCNAck_m.o \
    $O/fastSMR/messages/CCQuery_m.o \
    $O/fastSMR/messages/ConsensusQuery_m.o \
    $O/fastSMR/messages/CoordResponse_m.o \
    $O/fastSMR/messages/CycleEvent_m.o \
    $O/fastSMR/messages/Event_m.o \
    $O/fastSMR/messages/FSMRAgree_m.o \
    $O/fastSMR/messages/FSMRCCEnd_m.o \
    $O/fastSMR/messages/FSMRCCQueryReply_m.o \
    $O/fastSMR/messages/FSMRDecision_m.o \
    $O/fastSMR/messages/FSMRFinal_m.o \
    $O/fastSMR/messages/FSMRGREnd_m.o \
    $O/fastSMR/messages/FSMRProposal_m.o \
    $O/fastSMR/messages/GREnd_m.o \
    $O/fastSMR/messages/GRQuery_m.o \
    $O/fastSMR/messages/GRQueryReply_m.o \
    $O/fastSMR/messages/HBProbe_m.o \
    $O/fastSMR/messages/HBResponse_m.o \
    $O/fastSMR/messages/HBTimeout_m.o \
    $O/fastSMR/messages/InitJoin_m.o \
    $O/fastSMR/messages/Join_m.o \
    $O/fastSMR/messages/RoundQuery_m.o \
    $O/fastSMR/messages/RoundQueryReply_m.o \
    $O/fastSMR/messages/Timeout_m.o \
    $O/fastSMR/messages/Update_m.o

# Message files
MSGFILES = \
    fastSMR/messages/CCNAck.msg \
    fastSMR/messages/CCQuery.msg \
    fastSMR/messages/ConsensusQuery.msg \
    fastSMR/messages/CoordResponse.msg \
    fastSMR/messages/CycleEvent.msg \
    fastSMR/messages/Event.msg \
    fastSMR/messages/FSMRAgree.msg \
    fastSMR/messages/FSMRCCEnd.msg \
    fastSMR/messages/FSMRCCQueryReply.msg \
    fastSMR/messages/FSMRDecision.msg \
    fastSMR/messages/FSMRFinal.msg \
    fastSMR/messages/FSMRGREnd.msg \
    fastSMR/messages/FSMRProposal.msg \
    fastSMR/messages/GREnd.msg \
    fastSMR/messages/GRQuery.msg \
    fastSMR/messages/GRQueryReply.msg \
    fastSMR/messages/HBProbe.msg \
    fastSMR/messages/HBResponse.msg \
    fastSMR/messages/HBTimeout.msg \
    fastSMR/messages/InitJoin.msg \
    fastSMR/messages/Join.msg \
    fastSMR/messages/RoundQuery.msg \
    fastSMR/messages/RoundQueryReply.msg \
    fastSMR/messages/Timeout.msg \
    fastSMR/messages/Update.msg

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
	-rm -f LateEventHandling_SimpleDiscard LateEventHandling_SimpleDiscard.exe libLateEventHandling_SimpleDiscard.so libLateEventHandling_SimpleDiscard.a libLateEventHandling_SimpleDiscard.dll libLateEventHandling_SimpleDiscard.dylib
	-rm -f ./*_m.cc ./*_m.h
	-rm -f common/*_m.cc common/*_m.h
	-rm -f fastSMR/*_m.cc fastSMR/*_m.h
	-rm -f fastSMR/data/*_m.cc fastSMR/data/*_m.h
	-rm -f fastSMR/messages/*_m.cc fastSMR/messages/*_m.h

cleanall: clean
	-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cc common/*.cc fastSMR/*.cc fastSMR/data/*.cc fastSMR/messages/*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/common/Constants.o: common/Constants.cc \
	common/Constants.h
$O/common/FairLossChannel.o: common/FairLossChannel.cc \
	common/Constants.h \
	common/FairLossChannel.h \
	common/Purger.h \
	common/Util.h \
	fastSMR/messages/Event_m.h \
	fastSMR/messages/Update_m.h
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
	fastSMR/messages/Event_m.h \
	fastSMR/messages/InitJoin_m.h \
	fastSMR/messages/Join_m.h \
	fastSMR/messages/Timeout_m.h \
	fastSMR/messages/Update_m.h
$O/fastSMR/FastSMRNode.o: fastSMR/FastSMRNode.cc \
	common/Constants.h \
	common/GatebasedModule.h \
	common/Purger.h \
	common/Util.h \
	common/sdbm.h \
	fastSMR/FastSMRNode.h \
	fastSMR/data/FSMRLastState.h \
	fastSMR/data/FSMRReady.h \
	fastSMR/data/LastState.h \
	fastSMR/data/Ready.h \
	fastSMR/messages/CCNAck_m.h \
	fastSMR/messages/CCQuery_m.h \
	fastSMR/messages/ConsensusQuery_m.h \
	fastSMR/messages/CycleEvent_m.h \
	fastSMR/messages/Event_m.h \
	fastSMR/messages/FSMRAgree_m.h \
	fastSMR/messages/FSMRCCEnd_m.h \
	fastSMR/messages/FSMRCCQueryReply_m.h \
	fastSMR/messages/FSMRDecision_m.h \
	fastSMR/messages/FSMRFinal_m.h \
	fastSMR/messages/FSMRGREnd_m.h \
	fastSMR/messages/FSMRProposal_m.h \
	fastSMR/messages/GRQueryReply_m.h \
	fastSMR/messages/GRQuery_m.h \
	fastSMR/messages/HBProbe_m.h \
	fastSMR/messages/HBResponse_m.h \
	fastSMR/messages/InitJoin_m.h \
	fastSMR/messages/Join_m.h \
	fastSMR/messages/RoundQueryReply_m.h \
	fastSMR/messages/RoundQuery_m.h \
	fastSMR/messages/Update_m.h
$O/fastSMR/FastSMRRend.o: fastSMR/FastSMRRend.cc \
	common/Constants.h \
	common/FairLossChannel.h \
	common/GatebasedModule.h \
	common/Purger.h \
	common/Util.h \
	common/sdbm.h \
	fastSMR/FastSMRRend.h \
	fastSMR/messages/Event_m.h \
	fastSMR/messages/HBProbe_m.h \
	fastSMR/messages/HBResponse_m.h \
	fastSMR/messages/HBTimeout_m.h \
	fastSMR/messages/Update_m.h
$O/fastSMR/data/FSMRLastState.o: fastSMR/data/FSMRLastState.cc \
	fastSMR/data/FSMRLastState.h \
	fastSMR/data/LastState.h
$O/fastSMR/data/FSMRReady.o: fastSMR/data/FSMRReady.cc \
	fastSMR/data/FSMRReady.h \
	fastSMR/data/Ready.h
$O/fastSMR/data/LastState.o: fastSMR/data/LastState.cc \
	fastSMR/data/LastState.h
$O/fastSMR/data/Ready.o: fastSMR/data/Ready.cc \
	fastSMR/data/Ready.h
$O/fastSMR/messages/CCNAck_m.o: fastSMR/messages/CCNAck_m.cc \
	common/Constants.h \
	fastSMR/messages/CCNAck_m.h
$O/fastSMR/messages/CCQuery_m.o: fastSMR/messages/CCQuery_m.cc \
	common/Constants.h \
	fastSMR/messages/CCQuery_m.h
$O/fastSMR/messages/ConsensusQuery_m.o: fastSMR/messages/ConsensusQuery_m.cc \
	common/Constants.h \
	fastSMR/messages/ConsensusQuery_m.h
$O/fastSMR/messages/CoordResponse_m.o: fastSMR/messages/CoordResponse_m.cc \
	fastSMR/messages/CoordResponse_m.h
$O/fastSMR/messages/CycleEvent_m.o: fastSMR/messages/CycleEvent_m.cc \
	common/Constants.h \
	fastSMR/messages/CycleEvent_m.h
$O/fastSMR/messages/Event_m.o: fastSMR/messages/Event_m.cc \
	fastSMR/messages/Event_m.h
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
$O/fastSMR/messages/GREnd_m.o: fastSMR/messages/GREnd_m.cc \
	common/Constants.h \
	fastSMR/messages/GREnd_m.h
$O/fastSMR/messages/GRQueryReply_m.o: fastSMR/messages/GRQueryReply_m.cc \
	common/Constants.h \
	fastSMR/messages/GRQueryReply_m.h
$O/fastSMR/messages/GRQuery_m.o: fastSMR/messages/GRQuery_m.cc \
	common/Constants.h \
	fastSMR/messages/GRQuery_m.h
$O/fastSMR/messages/HBProbe_m.o: fastSMR/messages/HBProbe_m.cc \
	common/Constants.h \
	fastSMR/messages/HBProbe_m.h
$O/fastSMR/messages/HBResponse_m.o: fastSMR/messages/HBResponse_m.cc \
	common/Constants.h \
	fastSMR/messages/HBResponse_m.h
$O/fastSMR/messages/HBTimeout_m.o: fastSMR/messages/HBTimeout_m.cc \
	fastSMR/messages/HBTimeout_m.h
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
$O/fastSMR/messages/Timeout_m.o: fastSMR/messages/Timeout_m.cc \
	fastSMR/messages/Timeout_m.h
$O/fastSMR/messages/Update_m.o: fastSMR/messages/Update_m.cc \
	common/Constants.h \
	fastSMR/messages/Update_m.h

