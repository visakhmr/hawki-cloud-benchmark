#!/usr/bin/python
import boto
import subprocess
import time
import sys
from optparse import OptionParser

#Set cluster parameters and cluster name
def parseOptions():
	parser = OptionParser()
	parser.add_option("-N", dest="clustername", type="string", default="hawkicluster",
	                  help="Name of cluster (string)")
	parser.add_option("-i", dest="imageid", type="string", default="ami-999d49f0",
                          help="Image id (string) [eg: ami-999d49f0]")
	parser.add_option("-t", dest="instancetype", type="string", default="m1.small",
                          help="Type of instance (string) [eg: m1.small]")
	parser.add_option("-s",dest="size", type="int", default="2",
	                  help="Cluster Size (int)")
	parser.add_option("-b", dest="benchmark",  default="benchmark", 
                  	  help="Path to benchmark code and scripts (folder)", metavar="FILE")
	parser.add_option("-u",dest="username", type="string", default="sgeadmin",
	                  help="Cluster login Username (string)")
	parser.add_option("-r",dest="results", default="results",
                  	  help="Path to benchmark results folder", metavar="FILE")
	(options, args) = parser.parse_args()
	return (options.clustername, options.imageid, options.instancetype, options.size, options.benchmark, options.username, options.results)

#Start cluster using starcluster tool, waiting for ready
def startCluster():
	print "Hawki>>> Starting cluster ..."+clustername
	try:
		process = subprocess.Popen("starcluster start "+ clustername, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        	output,stderr = process.communicate()
        	status = process.poll()
		output=output.splitlines()
		#get frontend nodename
		for line in output:
		 if 'The master node is' in line:
		   line=line.split(' ');
 		   hostname =  line[-1]
		   print "Front node :"+hostname
	except StandardError, err:
        	print err
		sys.exit()


#compile and execute scripts[with threshold limit] to run in front end are in instructions.sh
#make
#qsub command
#python report.py

#Transfer and extract  the benchmark program and parameters to the instance
#scp using pem file source files and scripts to front node and sge file to front end
def transferFiles():
	print "Hawki>>> Transferring files ..."
	try:
       		process = subprocess.Popen("starcluster put -u " + username +" " + clustername + " " + benchmark + "/ ./", shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		output,stderr = process.communicate()
        	status = process.poll()
		#get frontend nodename
		print output
	except StandardError, err: #clean up
        	print err
		sys.exit()
	
#ssh frontend pem and run scripts waiting for results
#execute program  starcluster sshmaster --user sgeadmin pythoncluster ./instructions.sh
def runBenchmark():
	print "Hawki>>> Running Benchmark ..."
	try:
		process = subprocess.Popen("starcluster sshmaster --user "+username + " " + clustername +
			" ./instructions.sh", shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		output,stderr = process.communicate()
		status = process.poll()
		print output
	except StandardError, err:
	        print err
	        sys.exit()


#Transfer the results to the instance
def getResults():
	print ">>> Gathering results ..."
	try:
	        process = subprocess.Popen("starcluster get -u "+username+" "+clustername+" ./*.sge.o* " + 
	                results+"/", shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		output,stderr = process.communicate()
	        status = process.poll()
		#get frontend nodename
		print output

	except StandardError, err:
	        print err
		sys.exit()


#stop instance using starcluster, waiting for completion
def terminateCluster():
	print ">>> Terminating cluster ..."
	try:
		process = subprocess.Popen("starcluster terminate -c "+clustername, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		output,stderr = process.communicate()
		status = process.poll()
		print output

	except StandardError, err:
	        print err
	        sys.exit()##ERROR msg


#Start

(clustername, imageid, instancetype, size, benchmark, username, results) = parseOptions()
startCluster()
transferFiles()
runBenchmark()
getResults()
terminateCluster()

#End
