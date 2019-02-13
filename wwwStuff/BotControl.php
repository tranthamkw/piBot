<html>
<head>
<title>-= BOT Control =- </title>

<?php

$CMD="";
if ($_SERVER["REQUEST_METHOD"]=="POST"){
	$CMD=test_input($_POST["CMD"]);
	$angle=$_POST['angle'];
	$speed=$_POST['speed'];
	} else {
 $angle=2;
 $speed=128;
}
function test_input($data){
	$data=trim($data);
	$data=stripslashes($data);
	$data=htmlspecialchars($data);
	return $data;
	}

?>

</head>
<body>


<table align="center" border="0">
<tr>
<td align="center">
<b> BOT CONTROL</b><br>
<form name="form1"  method = "post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]);?>">
<input type ="text" name="speed" style="width:100px" value=<?php echo $speed; ?>><p>
<input type ="submit" name="left" value="LEFT">
<input type ="submit" name="forward" value="FORWARD">
<input type ="submit" name="right" value="RIGHT"><p>
<input type ="submit" name="turnleft" value="ROTLEFT">
<input type ="submit" name="stop" value="STOP">
<input type ="submit" name="turnright" value="ROTRIGHT"><p>
<input type ="submit" name="backward" value="BACKWARD"><p>

</form>
</td>
<td align="center">
<b> CAMERA CONTROL</b><br>
<form name="form2"  method = "post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]);?>">
<input type ="text" name="angle" style="width:100px" value=<?php echo $angle;?>><p>
<input type ="submit" name="setAngle" value="SET ANGLE"><p>


</form>
</td>
</tr>
<tr>
<td> <?php
	$speed=$_POST['speed'];
	$angle=$_POST['angle'];
	if (!preg_match("/^[1-9 0-9]*$/",$speed)){
	echo "invalid speed number";
	} else {
	if (isset($_POST['forward'])){
		// go forward
		echo "forward";
		$a=popen("/home/pi/piBot/piBoard4/botControlScript.sh '".$speed."' 8",'r');
		while($b=fgets($a,32)){
			echo $b;
			ob_flush();
			flush();
		}
		pclose($a);

	} else if (isset($_POST['stop'])){
		// stop
		$a=popen("/home/pi/piBot/piBoard4/botControlScript.sh 100 5",'r');
		while($b=fgets($a,32)){
			echo $b;
			ob_flush();
			flush();
		}
		pclose($a);
		echo "stop";

	} else if (isset($_POST['backward'])){
		echo "backward";
		$a=popen("/home/pi/piBot/piBoard4/botControlScript.sh '".$speed."' 2",'r');
		while($b=fgets($a,32)){
			echo $b;
			ob_flush();
			flush();
		}
		pclose($a);
	} else if (isset($_POST['setAngle'])){
		echo "Set Angle";
		$a=popen("/home/pi/piBot/piBoard4/setAngleScript.sh '".$angle."'",'r');
		while($b=fgets($a,32)){
			echo $b;
			ob_flush();
			flush();
		}
		pclose($a);

	} else if (isset($_POST['left'])){
		echo "vere left";
		$a=popen("/home/pi/piBot/piBoard4/botControlScript.sh '".$speed."' 7",'r');
		while($b=fgets($a,32)){
			echo $b;
			ob_flush();
			flush();
		}
		pclose($a);

	} else if (isset($_POST['right'])){
		echo "vere right";
		$a=popen("/home/pi/piBot/piBoard4/botControlScript.sh '".$speed."' 9",'r');
		while($b=fgets($a,32)){
			echo $b;
			ob_flush();
			flush();
		}
		pclose($a);


	} else if (isset($_POST['turnright'])){
		echo "turn right";
		$a=popen("/home/pi/piBot/piBoard4/botControlScript.sh '".$speed."' 6",'r');
		while($b=fgets($a,32)){
			echo $b;
			ob_flush();
			flush();
		}
		pclose($a);

	} else if (isset($_POST['turnleft'])){
		echo "turn left";
		$a=popen("/home/pi/piBot/piBoard4/botControlScript.sh '".$speed."' 4",'r');
		while($b=fgets($a,32)){
			echo $b;
			ob_flush();
			flush();
		}
		pclose($a);

	}
}

	?>
</td>
</tr>
</table>



</body>
</html>
