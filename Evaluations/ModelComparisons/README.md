Three models are compared: 1) primary-backup, 2) pure consensus-based, and 3) FastSMR

They are compared in 1) interaction latency (i.e., end-to-end latency) and 2) update delivery rate.

The AlternativeEHModels project contains the primary-backup model and the consensus-based model. In its omnetpp.ini file, select the [Config PrimaryBackup] configuration to the run the primary-backup model, while select the [Config ConsensusBased] configuration to run the consensus-based model. 

The FastSMRModel project contains the FastSMR model only.

In the omnetpp.ini file of both projects, configure jitter and packet loss rate as follows for the evaluation in interaction latency, so that only the network jitter is varied.

**.pktLossRate = 0.01

#**.jitter = truncnormal(0ms, ${jitter = 50, 100, 150, 200, 250}ms)

In the omnetpp.ini file of both projects, configure jitter and packet loss rate as follows for the evaluation in update delivery rate, so that only the packet loss rate is varied.

#**.pktLossRate = ${DropRate = 0.3, 0.4, 0.5, 0.6, 0.7}

**.jitter = 50ms

The simulation results can be found in the \simulation\results folder:

	\AlternativeEHModels\simulations\PrimaryBackup-Latency.anf: contains the experiment result of interaction latency evaluation for the primary-backup model.

	\AlternativeEHModels\simulations\ConsensusBased-Latency.anf: contains the experiment result of interaction latency evaluation for the consensus-based model.

	\FastSMRModel\simulations\FastSMR-Latency.anf: file contains experiment result of interaction latency evaluation for the FastSMR model.

	\AlternativeEHModels\simulations\PrimaryBackup-PacketDrop.anf: contains the experiment result of update delivery rate evaluation for the primary-based model.

	\AlternativeEHModels\simulations\ConsensusBased-PacketDrop.anf: contains the experiment result of update delivery rate evaluation for the consensus-based model.

	\FastSMRModel\simulations\FastSMR-PacketDrop.anf: contains the experiment result of update delivery rate evaluation for the FastSMR model.