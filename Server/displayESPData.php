<!DOCTYPE html>
<html><body>
<?php
/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-mysql-database-php/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

# Testing on local server (my laptop)
$servername = "localhost";
// REPLACE with your Database name
$dbname = "occupancy";
// REPLACE with Database user
$username = "root";
// REPLACE with Database user password
$password = "";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 

$sql = "SELECT * FROM occupancy.sensor_data ORDER BY Time_Captured DESC";

echo '<table cellspacing="5" cellpadding="5">
      <tr> 
        <td>Building_ID</td> 
        <td>Room_ID</td> 
        <td>Sensor_ID</td> 
        <td>Position_X</td> 
        <td>Position_Y</td> 
        <td>Speed</td> 
        <td>Direction</td> 
        <td>Distance</td> 
        <td>RGB_CIE_X</td>
        <td>RGB_CIE_Y</td> 
        <td>Date_Time_Logged</td> 
        <td>Time_Captured</td> 
      </tr>';
 
if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $BID = $row["BID"];
        $RID = $row["RID"];
        $SID = $row["SID"];
        $posX = $row["posX"];
        $posY = $row["posY"];
        $speed = $row["speed"];
        $direction = $row["direction"];
        $height = $row["height"];
        $cieX = $row["cieX"];
        $cieY = $row["cieY"];
        $Date_Time_Logged = $row["Date_Time_Logged"];
        $Time_Captured = $row["Time_Captured"];

        // Uncomment to set timezone to - 1 hour (you can change 1 to any number)
        //$row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time - 1 hours"));
      
        // Uncomment to set timezone to + 4 hours (you can change 4 to any number)
        //$row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time + 4 hours"));
      
        echo '<tr> 
                <td>' . $BID . '</td> 
                <td>' . $RID . '</td> 
                <td>' . $SID . '</td> 
                <td>' . $posX . '</td> 
                <td>' . $posY . '</td>
                <td>' . $speed . '</td> 
                <td>' . $direction . '</td> 
                <td>' . $height . '</td> 
                <td>' . $cieX . '</td> 
                <td>' . $cieY . '</td> 
                <td>' . $Date_Time_Logged . '</td> 
                <td>' . $Time_Captured . '</td> 
              </tr>';
    }
    $result->free();
}

$conn->close();
?> 
</table>
</body>
</html>