#include "Base64DialogImpl.h"
#include <openssl/evp.h>
#include <wx/msgdlg.h>
std::string Base64Encode(const unsigned char* data, size_t size)
{
    size_t base64_len = (size + 2) / 3 * 4;
    if (base64_len == 0)
    {
        return "";
    }
    std::string ret;
    ret.resize(base64_len);
    EVP_EncodeBlock((unsigned char*)ret.data(), data, size);
    return std::move(ret);
}

bool Base64Decode(const std::string& src, std::vector<unsigned char>& out)
{
    size_t srcLen = src.size();
    if (srcLen % 4 != 0)
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


Base64DialogImpl::Base64DialogImpl(wxWindow* parent):Base64Dialog(parent)
{
    SetIcon(wxICON(base64Icon));
}

Base64DialogImpl::~Base64DialogImpl()
{

}

void Base64DialogImpl::OnButtonBase64DecodeClicked(wxCommandEvent& event)
{
    auto str = mTextCtrlOrigin->GetValue();
    auto utf8str = str.ToUTF8();
    std::string toDecodeStr(utf8str.data(), utf8str.length());
    std::vector<unsigned char> ret;
    if (Base64Decode(toDecodeStr, ret))
    {
        wxString decodedText = wxString::FromUTF8(std::string((const char*)ret.data(), ret.size()));
        mTextCtrlResult->SetValue(decodedText);
    }
    else
    {
        wxMessageBox(wxT("½âÂëÊ§°Ü£¬Çë¼ì²éÊäÈë×Ö·û´®ÊÇ·ñÊÇÓÐÐ§µÄBase64×Ö·û´®£¡"), wxT("¾¯¸æ"), wxOK | wxICON_WARNING);
    }
}
void Base64DialogImpl::OnButtonBase64EncodeClicked(wxCommandEvent& event)
{
    auto str = mTextCtrlOrigin->GetValue();
    auto utf8str = str.ToUTF8();
    std::string result = Base64Encode((const unsigned char*)utf8str.data(), utf8str.length());
    if (!result.empty())
    {
        mTextCtrlResult->SetValue(result);
    }
}