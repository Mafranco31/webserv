#!/usr/bin/env php

<?php
// Set the content type to application/json
header('Content-Type: application/json');

// Specify the uploads directory
$uploadDir = __DIR__ . '/uploads';

// Check if the directory exists
if (!is_dir($uploadDir)) {
    // Return an error response if the directory does not exist
    echo json_encode(['error' => 'Uploads directory does not exist.']);
    exit;
}

// Initialize an array to hold the list of files
$fileList = [];

// Open the directory
if ($handle = opendir($uploadDir)) {
    // Loop through the files in the directory
    while (false !== ($entry = readdir($handle))) {
        // Skip the current directory (.) and parent directory (..)
        if ($entry != '.' && $entry != '..') {
            $fileList[] = $entry; // Add file name to the list
        }
    }
    closedir($handle); // Close the directory handle
}

// Return the list of files in JSON format
$encoded = json_encode($fileList);
header('Content-Type: application/json');
header('Content-Length: ' . strlen($encoded) + 1);
echo $encoded;
?>