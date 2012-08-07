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
$user =& JFactory::getUser();
$userid = $user->id;

if ($_POST['file']=="")
{
	$allowedExts = array("tar");
	$extension = end(explode(".", $_FILES["file"]["name"]));
	if (($_FILES["file"]["type"] == "application/x-tar") && ($_FILES["file"]["size"] < 2000000) && in_array($extension, $allowedExts))
	{
		if ($_FILES["file"]["error"] > 0)
		{
	   		 echo "Error: " . $_FILES["file"]["error"] . "<br />";
	    	}
	  	else
	    	{
	    	echo "Upload: " . $_FILES["file"]["name"] . "<br />";
	    	echo "Type: " . $_FILES["file"]["type"] . "<br />";
	    	echo "Size: " . ($_FILES["file"]["size"] / 1024) . " Kb<br />";
	    	echo "Stored in: " . $_FILES["file"]["tmp_name"];
	   	}
    
		if (file_exists("benchmark/" . $_FILES["file"]["name"]))
	     	{
	     		echo $_FILES["file"]["name"] . " already exists. ";
      		}
		else
      		{
      			move_uploaded_file($_FILES["file"]["tmp_name"], "benchmark/" . $_FILES["file"]["name"]);
      			echo "Stored in: " . "benchmark/" . $_FILES["file"]["name"];
			$benchmarkname = basename($_FILES["file"]["name"], ".tar");
			system("tar -xvf benchmark/$benchmarkname.tar -C benchmark/");
			system("rm -f benchmark/$benchmarkname.tar");
			$description=$_POST['description'];

		//todo store name and description in database 

			$benchmarklist="INSERT into cx_benchmarks (`benchmarkname`, `sourcepath`,`resultpath`,`description`, `state`) VALUES('$benchmarkname','$benchmarkname','$benchmarkname"."_results','$description', 1)";
			$database->setQuery ($benchmarklist);

			if(!$database->query()) //no benchmarks
			{
			        echo "\nError in inserting benchmark details";
			}
			else
			{
        			echo "\nInserted";
			}

	      	}
	}
	else
	{
		echo "<br>File too big</br>";
}
	
}
?>
<div>
	<form action="" method="post" enctype="multipart/form-data">
	<label for="file">Benchmark source (.tar): </label>
	<input type="file" name="file" id="file" /> <a href="template.tar">Template</a>
	<br />
	<input type="submit" name="submit" value="Submit" />
	<label for="description"> Description</label><textarea name="description"></textarea>
	</form>
</div>

