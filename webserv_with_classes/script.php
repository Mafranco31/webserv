#!/usr/bin/env php

<?php
// Set content type to plain text for easier debugging
header('Content-Type: text/plain');

// Get the raw input data from the request body
$rawInput = file_get_contents('php://input');

// Print the raw input
echo "Raw Input:\n";
echo $rawInput . "\n";  // Ensure there's a newline after the raw input

// Optionally, you can also print some debugging information
echo "\n\n--- Debugging Information ---\n";

print_r($_SERVER);

// Print request method
echo "REQUEST_METHOD: " . $_SERVER['REQUEST_METHOD'] . "\n";

// Print query parameters if present
if (!empty($_GET)) {
    echo "\nGET Parameters:\n";
    print_r($_GET);
}

// Print POST parameters if present
if (!empty($_POST)) {
    echo "\nPOST Parameters:\n";
    print_r($_POST);
}

// Print uploaded files if present
if (!empty($_FILES)) {
    echo "\nUploaded Files:\n";
    print_r($_FILES);
}
?>

