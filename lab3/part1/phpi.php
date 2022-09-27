<?php
$output = shell_exec('pwd');
echo "$output";

$output = shell_exec('ls');
echo "$output";

$output = shell_exec('ls /');
echo "$output";

$output = shell_exec('ps aux --no-headers | wc -l');
echo "$output";
?>

