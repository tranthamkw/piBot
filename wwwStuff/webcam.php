<html>


<table align="center">
<tr>
<td align="center">

<?php
$output = trim(shell_exec('hostname -I'));
//echo $output;
$prefix = '<img src="http://';
$suffix = ':8081">';
echo ($prefix.$output.$suffix);
?>
</td>
</tr>

</html>
