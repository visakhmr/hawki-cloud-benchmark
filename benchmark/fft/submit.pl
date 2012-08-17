#!/usr/bin/perl -w

$executable="benchmark";
$threshold=$ARGV[2];
$commandline=$ARGV[1];
$numproc=$ARGV[0];

system("swapoff -a");
system("sed '/#INPUT=/ i INPUT=\`echo $commandline\`' benchmark.sge > benchmark.temp");
system("mv benchmark.temp benchmark.sge");
sleep (2);
#Compile the source
system("make");

	#update input datafile name
	
        # submit job
        $cmd = "qsub -cwd -pe orte $numproc $executable.sge";
        print $cmd."\n";

        my $ret = `$cmd`;
        die "Error: $?" if($? != 0);

        chomp($ret);
        my @words = split(/ /,$ret);
        my $jobId = $words[2];

        print "Job ID: $jobId submitted for $numproc processes\n";

        #check queue
	$starttime = time();
        while($starttime+$threshold > time())
        {
                my $line = "";

                open(QUEUE,"qstat | grep $jobId |") or die "Unable to run qstat";
                while(<QUEUE>)
                {
                        chomp;
                        $line = $_;
                }
                close QUEUE;

                last if($line eq "");
		sleep (1);
        }

