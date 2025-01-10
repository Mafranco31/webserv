<?php
// Set the content type to JSON
// header('Content-Type: application/json');

// Specify the directory you want to list
$directory = './uploads'; // Replace 'your-directory' with the path to your directory

// Function to get the list of files in the directory
function listFiles($dir) {
    if (!is_dir($dir)) {
        return ['error' => 'Directory not found'];
    }

    // Get an array of all files and directories in the specified directory
    $files = scandir($dir);

    // Filter out the special entries "." and ".."
    $files = array_filter($files, function($file) use ($dir) {
        return $file !== '.' && $file !== '..' && is_file($dir . DIRECTORY_SEPARATOR . $file);
    });

    // Return the filtered list of files
    return array_values($files); // Use array_values to reindex the array
}

// Get the list of files
$result = listFiles($directory);

// Output the result as JSON
header('Content-Type: text/plain');
header('Connexion: close');
header('Content-Length: ' . strlen(json_encode($result)));
echo json_encode($result);
?>