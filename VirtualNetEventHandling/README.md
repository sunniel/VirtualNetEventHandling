The simulator is developed on OMNeT++ 5.4. Other third-party dependent libraries includes: 

1. boost C++ libraries (1.66.0, https:\\www.boost.org\)

2. JSON for Modern C++ (https:\\github.com\nlohmann\json)

The above libraries, as well as the path to "<omnetpp-5.4_home>\include\omnetpp", needs to be added to the project Include directory paths.

The Makefile is auto generated, thus, no need to change it.

Folder introduction:

1. \simulations: contains the simulation configuration file: omnetpp.int, result analysis file(s) with .anf suffix, and the simulation results in the "results" folder.

2. \output: c++ executable file for debug and release.

3. \src: simulation source codes, includes the c++ source codes (.h and .cc files), message definition files (.msg), and network definition files (.ned).

	1) \abstract: the definition of the entire simulation network and a logical computer (i.e., a mesh computer).
	
	2) \common: includes the constants, base modules (HostBase), and utility functions.
	
	3) \global: includes the global supporting modules for dynamic module generation\destruction, address-to-host mapping management, network churn generation, global simulation parameters, simulation results generation, etc. To simplify the simulation model, they are invoked through direct API call rather than message exchange among other modules.
	
	4) \hosts: includes the definition of main modules: client, replica node (node), and rendezvous node(rendezvous). The rendezvous node is introduced to facilitating the management of node life cycle and the the generation of new replicas. In a logical computer, node life state is monitored by the rendezvous through heartbeat message exchange.
	
	5) \ctrl: includes the control components of modules, each of which executes the main logics of a module.
	
	6) \objects: includes the common c++ objects for inter-module communication. Most of them are copied from iNet (https:\\inet.omnetpp.org\) and OverSim (http:\\www.oversim.org\) after simplification. 
	
	7) \transport: includes the network transport components used in all modules to simulate unreliable and reliable communication.
	
	8) \app: the application-level components. Currently, it only simulates the simplest event handling logics to generate application states.
	
	9) \others: other components used by some modules.
	
	10) \message: includes the message definitions used for module communication. Most of the files in this folder are auto-generated in project build.
	
omnetpp.ini: 

	1) the simulation-level configurations are defined in the form of **.<parameter> = <value>
	
	2) the module-level configurations are defined in the form of **.<module>.<parameter> = <value>
	
	3) the component-level configurations are defined in the form of **.<module>.<component>.<parameter> = <value>
	
	4) the [General] section defines the simulation execution related parameters.