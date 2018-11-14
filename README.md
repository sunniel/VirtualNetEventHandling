This repository contains the demo codes for the paper "Virtual Net: a Decentralized Architecture for Interaction in Mobile Virtual Worlds".

The project simulates the event handling process in a decentralized virtual world architecture. The rough idea of the architecture is that each user is assigned a group of replicated nodes (or replicas), called a mesh computer. Through client connection, each mesh computer maintains the latest state of a user in a virtual environment, such as avatar location and other states. The interaction of a user and the virtual environment through a client will be synchronized to the replicas of the mesh computer for reliable virtual world application execution. The main problem is how to maintain the consistent state on all the replicas of a mesh computer, and meanwhile reducing the interaction latency. The work in the paper proposed a fast event handling approach (called FastSMR) based on state machine replication (SMR) and time synchronization to opportunistically reduce the communication overhead in total-order event delivery. The main contribution is the late event handling mechanism in the case that event arrival is accidentally later than the pre-determined time slot. Through the proposed mechanism, event lost rate can be minimized, while system responsiveness and the state consistency can still be maintained.

In a mesh computer, each node is treated as an agent which can maintains its state through event handling, leader election, and group reconfiguration (i.e., new replica creation). Through SMR, All the replicas of a mesh computer forms a new monolith agent, called a replicated agent (or re-agent). Based on the proposed FastSMR, the interaction between virtual world users now becomes the interaction between multiple re-agents, which is under our development.

Update 2018-11-14:

1. The codes (including the test cases) have been migrated from OMNeT++ 4.2.2 to OMNeT++ 5.4. Now the main version is in the VirtualNetEventHandling folder.

2. The old version has been moved to the OMNeT++ 4.2.2 folder.

3. The test cases of the latest version can be found in the Evaluations folder.

Details of the work can be found the in respective project folders and sub-folders.

Contacts: Daniel Shen (daniel.shen@connect.umac.mo), Dr. Jingzhi Guo (jzguo@umac.mo)