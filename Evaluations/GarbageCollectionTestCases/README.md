This project evaluates the effectiveness of the proposed garbage collection mechanism. 

Four test cases are configured as follows.

1) For observing the length of event delivery queue (i.e., Qd) with garbage collection, garbage collection is enabled through the following configurations.

	#**.node[*].ctrl.GCCycle = 5000ms

	**.node[*].ctrl.GC = ${GC = true}

	#**.churnGenerator.life_span_max = 1000s

2) For observing the length of event delivery queue (i.e., Qd) without garbage collection, garbage collection is disabled through the following configurations.

	#**.node[*].ctrl.GCCycle = 5000ms

	#**.node[*].ctrl.GC = ${GC = false}

	#**.churnGenerator.life_span_max = 1000s

3) For observing the change of event delivery queue (i.e., Qd) length with different garbage collection cycle lengths, change the system configuration to the following values.

	#**.node[*].ctrl.GCCycle = ${GCCycle = 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000}ms

	**.node[*].ctrl.GC = ${GC = true}

	#**.churnGenerator.life_span_max = 1000s

4) For observing the interaction latency length with different garbage collection cycle lengths, change the system configuration to the following values.

	**.node[*].ctrl.GCCycle = ${GCCycleForLatency = 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000}ms

	**.node[*].ctrl.GC = ${GC = true}

	#**.churnGenerator.life_span_max = 200s

The simulation results can be found in the /simulation/results folder: 

\GarbageCollectionTestCases\simulations\FastSMR-GCFlag.anf: contains the experiment result for observing the length of Qd with/without garbage collection.

\GarbageCollectionTestCases\simulations\QdLengthTrend.anf: contains the experiment result for observing the length of Qd with different garbage collection cycle lengths.

\GarbageCollectionTestCases\simulations\FastSMR-GC-Latency.anf: contains the experiment result for observing the interaction latency length with different garbage collection cycle lengths.