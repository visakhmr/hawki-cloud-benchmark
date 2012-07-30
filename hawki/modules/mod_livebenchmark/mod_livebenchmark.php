<?php
//don't allow other scripts to grab and execute our file
defined('_JEXEC') or die('Direct Access to this location is not allowed.');

/*
Live Benchmark tool
Allows users to select configuration and run benchmark on demand
*/


$benchmarkid = $_POST['benchmarkid'];
$instanceid = $_POST['instanceid'];
$instancecount = $_POST['instancecount'];


$database =& JFactory::getDBO();
//Select all published benchmarks
$benchmarklist="SELECT `benchmarkid`, `benchmarkname`, `sourcepath`,`resultpath`,`description` from cx_benchmarks where state=1";
$database->setQuery ($benchmarklist);
$database->query();

if($database->getNumRows()==0) //no benchmarks
{
	echo "No benchmarks added";
}
else
{
	$benchmarks = $database->loadObjectList();
	$url=$_SERVER['REQUEST_URI']."/";
?>
	<div>
	<form action="" method="post">
		Benchmark: <select name="benchmarkid">
			<!--option selected="selected">Select Benchmark</option-->
			<?php
			 foreach ( $benchmarks as $benchmark ){?>
			  <?php if($benchmarkid == $benchmark->benchmarkid) {?>
			   <option selected="selected" value="<?php echo $benchmark->benchmarkid;?>"><?php echo $benchmark->benchmarkname; ?></option>
			  <?php }
			  else{
			  ?>
			   <option value="<?php echo $benchmark->benchmarkid;?>"><?php echo $benchmark->benchmarkname; ?></option>
			  <?php
			   } 
			 }
			 ?>
		</select>
<?php
}


//Select all published benchmarks
$instancelist="SELECT `instanceid`, `instancename`, `description`, `vendor` from cx_instances where state=1";
$database->setQuery ($instancelist);
$database->query();

if($database->getNumRows()==0) //no instances
{
        echo "No instances added";
}
else
{
        $instances = $database->loadObjectList();
        $url=$_SERVER['REQUEST_URI']."/";
?>
                Instance type: <select name="instanceid">
                        <!--option selected="selected">Select instance</option-->
                        <?php
                         foreach ( $instances as $instance ){?>
                          <?php if($instanceid == $instance->instanceid) {?>
                           <option selected="selected" value="<?php echo $instance->instanceid;?>"><?php echo $instance->instancename; ?></option>
                          <?php }
                          else{
                          ?>
                           <option value="<?php echo $instance->instanceid;?>"><?php echo $instance->instancename; ?></option>
                          <?php
                           }
                         }
                         ?>
                </select>
<?php
} ?>
		Number of instances: <select name="instancecount">
                        <?php
                         for ($i=1;$i<=16;$i++){ ?>
                          <?php if($instancecount == $i) { ?>
                           <option selected="selected" value="<?php echo $i;?>"><?php echo $i; ?></option>
                          <?php }
                          else{
                          ?>
                           <option value="<?php echo $i; ?>"><?php echo $i; ?></option>
                          <?php
                           }
                         }
                         ?>
		</select>
		<br />		
		<input type=submit style="margin-bottom: 20px;" value="Select input size"/>
		<br />
	</form>
	</div>
<?php


if (!($benchmarkid=="" && $instanceid=="" && $instancecount==""))
{
	$inputid = $_POST['inputid'];
	//Read input id from user
	$database =& JFactory::getDBO();
	//Select all published benchmarks
	$inputlist="SELECT `inputid`, `benchmarkid`, `inputname`,`state` from cx_input where benchmarkid=$benchmarkid and state=1";
	$database->setQuery ($inputlist);
	$database->query();
	
	if($database->getNumRows()==0) //no inputs
	{
	        echo "No inputs added";
	}
	else
	{
	        $inputs = $database->loadObjectList();
	        $url=$_SERVER['REQUEST_URI']."/";
	?>
	        <div>
	        <form action="" method="post">
			<input type=hidden name="benchmarkid" value="<?php echo $benchmarkid; ?>">
			<input type=hidden name="instanceid" value="<?php echo $instanceid; ?>">
			<input type=hidden name="instancecount" value="<?php echo $instancecount; ?>">
	                Input: <select name="inputid">
	                        <!--option selected="selected">Select Benchmark</option-->
	                        <?php
	                         foreach ( $inputs as $input ){?>
	                          <?php if($inputid == $input->inputid) {?>
	                           <option selected="selected" value="<?php echo $input->inputid;?>">
					<?php echo $input->inputname; ?>
				   </option>
	                          <?php }
	                          else{
	                          ?>
	                           <option value="<?php echo $input->inputid;?>"><?php echo $input->inputname; ?></option>
	                          <?php
	                           }
        	                 }
        	                 ?>
        	        </select>
		<input type=submit value="Run Benchmark"/>
		</form>
		</div>
	<?php
	}	
	
	
	if(!($inputid==""))//RUN BENCHMARK
	{
		echo "<br />Running Benchmark (This will take a few minute)...";
		$benchmark="";
		$input="";
		$instance="";
		//Get benchmark source path, results folder using benchmarkid
		$benchmarklist="SELECT `benchmarkname`, `sourcepath`,`resultpath`, `description` from cx_benchmarks where benchmarkid=$benchmarkid and state=1";
        	$database->setQuery ($benchmarklist);
        	$database->query();

        	if($database->getNumRows()==0) 
        	{
        	        die("<br />No benchmark details returned from database !");
        	}
        	else
        	{	$benchmark = $database->loadAssoc();
		}

		//Get inputname using input id
                $inputlist="SELECT `inputname` from cx_input where inputid=$inputid and state=1";
                $database->setQuery ($inputlist);
                $database->query();
                
                if($database->getNumRows()==0) 
                {
                        die("<br />No input details returned from database !");
                }
                else
                {       $input = $database->loadAssoc();
                }

		//Get instance name using instanceid
                $instancelist="SELECT `instancename`, `description`, `vendor` from cx_instances where instanceid=$instanceid and state=1";
                $database->setQuery ($instancelist);
                $database->query();
                
                if($database->getNumRows()==0) 
                {
                        die("<br />No instance details returned from database !");
                }
                else
                {       $instance = $database->loadAssoc();
                }


		//RUN using sourcepath, resultpath, inputname
		#echo $benchmark['sourcepath'].$benchmark['resultpath'].$input['inputname'].$instance['instancename'];
		echo "<pre>";
		$cmd = "python /home/ubuntu/Hawk-i/environment/manager.py -t ".$instance['instancename']." -b /home/ubuntu/Hawk-i/benchmark/".$benchmark['sourcepath']." -r /home/ubuntu/Hawk-i/environment/".$benchmark['resultpath']." -s $instancecount";
		#echo $cmd;
		if(system($cmd))
		{
			#echo $output;
			echo "</pre><br /> Please Go to your home page to view the results";
		}
		else
		{
			echo "<br><h3> An error occured during benchmark execution, please contact administrator</h3>";
		}
	}
}

?>
