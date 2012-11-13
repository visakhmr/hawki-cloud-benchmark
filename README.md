hawki-cloud-benchmark
=====================

Benchmark Scientific applications in HPC Cloud

This piece of work creates a portal to configure and submit jobs in an Open Grid Scheduler (OGS/GE), previously known as Sun grid Engine.  

The project has 3 parts separated as 3 folders:
1) hawki: Simple joomla installations with 3 modules(portlets) to submit configuration and inputs to execute benchmarks.
2) environment: A manager script in python to accept the input parameters (from portlets or from console) and run benchmarks after creating a cluster with the specified configuration.
3) benchmarks: A library of benchmarks/programs to be executed in a cluster. You can upload your own programs though the portal.

URL: http://ec2-23-20-179-14.compute-1.amazonaws.com/hawki
Contact us for login details.

Simple flow of events in a typical execution will be:
1) Open the portal
2) Download benchmark template
3) Add your piece of code in the template following the instructions in template.tar
4) Archive and upload template and specify any command line inputs
5) Execute benchmark and wait for a detailed result in browser
6) View summary of all executions in results tab

Demo video : http://www.youtube.com/watch?v=BPZTeoY2RUM
