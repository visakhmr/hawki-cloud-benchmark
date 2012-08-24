Steps:
Extract template.tar to get template folder
Rename the benchmark folder to desired benchmark name
Add benchmark source files as described in 'benchmark.c'
Edit Makefile to set the compiler and modify the source files
Edit SGE file to modify the cluster execution details



To submit the files during debugging in master node with SGE/OGE:
Run perlscript [Syntax]
./submit.pl <numprocs> [args]
