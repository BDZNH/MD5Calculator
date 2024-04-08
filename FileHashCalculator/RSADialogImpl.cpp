#include "RSADialogImpl.h"
#include <wx/file.h>
#include <wx/msgdlg.h>
#include "RSACipher.h"
RSADialogImpl::RSADialogImpl(wxWindow* parent):RSADialog(parent)
{
}

void RSADialogImpl::OnFilePublicKeyChanged(wxFileDirPickerEvent& event)
{
    wxString file = event.GetPath();
    if (wxFile::Exists(file))
    {
        wxFile publicKey(file);
        if (publicKey.IsOpened())
        {
            publicKey.SeekEnd();
            auto fileSize = publicKey.Tell();
            publicKey.Seek(0);
            if (fileSize > 0)
            {
                mStringPublicKey.resize(fileSize);
                if (fileSize == publicKey.Read((void*)mStringPublicKey.data(), fileSize))
                {
                    //all be down
                }
                else
                {
                    mStringPublicKey.clear();
                    event.SetPath(wxEmptyString);
                    wxMessageBox(wxT("未能读取整个文件"), wxT("警告"), wxOK | wxICON_WARNING, this);
                }
            }
            else
            {
                event.SetPath(wxEmptyString);
                wxMessageBox(wxT("文件大小为 0"), wxT("警告"), wxOK | wxICON_WARNING, this);
            }
        }
        else
        {
            event.SetPath(wxEmptyString);
            wxMessageBox(wxT("未能读取文件"), wxT("警告"), wxOK | wxICON_WARNING, this);
        }
    }
}

void RSADialogImpl::OnFilePrivateKeyChanged(wxFileDirPickerEvent& event)
{
    wxString file = event.GetPath();
    if (wxFile::Exists(file))
    {
        wxFile privateKey(file);
        if (privateKey.IsOpened())
        {
            privateKey.SeekEnd();
            auto fileSize = privateKey.Tell();
            privateKey.Seek(0);
            if (fileSize > 0)
            {
                mStringPrivateKey.resize(fileSize);
                if (fileSize == privateKey.Read((void*)mStringPrivateKey.data(), fileSize))
                {
                    //all be down
                }
                else
                {
                    mStringPrivateKey.clear();
                    event.SetPath(wxEmptyString);
                    wxMessageBox(wxT("未能读取整个文件"), wxT("警告"), wxOK | wxICON_WARNING, this);
                }
            }
            else
            {
                event.SetPath(wxEmptyString);
                wxMessageBox(wxT("文件大小为 0"), wxT("警告"), wxOK | wxICON_WARNING, this);
            }
        }
        else
        {
            event.SetPath(wxEmptyString);
            wxMessageBox(wxT("未能读取文件"), wxT("警告"), wxOK | wxICON_WARNING, this);
        }
    }
}

void RSADialogImpl::OnButtonEncryptClicked(wxCommandEvent& event)
{
    DoEncrypt();
}

void RSADialogImpl::OnButtonDecryptClicked(wxCommandEvent& event)
{
    DoDecrypt();
}

void RSADialogImpl::DoEncrypt()
{
    if (mStringPublicKey.empty())
    {
        wxMessageBox(wxT("公钥无效"), wxT("警告"), wxOK | wxICON_WARNING, this);
        return;
    }
    wxString text = mTextCtrlOrigin->GetValue();
    if (text.IsEmpty())
    {
        return;
    }
    RSAPublic rsapublic(mStringPublicKey);
    auto utf8Text = text.ToUTF8();
    std::string ret;
    if (rsapublic.EncryptAsBase64((unsigned char*)utf8Text.data(), utf8Text.length(), ret) > 0)
    {
        mTextCtrlValue->SetValue(ret);
    }
    else
    {
        wxMessageBox(wxString::Format("加密时遇到错误: %s",rsapublic.GetLastError().c_str()), wxT("警告"), wxOK | wxICON_WARNING, this);
        mTextCtrlValue->SetValue(wxEmptyString);
    }
}

void RSADialogImpl::DoDecrypt()
{
    if (mStringPrivateKey.empty())
    {
        wxMessageBox(wxT("公钥无效"), wxT("警告"), wxOK | wxICON_WARNING, this);
        return;
    }
    wxString text = mTextCtrlOrigin->GetValue();
    if (text.IsEmpty())
    {
        return;
    }
    RSAPrivate rsaprivate(mStringPrivateKey);
    auto utf8Text = text.ToUTF8();
    std::string instr;
    instr.assign(utf8Text.data(), utf8Text.length());
    std::vector<uint8_t> out;
    if (rsaprivate.DecryptFromBase64(instr,out) > 0)
    {
        std::string retstr;
        retstr.assign((const char*)out.data(), out.size());
        mTextCtrlValue->SetValue(wxString::FromUTF8(retstr));
    }
    else
    {
        wxMessageBox(wxString::Format("解密时遇到错误: %s", rsaprivate.GetLastError().c_str()), wxT("警告"), wxOK | wxICON_WARNING, this);
        mTextCtrlValue->SetValue(wxEmptyString);
    }
}