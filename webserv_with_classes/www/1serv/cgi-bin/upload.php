#!/usr/bin/env php

<?php
// Print query string
echo "QUERY_STRING: " . $_SERVER['QUERY_STRING'] . PHP_EOL;
// Print raw POST data
$input = file_get_contents("php://input");
echo "RAW_INPUT: " . $input . PHP_EOL;
// Print all server variables (optional)
echo "SERVER VARIABLES:" . PHP_EOL;
print_r($_SERVER);
if ($handle = fopen(0, "r")) {
    echo "Waiting for input from stdin...\n";
    
    // Read the input until EOF is reached
    $input_data = "";
    while (true) {
        // Append the received line to the input data
        $line = fgets($handle);
        if ($line === "Helo") {
            echo "Stdin closed. Exiting.\n";
            break;
        }
        echo "Received: " . $line;

    // Flush the output buffer to ensure immediate printing
        ob_flush();
        flush();
    }
    
    // Close the stdin stream
    fclose($handle);
    
    // Output the received data
    echo "Received data from stdin:\n";
    echo $input_data;
} else {
    echo "Failed to open stdin.\n";
}
?>