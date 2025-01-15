#!/usr/bin/env php

<?php
// Ensure the script is being run via CGI
if (php_sapi_name() !== 'cgi-fcgi') {
    echo "This script must be run as a CGI application.\n";
    exit(1);
}

// Define the upload directory
$uploadDir = __DIR__ . '/uploads/';
if (!is_dir($uploadDir)) {
    mkdir($uploadDir, 0755, true);
}

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    if (isset($_FILES['file'])) {
        $file = $_FILES['file'];

        // Check for upload errors
        if ($file['error'] !== UPLOAD_ERR_OK) {
            exit(1);
        }

        // Move the uploaded file to the target directory
        $destination = $uploadDir . basename($file['name']);
        if (move_uploaded_file($file['tmp_name'], $destination)) {
            exit(0);
        } else {
            exit(1);
        }
    } else {
        exit(1);
    }
} else {
    exit(1);
}

?>
