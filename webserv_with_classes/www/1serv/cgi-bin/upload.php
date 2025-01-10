#!/usr/bin/env php

<?php
print_r ($_FILES['userfile']);
if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_FILES['userfile'])) {
    $originalName = $_FILES['userfile']['name']; // Original name of the uploaded file
    $tempPath = $_FILES['userfile']['tmp_name']; // Path to the temporary file
    $fileType = $_FILES['userfile']['type'];     // MIME type of the file
    $fileSize = $_FILES['userfile']['size'];     // Size of the file in bytes
    
    echo "Original File Name: $originalName\n";
    echo "Temporary File Path: $tempPath\n";
    echo "File Type: $fileType\n";
    echo "File Size: $fileSize bytes\n";
} else {
    echo "No file uploaded or incorrect request.";
}
?>

<!-- <?php
// Set the Content-Type for the response

//echo "yo";

// Check if a file was uploaded
if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_FILES['userfile'])) {
    $file = $_FILES['userfile'];

    // Check for errors
    if ($file['error'] === UPLOAD_ERR_OK) {
        // Define the target directory
        $uploadDir = './www/1serv/cgi-bin/uploads/';
        $uploadPath = $uploadDir . basename($file['name']);

        // Move the uploaded file to the target directory
        if (move_uploaded_file($file['tmp_name'], $uploadPath)) {
            echo "<p>File uploaded successfully to: $uploadPath</p>";
        } else {
            echo "<p>Failed to move the uploaded file.</p>";
        }
    } else {
        echo "<p>File upload error: " . $file['error'] . "</p>";
    }
} else {
    echo "<p>No file uploaded.</p>";
}
?> -->
