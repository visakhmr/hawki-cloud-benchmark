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

$document = &JFactory::getDocument();
$document->addScript('modules'.DS.'mod_report'.DS.'sorttable.js');
$database =& JFactory::getDBO();
$user =& JFactory::getUser();
$userid = $user->id;
//Select all published benchmarks
$resultslist="SELECT  `currenttime` ,  (select `benchmarkname` from cx_benchmarks where cx_benchmarks.`benchmarkid`=cx_results.`benchmarkid`) as `benchmarkname` ,  (select `instancename` from cx_instances where cx_instances.`instanceid`=cx_results.`instanceid`) as `instancename` , (select `inputstring` from cx_input where cx_input.`inputid`=cx_results.`inputid`) as `inputstring` , `size` ,  `timetocompletion`, `status` FROM  `cx_results` WHERE `userid`=$userid";

#print $resultslist;

$database->setQuery ($resultslist);
$database->query();

if($database->getNumRows()==0) //no benchmarks
{
	echo "No benchmarks added";
}
else
{
	$results = $database->loadObjectList();
?>
	<style>
	.sortable
	{
		border: 1px solid #CCC;

	}
	.sortable td
	{
		border: 0;	
		text-align: right;	
	}
	.sortable th
	{
		cursor: pointer;
		border: 1px solid #CCC;
		text-align: center;
		padding: 5px 15px;
	}
	.tr0
	{
		background-color : #fff;
        }
	.tr1
	{
		background-color: #ddd;
	}

	</style>
	<table class="sortable">
	<th>Date</th><th>Benchmark</th><th>Instance type</th><th>Input</th><th>Count</th><th>Execution Time</th><th>Status</th>
<?php
	$index=0;
	foreach ( $results as $result ){
?>
		<tr class="tr<?php echo $index++%2;?>">
		<td><?php echo $result->currenttime;?></td>
		<td><?php echo $result->benchmarkname;?></td>
		<td><?php echo $result->instancename;?></td>
		<td><?php echo $result->inputstring;?></td>
		<td><?php echo $result->size;?></td>
		<td><?php echo $result->timetocompletion;?></td>
		<td><?php if($result->status==1) echo "Success"; else echo "Failed";?></td>
		</tr>
<?php	}
?>
	</table>
<?php
}
?>
