/*
** EPITECH PROJECT, 2023
** B-CPP-500-PAR-5-1-rtype-sofiane.bassaler
** File description:
** Encode
*/

#ifndef ENCODE_HPP_
    #define ENCODE_HPP_
#include <cstdint> // For uint8_t

std::vector<uint8_t> encodeToBytes(const std::string& input);
std::string decodeToString(const std::vector<uint8_t>& input);
void printBytes(const std::vector<uint8_t>& bytes);
#endif /* !ENCODE_HPP_ */
