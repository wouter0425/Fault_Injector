# Fault Injector
This fault injector is used to inject faults in an executable and possibly its children.
The injector works by halting a process, obtaining the process register values, changing these values, putting the registers back into the process and continue its operation. Additionally, the fault injector logs the target core and the child processes that run on these cores (if defined by the user)

# Fault injection parameters
The fault injector simulates an intermittent fault model. In this fault model, a core/or cores periodically suffer from a burst of SEU.
This could be caused by temperature, EMC or other environmental factors.
- target_location: location of the target executable
- startup delay: number of ms the injector waits after starting the target
- burst_time: number of ms the injector attacks the target
- burst_frequency: a delay in microseconds between injections

# How to run
- make clean            // Removes the bin and obj folder
- make reset            // Removes all the log files
- make                  // creates the executable
- sudo bin/injector     // Runs the fault injector
- ctrl + c              // terminates the fault injector and writes the results

# Visualize the results
In order to visualize the results, a python script is added to plot a graph based on the .txt output files. It can be used as follows:
- python3 plot_data.py <directory>
This will plot the most recently added output file in the specified directory. Additionally, two flags are supported:
- --file        // To add a specific file, the should be the full file path, for example: python3 plot_data.py NMR --file NMR/      targets_2024-07-18_02-25-06.txt
- --aggregation // Aggregates the data points, in cases where there are a lot of datapoints, aggregation will aggregate them by a user defined value, for example: python3 plot_data.py NMR --file NMR/targets_2024-07-18_02-25-06.txt --aggregation 5000
