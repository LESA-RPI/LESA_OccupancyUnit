<?php

// Set the New york time as default timezone
date_default_timezone_set('America/New_York');


//server info, will need ssh for remote server
/*$servername = "localhost";
$username = "occupancy_user";
$password = "#Occ_LESA_ToF#";
$database_name = "occupancy";*/

# Testing on local server (my laptop)
$servername = "localhost";
// REPLACE with your Database name
$dbname = "occupancy";
// REPLACE with Database user
$username = "root";
// REPLACE with Database user password
$password = "";

// Keep this API Key value to be compatible with the ESP32 code provided in the project page. 
// If you change this value, the ESP32 sketch needs to match
$api_key_value = "1234567890";

$api_key= $BID=$RID=$SID=$posX=$posY=$speed=$direction=$height=$cieX=$cieY = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $api_key = test_input($_POST["api_key"]);
    if($api_key == $api_key_value) {
        // Current date and time the server recieved the message from the client ESP
        $date = '\'' . date("Y-m-d H:m:s") . '\'';
        // Post data
        $BID = test_input($_POST["BID"]);
        $RID = test_input($_POST["RID"]);
        $SID = test_input($_POST["SID"]);
        $posX = test_input($_POST["posX"]);
        $posY = test_input($_POST["posY"]);
        $speed = test_input($_POST["speed"]);
        $direction = test_input($_POST["direction"]);
        $height = test_input($_POST["height"]);
        $cieX = test_input($_POST["cieX"]);
        $cieY = test_input($_POST["cieY"]);

        //changed "occupancy.sensor_data" to "occupancy.sensor_data23" for new variables
        $sql = "INSERT INTO occupancy.sensor_data (Building_ID,Room_ID,Sensor_ID,Position_X,Position_Y,Speed,Direction,Distance,RGB_CIE_X,RGB_CIE_Y,Date_Time_Logged,Time_Captured) VALUES ($BID, $RID, $SID, $posX, $posY, $speed, $direction, $height, $cieX, $cieY, $date, $date);";

        echo nl2br("($BID, $RID, $SID, $posX, $posY, $speed, $direction, $height, $cieX, $cieY, $date, $date)\r\n"); 
        
        // Create connection
        $conn = new mysqli($servername, $username, $password, $dbname);
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
        
        $sql = "INSERT INTO SensorData (sensor, location, value1, value2, value3)
        VALUES ('(" . $BID . "', '" . $RID . "', '" . $SID . "', '" . $posX . "', '" . $posY . "', '" . $speed . "', '" . $direction . "', '" . $height . "', '" . $cieX . "', '" . $cieY . "', '" . $date . "', '" . $date . "')";
        
        //"$connection->query($sql)" is what send the command in $sql to the table in the database
        //if its IDENTICAL (===) to TRUE, the command was sent successfully, echo successful message
        if ($conn->query($sql) === TRUE) {
            echo nl2br("New record created successfully \n");
        } 
        else {
            echo "Error: " . $sql . "<br>" . $conn->error;
        }
    
        $conn->close();
    }
    else {
        echo "Wrong API Key provided.";
    }

}
else {
    echo "No data posted with HTTP POST.";
}
/*
function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}
*/





















function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}




?>
