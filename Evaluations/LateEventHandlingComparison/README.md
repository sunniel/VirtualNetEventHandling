This project evaluates the effectiveness of the proposed late event handling mechanism and compares it with an alternative which simply discards late events. In the omnetpp.ini file, select the [Config SimpleDiscard] configuration to the run the simple event discard approach, while select the [Config DynamicDelivery] configuration to run the proposed late event handling mechanism. 

In the experiment, only update delivery rate is compared. The time of event sent from clients are varied with a random value generator to generate late events through the following configuration.

**.client[*].ctrl.offset = normal(0ms, ${0,25,50,75,100,200,300,400,500}ms)

The simulation results can be found in the \simulation\results folder:

\LateEventHandlingComparison\simulations\SimpleDiscard.anf: contains the experiment result for the simple late event discard approach.

\LateEventHandlingComparison\simulations\DynamicDelivery.anf: contains the experiment result for the proposed late event discard approach.