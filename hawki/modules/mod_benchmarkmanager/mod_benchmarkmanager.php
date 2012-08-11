<?php
//don't allow other scripts to grab and execute our file
defined('_JEXEC') or die('Direct Access to this location is not allowed.');

/*
Live Benchmark tool
Allows users to select configuration and run benchmark on demand
*/



$database =& JFactory::getDBO();
$user =& JFactory::getUser();
$userid = $user->id;

function insert_inputs($benchmarkid) 
{
echo "Benchmark".$benchmarkid;

		//todo inputs in database 

			$inputs = split(',', $_POST['inputs']);
			$values="";
			foreach ($inputs as $input) 
			{
				$values=$values."($benchmarkid, '$input', 1),";
			}
			#remove last comma
			$values=rtrim($values, ",");
			#echo "<br>$values<br>";	
			
			$inputlist="INSERT into cx_input (`benchmarkid`, `inputstring`, `state`) VALUES ".$values;
			#echo $inputlist;
			$database->setQuery ($inputlist);

			if(!$database->query())
			{
			        die("\nError in inserting inputs");
			}
			else
			{
        			echo "\nInserted inputs";
			}
}


if($_POST['benchmarkid']=="")
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
			        die("\nError in inserting benchmark details");
			}
			else
			{
        			echo "\nInserted benchmark";
			}
			$benchmarkid = $database->insertid(); 

	      	}
#		insert_inputs($benchmarkid);
//todo inputs in database 

                        $inputs = split(',', $_POST['inputs']);
                        $values="";
                        foreach ($inputs as $input)
                        {
                                $values=$values."($benchmarkid, '$input', 1),";
                        }
                        #remove last comma
                        $values=rtrim($values, ",");
                        #echo "<br>$values<br>";        

                        $inputlist="INSERT into cx_input (`benchmarkid`, `inputstring`, `state`) VALUES ".$values;
                        #echo $inputlist;
                        $database->setQuery ($inputlist);

                        if(!$database->query()) 
                        {
                                die("\nError in inserting inputs");
                        }
                        else
                        {
                                echo "\nInserted inputs";
                        }


	}
	else
	{
		echo "<br>Please Choose a file and Enter command line inputs</br>";
	}	
}

else
{
	echo "Saving Inputs...";
	$benchmarkid=$_POST['benchmarkid'];
#	insert_inputs($benchmarkid);
//todo inputs in database 

                        $inputs = split(',', $_POST['inputs']);
                        $values="";
                        foreach ($inputs as $input)
                        {
                                $values=$values."($benchmarkid, '$input', 1),";
                        }
                        #remove last comma
                        $values=rtrim($values, ",");
                        #echo "<br>$values<br>";        

                        $inputlist="INSERT into cx_input (`benchmarkid`, `inputstring`, `state`) VALUES ".$values;
                        #echo $inputlist;
                        $database->setQuery ($inputlist);

                        if(!$database->query()) 
                        {
                                die("\nError in inserting inputs");
                        }
                        else
                        {
                                echo "\nInserted inputs";
                        }


}

?>
<h3>Add new benchmark</h3>
<div>
	<table>
	<form action="" method="post" enctype="multipart/form-data">
	<tr>
		<td><label for="file">Benchmark source (.tar): </label></td>
		<td><input type="file" name="file" id="file" /> <a href="template.tar">Template</a></td>
	</tr>
	<tr>	<td><label for="description"> Description</label></td>
		<td><textarea name="description"></textarea></td>
	</tr>
	<tr>	<td><label for="inputs">Inputs (comma separated)</label></td>
		<td><input type="text" name="inputs"/></td>
	</tr>
	<tr>	<td>&nbsp;</td>
		<td><input type="submit" name="submit" value="Submit" /></td>
	</tr>
	</form>
	</table>
</div>

<hr>

<!------ Second form -->
<h3> Add new inputs </h3>
<?php
$benchmarklist="SELECT `benchmarkid`, `benchmarkname` from cx_benchmarks where state=1";
$database->setQuery ($benchmarklist);
$database->query();
$benchmarkid=$_POST['benchmarkid'];

if($database->getNumRows()==0) //no benchmarks
{
        echo "No benchmarks added";
}
else
{
        $benchmarks = $database->loadObjectList();
        $url=$_SERVER['REQUEST_URI']."/";
?>

	<table>
	<form action="" method="post">
	<tr>
		<td><label for="file">Benchmark </label></td>
		<td>
		<select name="benchmarkid">
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
		</td>
	</tr>
	<tr>	<td><label for="inputs">New Inputs (comma separated)</label></td>
		<td><input type="text" name="inputs"/></td>
	</tr>
	<tr>	<td>&nbsp;</td>
		<td><input type="submit" name="submit" value="Submit" /></td>
	</tr>
	</form>
	</table>
</div>
<?php
}
?>
