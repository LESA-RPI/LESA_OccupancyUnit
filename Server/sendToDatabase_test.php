<?php

// test this code by enter the link below in your browser: 
// Replacing 129.161.138.91 with the ip address of the PHP/MySQL node
// using campus vm: 128.113.216.70
//New URL format below
//http://128.113.216.70/sendToDatabase_test.php?column1=123&Time='2023-12-12 12:12:12'
//ALTERNATIVE that also works: swapping out ip address with server name below
//http://mdl-vm11.eng.rpi.edu/sendToDatabase_test.php?column1=123&Time='2023-12-12 12:12:12'

/*correct output in browser:
php file calledMySQL connectedINSERT INTO occupancy.test (column1) VALUES (123);New record created successfully
*/


$servername = "localhost";
$username = "occupancy_user";
$password = "#Occ_LESA_ToF#";
$database_name = "occupancy";

// Set the New york time as default timezone
date_default_timezone_set('America/New_York');

echo "php file called";

if(isset($_GET["column1"])) {
   echo $_REQUEST;
   $column1 = $_GET["column1"];
  
   // Create MySQL connection from PHP to MySQL server
   $connection = new mysqli($servername, $username, $password, $database_name);
   // Current date and time the server recieved the message from the client ESP
   $date = '\'' . date("Y-m-d h:m:s") . '\'';   
   
   // Create connection
   $conn = new mysqli($servername, $username, $password, $database_name);
   // Check connection
   if ($conn->connect_error) {
      die("Connection failed: " . $conn->connect_error);
   }
   echo "MySQL connected";

   $sql = "INSERT INTO occupancy.test (column1) VALUES ($column1);";
   echo $sql;

   if ($conn->query($sql) === TRUE) {
   echo "New record created successfully";
   } else {
   echo "Error: " . $sql . "<br>" . $conn->error;
   }

   $connection->close();
} else {
//if MAC address = NULL, print error message in terminal
   echo "data is not set in the HTTP request";}







?>