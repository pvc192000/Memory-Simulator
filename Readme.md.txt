*****Memory Simulator*****

To run the program:

1. go to the directory which consists memSim.cpp and the makefile

2. run the makefile
	$ make

3. run the program with the following command:
	$ ./memsim <tracefile> <nframes> <lru | fifo | vms> <p (only if using vms)> <debug | quiet>
		
		here p is an integer from 1 to 100. 
		p repesents the percent of the frames to use for the secondary buffer.
	
	Examples:

	$ ./memsim bzip.trace 32 fifo debug
	$ ./memsim bzip.trace 32 lru quiet
	$ ./memsim sixpack.trace 32 vms 20 debug

**************************
