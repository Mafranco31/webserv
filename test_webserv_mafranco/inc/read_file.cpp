#ifndef READ_FILE_CPP
# define READ_FILE_CPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

std::string readFileToString(const std::string& filename) {
    // Create an ifstream object to read the file
    std::ifstream file(filename);

    // Check if the file was opened successfully
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return "";
    }

    // Use a stringstream to read the file contents into a std::string
    std::stringstream buffer;
    buffer << file.rdbuf(); // Read the entire file into the buffer

    // Close the file
    file.close();

    // Return the contents of the file as a std::string
    return buffer.str();
}

#endif