#include"Utils.h"
#include <openssl/evp.h>
std::string Utils::Base64Encode(const unsigned char* data, size_t size)
{
    size_t base64_len = (size + 2) / 3 * 4;
    if (base64_len == 0)
    {
        return "";
    }
    std::string ret;
    ret.resize(base64_len);
    EVP_EncodeBlock((unsigned char*)ret.data(), data, static_cast<int>(size));
    return std::move(ret);
}

bool Utils::Base64Decode(const std::string& src, std::vector<unsigned char>& out)
{
    size_t srcLen = src.size();
    if ((srcLen % 4 != 0) || srcLen == 0)
    {
        return false;
    }

    size_t destLen = (srcLen / 4) * 3;
    out.resize(destLen);

    //todo: ÅÐ¶Ï·µ»ØÖµ
    int ret = EVP_DecodeBlock((unsigned char*)out.data(), (const unsigned char*)src.c_str(), (int)src.size());
    if (ret == -1)
    {
        //base64 decode failed
        return false;
    }
    int i = 0;
    while (src.at(--srcLen) == '=')
    {
        ret--;
        if (++i > 2)
        {
            // input maybe not base64 str;
            return false;
        }
    }
    out.resize(ret);
    return true;
}