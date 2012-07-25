#!/usr/bin/perl -w

$executable="fft_benchmark";
$numproc=$ARGV[0];

#Compile the source
system("make");

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
        while(1)
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
                sleep(2);
        }

        print "Job ID: $jobId completed $numproc processes\n";
        my $file = "image.sge.o$jobId";
