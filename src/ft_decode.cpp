#include "../inc/header.hpp"

int hexToDec(char hex) {
    if (hex >= '0' && hex <= '9') return hex - '0';
    if (hex >= 'A' && hex <= 'F') return hex - 'A' + 10;
    if (hex >= 'a' && hex <= 'f') return hex - 'a' + 10;
    return -1;
}

// Function to decode a percent-encoded string
std::string decodePercentEncoding(const std::string &encoded) {
    std::string decoded;
    size_t length = encoded.size();

    for (size_t i = 0; i < length; ++i) {
        if (encoded[i] == '%' && i + 2 < length && 
            std::isxdigit(encoded[i + 1]) && std::isxdigit(encoded[i + 2])) {
            char high = encoded[i + 1];
            char low = encoded[i + 2];
            int highValue = hexToDec(high);
            int lowValue = hexToDec(low);
            if (highValue != -1 && lowValue != -1) {
                char decodedChar = static_cast<char>((highValue << 4) | lowValue);
                // Check if the character is within the printable ASCII range
                if (decodedChar >= 0x20 && decodedChar <= 0x7E) {
                    decoded += decodedChar;
                }
                else throw std::invalid_argument("invalid argument");
                i += 2;
            } else {
                decoded += '%';
            }
        } else if (encoded[i] == '+') {
            decoded += ' ';
        } else {
            decoded += encoded[i];
        }
    }

    return decoded;
}
