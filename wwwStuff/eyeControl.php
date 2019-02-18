<html>
<head>
<title>-= EYE Control =- </title>

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

<b> EYE CONTROL</b><br>

<table align="center" border="1">
<tr><th>Button</th><th>Image</th></tr>

<form name="form1"  method = "post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]);?>">
<tr><td> <input type ="submit" name="norm" value="Norm"></td><td><img src="botEyeNormL.bmp"><img src="botEyeNormR.bmp"></td></tr>
<tr><td> <input type ="submit" name="grrr" value="Grrr"></td><td><img src="botEyeMadL.bmp"><img src="botEyeMadR.bmp"></td></tr>
<tr><td> <input type ="submit" name="supp" value="Supp"></td><td><img src="botEyeSupL.bmp"><img src="botEyeSupR.bmp"></td></tr>
<tr><td> <input type ="submit" name="clos" value="clos"></td><td><img src="botEyeClosedL.bmp"><img src="botEyeClosedR.bmp"></td></tr>
<tr><td> <input type ="submit" name="gsxx" value="gsxx"></td><td><img src="botEyeMadL.bmp"><img src="botEyeSupR.bmp"></td></tr>

</form>
</table>



 <?php

	echo "<pre>";
	if (isset($_POST['norm'])){
		$a=popen("/home/pi/piBot/piBoard4/eyeControlScript.sh botEyeNormL.bmp botEyeNormR.bmp",'r');
		while($b=fgets($a,32)){
			echo $b;
			ob_flush();
			flush();
		}
		pclose($a);

	} else if (isset($_POST['gsxx'])){
		$a=popen("/home/pi/piBot/piBoard4/eyeControlScript.sh botEyeMadL.bmp botEyeSupR.bmp",'r');
		while($b=fgets($a,32)){
			echo $b;
			ob_flush();
			flush();
		}
		pclose($a);
	} else if (isset($_POST['clos'])){
		$a=popen("/home/pi/piBot/piBoard4/eyeControlScript.sh botEyeClosedL.bmp botEyeClosedR.bmp",'r');
		while($b=fgets($a,32)){
			echo $b;
			ob_flush();
			flush();
		}
		pclose($a);
	} else if (isset($_POST['supp'])){
		$a=popen("/home/pi/piBot/piBoard4/eyeControlScript.sh botEyeSupR.bmp botEyeSupL.bmp",'r');
		while($b=fgets($a,32)){
			echo $b;
			ob_flush();
			flush();
		}
		pclose($a);


	} else if (isset($_POST['grrr'])){
		$a=popen("/home/pi/piBot/piBoard4/eyeControlScript.sh botEyeMadL.bmp botEyeMadR.bmp",'r');
		while($b=fgets($a,32)){
			echo $b;
			ob_flush();
			flush();
		}
		pclose($a);

	}
	echo "</pre>\n";

	?>


</body>
</html>
