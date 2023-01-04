#!/usr/bin/php-cgi

<?php
echo 'Bonjour ' . htmlspecialchars($_GET["lname"]) . htmlspecialchars($_GET["fname"]) . '!';
?>

