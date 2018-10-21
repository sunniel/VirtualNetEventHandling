# VirtualNetEventHandling

This repository contains the demo codes for the paper "Virtual Net: a Decentralized Architecture for Interaction in Mobile Virtual Worlds".

It has 8 sub-projects, including:
1. SimpleUpdateRedundancyModel
2. MultiNodeFastSMRSimulation
3. LateEventHandling_SimpleDiscard
4. LateEventHandling_DynamicCycleEventHandling
5. VNetGarbageCollection
6. VNetWithoutGarbageCollection
7. VNetTimeSynchronization
8. ContentIntegrity

Project 1-7 are OMNet++ 4.2.2 projects, with Boost 1.66 as the external additional library. Project 8 is a JAVA project. The complete implementation of the proposed fast event delivery algorithm is in the MultiNodeFastSMRSimulation project.

The SimpleUpdateRedundancyModel is used to evaluate the overall performance of the proposed fast event handling algorithm. The compared models are implemented in the SimpleUpdateRedundancyModel project. In this project, configuring omnetpp.ini to different instances in run to select different models. Specifically, the SimpleURM represents the primary-backup model, the BasicSMR represents the consensus-based model, and the FastSMR represents the proposed fast event delivery model. These settings are used to test the interaction latency of different models with different channel delay. The SimpleURMDeliveryRate, BasicSMRDeliveryRate, and FastSMRDeliveryRate settings are used to test the delivery rate of different models with different message drop rates.

The LateEventHandling_SimpleDiscard project and the LateEventHandling_DynamicCycleEventHandling project are used to evaluated the proposed late event handling approach. In the LateEventHandling_SimpleDiscard project, late events are simply discard, while in the LateEventHandling_DynamicCycleEventHandling project, late events are handled by the proposed late event handling approach. The execution results of these two projects are compared to evaluate the effectiveness and the efficiency improvement of the proposed late event handling approach.

The VNetWithoutGarbageCollection project and the VNetGarbageCollection project are used to evaluate the proposed garbage collect (GC) mechanism. The VNetWithoutGarbageCollection project simply discards the GC mechanism, while the VNetGarbageCollection project applies the project GC mechanism. The execution results of these two projects are compared to evaluate the effectiveness of the proposed GC mechanism.

The VNetTimeSynchronization project is used to evaluate the proposed time synchronization mechanism. In the VNetTimeSynchronization project, the "**.client[*].offset" property is configured to different values to simulate the clock offset between event senders and event recipients. If the value is configured to 0, it represents the time synchronization mode.

The ContentIntegrity project is used to evaluate the efficiency of the project content integrity check scheme.