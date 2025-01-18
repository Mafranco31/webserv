#!/usr/bin/env php
<?php
// Ensure the script is being run via CGI
if (php_sapi_name() !== 'cgi-fcgi') {
    exit(5);
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
            switch ($file['error']) {
                case UPLOAD_ERR_OK:
                    echo "No errors. The file uploaded successfully.<br>";
                    break;
                case UPLOAD_ERR_INI_SIZE:
                    echo "Error: The uploaded file exceeds the 'upload_max_filesize' directive in php.ini.<br>";
                    break;
                case UPLOAD_ERR_FORM_SIZE:
                    echo "Error: The uploaded file exceeds the 'MAX_FILE_SIZE' directive that was specified in the HTML form.<br>";
                    break;
                case UPLOAD_ERR_PARTIAL:
                    echo "Error: The uploaded file was only partially uploaded.<br>";
                    break;
                case UPLOAD_ERR_NO_FILE:
                    echo "Error: No file was uploaded.<br>";
                    break;
                case UPLOAD_ERR_NO_TMP_DIR:
                    echo "Error: Missing a temporary folder.<br>";
                    break;
                case UPLOAD_ERR_CANT_WRITE:
                    echo "Error: Failed to write file to disk.<br>";
                    break;
                case UPLOAD_ERR_EXTENSION:
                    echo "Error: A PHP extension stopped the file upload.<br>";
                    break;
                default:
                    echo "Unknown error occurred.<br>";
            }
            
            exit(1);
        }

        // Move the uploaded file to the target directory
        $destination = $uploadDir . basename($file['name']);
        if (move_uploaded_file($file['tmp_name'], $destination)) {
            exit(0);
        } else {
            exit(2);
        }
    } else {
        exit(3);
    }
} else {
    exit(4);
}

?>

