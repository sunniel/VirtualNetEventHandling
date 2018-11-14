This folder contains the codes for evaluating the FastSMR event handling approach:

1. \ModelComparison: contains the comparison of the proposed model with the primary-backup model and the consensus-based model

2. \LateEventHandlingComparison: contains the test case for evaluating the proposed late event handling approach.

3. \GarbageCollectionTestCases: contains the test cases for evaluating the proposed garbage collection mechanism.

4. \TimeSynchronizationTestCases: contains the test cases for evaluation the effect of time synchronization in the proposed approach.

Currently, all simulations include a warm-up period and a coo_down period, meaning that the events generated within these periods will not be applied in the statistical result calculation. The warm-up period is configured through the following configuration:

warmup-period = 10s

The cool-down period is configured through the following configuration:

**.globalStatistics.cool_down = 10s

Thus, to make samples for 1000 seconds, for example, the simulaion time can be configured as follows:

sim-time-limit = 1020s

In most cases that need to take network latency more or less under control, apply the following configuration to change the network latency generation model to (Dmin + jitter) where Dmin denotes the minimal estimated latency in single-trip transmission, including the time of message transmission preparation.

**.globalParameters.latency_gen_mode = 1

Details of the test cases can be found in the README file in the respective folders. All test codes are created through OMNeT++ 5.4 with the same configuration as the working project (i.e., VirtualNetEventHandling).