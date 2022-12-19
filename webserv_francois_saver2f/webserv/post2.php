#!/usr/bin/php-cgi

<?php
echo 'Hello ' . htmlspecialchars($_POST["fname"]) . ' ' . htmlspecialchars($_POST["lname"]) . '!!!';
?>