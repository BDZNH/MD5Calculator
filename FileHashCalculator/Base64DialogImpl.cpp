#include "Base64DialogImpl.h"
#include <wx/msgdlg.h>
#include "Utils.h"

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
    if (Utils::Base64Decode(toDecodeStr, ret))
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
    std::string result = Utils::Base64Encode((const unsigned char*)utf8str.data(), utf8str.length());
    if (!result.empty())
    {
        mTextCtrlResult->SetValue(result);
    }
}