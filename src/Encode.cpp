#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

// Converts a string to a vector of bytes (encoding)
std::vector<uint8_t> encodeToBytes(const std::string& input) {
    return std::vector<uint8_t>(input.begin(), input.end());
}

// Converts a vector of bytes back to a string (decoding)
std::string decodeToString(const std::vector<uint8_t>& input) {
    return std::string(input.begin(), input.end());
}

// Prints the bytes in a readable hexadecimal format
void printBytes(const std::vector<uint8_t>& bytes) {
    for (uint8_t byte : bytes) {
        // Print each byte as a two-digit hexadecimal number.
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << "\n"; // Switch back to decimal for normal output after this.
}
