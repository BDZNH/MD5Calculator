#pragma once
#include<string>
#include<vector>
namespace Utils {
    std::string Base64Encode(const unsigned char* data, size_t size);
    bool Base64Decode(const std::string& src, std::vector<unsigned char>& out);
}