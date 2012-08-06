#!/usr/bin/python
import boto
import subprocess
import time
import sys
import os
import MySQLdb
import shutil
from optparse import OptionParser

#Validate the input parameters
def validate():
	print "\nHawki>>> Validating the inputs"
	if not os.path.exists(results):
    		os.makedirs(results)
		print "Default results folder created"

	if not os.path.exists(benchmark+"/"+runscript):
		print "Benchmark script "+runscript+" doesn't exist, provide path -b <benchmark folder path>"
		sys.exit()

	clusterparams = " -n " + str(imageid) + " -s " + str(size) + " -u " + str(username) + " -i " + str(instancetype)
	return clusterparams

#Set cluster parameters and cluster name
def parseOptions():
	tempname=str(time.time());
	parser = OptionParser()
	parser.add_option("-N", dest="clustername", type="string", default=tempname,
	                  help="Name of cluster (string)")
	parser.add_option("-i", dest="imageid", type="string", default="ami-999d49f0",
                          help="Image id (string) [eg: ami-999d49f0]")
	parser.add_option("-t", dest="instancetype", type="string", default="m1.small",
                          help="Type of instance (string) [eg: m1.small]")
	parser.add_option("-s",dest="size", type="int", default="1",
	                  help="Cluster Size (int)")
	parser.add_option("-b", dest="benchmark",  default="../benchmark", 
                  	  help="Path to benchmark code and scripts (folder)", metavar="FILE")
	parser.add_option("-u",dest="userid", type="int", default=0,
	                  help="Web userid (for web interface)")
	parser.add_option("-r",dest="results", default=tempname+"_results",
                  	  help="Path to benchmark results folder", metavar="FILE")
	parser.add_option("-a",dest="archive", type="int", default=0,
                          help="Archive results to database (0/1)")
	parser.add_option("-d",dest="datainput", type="string", default="",
                          help="Input parameters (to change input files/parameters)")
	(options, args) = parser.parse_args()
	return (options.clustername, options.imageid, options.instancetype, options.size, options.benchmark, options.userid, options.results, options.archive, options.datainput)

#Start cluster using starcluster tool, waiting for ready
def startCluster():
	print "\nHawki>>> Starting cluster ..."+clustername
	try:
		process = subprocess.Popen("starcluster start "+ clusterparams + " " + clustername, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        	output,stderr = process.communicate()
        	status = process.poll()
		output=output.splitlines()
		#get frontend nodename
		for line in output:
		 if 'The master node is' in line:
		   line=line.split(' ')
 		   hostname =  line[-1]
		   #print "Front node :"+hostname
	except StandardError, err:
        	print err
		terminateCluster()
		sys.exit()

#compile and execute scripts[with threshold limit] to run in front end are in instructions.sh
#make
#qsub command
#python report.py

#Transfer and extract  the benchmark program and parameters to the instance
#scp using pem file source files and scripts to front node and sge file to front end
def transferFiles():
	print "\nHawki>>> Transferring files ..."
	try:
       		process = subprocess.Popen("starcluster put -u " + username +" " + clustername + " " + benchmark + "/ ./", shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		output,stderr = process.communicate()
        	status = process.poll()
		#get frontend nodename
		#print output
	except StandardError, err: #clean up
        	print err
		terminateCluster()
		sys.exit()
	
#ssh frontend pem and run scripts waiting for results
#execute program  starcluster sshmaster --user sgeadmin pythoncluster ./instructions.sh
def runBenchmark():
	print "\nHawki>>> Running Benchmark ..."
	try:
		process = subprocess.Popen("starcluster sshmaster --user "+username + " " + clustername +
			" ./" + runscript + " "+str(size)+" "+inputdata, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		output,stderr = process.communicate()
		status = process.poll()
		#print output
	except StandardError, err:
	        print err
		terminateCluster()
	        sys.exit()


#Transfer the results to the instance
def getResults():
	print "\nHawki>>> Gathering results ..."
	try:
	        process = subprocess.Popen("starcluster get -u "+username+" "+clustername+" ./*.sge.o* " + 
	                results+"/", shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		output,stderr = process.communicate()
	        status = process.poll()
		#get frontend nodename
		#print output

	except StandardError, err:
	        print err
		terminateCluster()
		sys.exit()

#Store results in database
def saveResults():
	print "\nHawki>>> Saving results ..."
	try:
		#Save results in variables from results folder
		output = {}	#empty array to store results
		outputfile = open(results+"/benchmark.sge.o1",'r')	#read file
		x = outputfile.readline()
		status=1
		timetocompletion = 0.0
		while (x):
			var = x.split(' ')
			if (var[0]=='real'):
				timetocompletion = float(var[-1])
				print "var1:"+var[0]+" var2:"+var[-1]
			if(var[0]=='status'):
				status=1	
				timetocompletion=0.0
			x=outputfile.readline()
	
		#delete directories
		#os.removedirs(results)
		shutil.rmtree(results)
	
		benchmarkid=0
		instanceid=0
		inputid=0
		
		#hard coded for prototype
		db = MySQLdb.connect("localhost","root","pass123","ubuntu_hawki" )
	        cursor = db.cursor()
		benchmarks = benchmark.split('/')
	
	        sql = "SELECT benchmarkid from cx_benchmarks where sourcepath like '%s'" % (benchmarks[-1])
		#print sql
	        try:
	                cursor.execute(sql)
	        	benchmarkid = cursor.fetchone()#benchmarkid
			benchmarkid=benchmarkid[0]
			print benchmarkid
	                db.commit()
	        except:
	                db.rollback()
			benchmarkid=0
	
	        sql = "SELECT instanceid from cx_instances where instancename like '%s'" % (instancetype)
		#print sql
	        try:
	                cursor.execute(sql)
	        	instanceid = cursor.fetchone()#instanceid
			instanceid=instanceid[0]
	                db.commit()
	        except:
	                db.rollback()
			instanceid=0
	
	        sql = "SELECT inputid from cx_input where inputstring like '%s' and benchmarkid = %d" % (inputdata, benchmarkid)
		#print sql
        	try:
       	        	cursor.execute(sql)
       		 	inputid = cursor.fetchone()#inputid
			inputid=inputid[0]
                	db.commit()
	        except:	
	                db.rollback()
			inputid=0
	
		#Insert into results table
		sql = "INSERT INTO cx_results (`benchmarkid` , `instanceid` , `inputid` , `userid` , `size`, `timetocompletion`, `status`)\
			 VALUES (%d, %d, %d, %d, %d, %f, %d)" % (benchmarkid, instanceid, inputid, userid, size, timetocompletion, status)
		print sql
		try:
	   		cursor.execute(sql)
		   	db.commit()
		except:
			db.rollback()
			db.close()
	except StandardError, err:
                print err
                terminateCluster()
                sys.exit()

#stop instance using starcluster, waiting for completion
def terminateCluster():
	print "\nHawki>>> Terminating cluster ..."
	try:
		pass
		process = subprocess.Popen("starcluster terminate -c "+clustername, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		output,stderr = process.communicate()
		status = process.poll()
		#print output

	except StandardError, err:
	        print err
	        sys.exit()##ERROR msg


#Start

runscript="submit.pl"
username="sgeadmin"
(clustername, imageid, instancetype, size, benchmark, userid, results, archive, inputdata) = parseOptions()
clusterparams=validate()
startCluster()
transferFiles()
runBenchmark()
getResults() #readtime computation time writetime memoryused cpuused
if(archive):
	saveResults()
terminateCluster()

#End
