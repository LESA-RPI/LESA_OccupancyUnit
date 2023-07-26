<?php

// test this code by enter the link below in your browser: 
// Replacing 129.161.138.91 with the ip address of the PHP/MySQL node
// using campus vm: 128.113.216.70
//New URL format below
//http://128.113.216.70/sendToDatabase.php?BID=0&RID=0&SID=0&PosX=0&PosY=0&Speed=0.5&Direction=90&Height=150&cieX=0.01&cieY=0.01&Time='2023-12-12 12:12:12'
//ALTERNATIVE that also works: swapping out ip address with server name below
//http://mdl-vm11.eng.rpi.edu/sendToDatabase.php?BID=0&RID=0&SID=0&PosX=0&PosY=0&Speed=0.5&Direction=90&Height=150&cieX=0.01&cieY=0.01&Time='2023-12-12 12:12:12'

/*correct output in browser:
INSERT INTO occupancy.sensor_data (Building_ID,Room_ID,Sensor_ID,Position_X,Position_Y,Speed,Direction,Height,RGB_CIE_X,RGB_CIE_Y,RGB_CIE_Z,Date_Time_Logged,Time_Captured) 
VALUES (0, 0, 0, 0, 0, 0.5, 90, 150, 0.01, 0.01, 0.01, '2023-04-24 05:04:32', '2023-04-24 05:04:32');New record created successfully
*/

//TO DO: program needs to be able to breakup url with multiple occupants data into multiple entries.

// Set the New york time as default timezone
date_default_timezone_set('America/New_York');

//if statement checks isset (if SID != NULL), if there IS a sensor ID 
if(isset($_GET["SID"])) {
//$_GET[key] returns value of key/value pair
//$[name] declares a variable
   $BID = $_GET["BID"];
   $RID = $_GET["RID"];
   $SID = $_GET["SID"];
   //$time = $_GET["Time"];
   //TO DO: add time of capture
   $posX = $_GET["PosX"];
   $posY = $_GET["PosY"];
   $speed = $_GET["Speed"];
   $direction = $_GET["Direction"];
   $height = $_GET["Height"];
   $cieX = $_GET["cieX"];
   $cieY = $_GET["cieY"];
  
   /* For testing on own laptop： 
   $servername = "localhost";
   $username = "ESP32";
   $password = "esp32io.com";
   $database_name = "db_sensor_data";*/

   $servername = "localhost";
   $username = "occupancy_user";
   $password = "#Occ_LESA_ToF#";
   $database_name = "occupancy";

   // Create MySQL connection from PHP to MySQL server
   $connection = new mysqli($servername, $username, $password, $database_name);
   // Current date and time the server recieved the message from the client ESP
   $date = '\'' . date("Y-m-d h:m:s") . '\'';

   // Check connection
   if ($connection->connect_error) {
      //prints message and terminates the script
      die("MySQL connection failed: " . $connection->connect_error);
   }
//changed "occupancy.sensor_data" to "occupancy.sensor_data23" for new variables
   $sql = "INSERT INTO occupancy.sensor_data (Building_ID,Room_ID,Sensor_ID,Position_X,Position_Y,Speed,Direction,Height,RGB_CIE_X,RGB_CIE_Y,Date_Time_Logged,Time_Captured) VALUES ($BID, $RID, $SID, $posX, $posY, $speed, $direction, $height, $cieX, $cieY, $date, $date);";

   echo $sql;
//"$connection->query($sql)" is what send the command in $sql to the table in the database
//if its IDENTICAL (===) to TRUE, the command was sent successfully, echo successful message
   if ($connection->query($sql) === TRUE) {
      echo "New record created successfully";
   } else {
      echo "Error: " . $sql . " => " . $connection->error;
   }

   $connection->close();
} else {
//if MAC address = NULL, print error message in terminal
   echo "data is not set in the HTTP request";
}
?>
