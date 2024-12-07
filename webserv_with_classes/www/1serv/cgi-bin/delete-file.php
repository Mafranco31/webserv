<?php
// Set the content type to JSON
header('Content-Type: application/json');

// Get the input data (JSON)
$input = json_decode(file_get_contents('php://input'), true);
$fileName = $input['file'] ?? null;

// Check if file name is provided
if (!$fileName) {
    echo json_encode(['error' => 'File name is required']);
    http_response_code(400); // Bad Request
    exit;
}

// Define the directory containing files
$directory = './uploads'; // Replace with your actual directory

// Full file path
$filePath = $directory . "/" . $fileName;

// Attempt to delete the file
if (file_exists($filePath) && unlink($filePath)) {
    echo json_encode(['success' => true]);
} else {
    echo json_encode(['error' => 'Failed to delete file']);
    http_response_code(500); // Internal Server Error
}
?>
