#include "QRCodeDialogImpl.h"
#define SVPNG_OUTPUT std::vector<std::uint8_t>& _buffer
#define SVPNG_PUT(u) _buffer.push_back(u)
#include "svpng.h"
#include <qrencode.h>
#include <wx/mstream.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/wfstream.h>

struct Pixel
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

static
bool RealGenerateQrCode(int size,const unsigned char* data, QRecLevel level, int version, int casesensitive, int padding, int expectWidth, std::vector<std::uint8_t>& buffer)
{
    if (!data)
    {
        return false;
    }
    if (version < 0 || version > QRSPEC_VERSION_MAX)
    {
        version = 0;
    }
    QRcode* qrcode = QRcode_encodeData(size, data, version, level);
    if (!qrcode)
    {
        return false;
    }
    const int width = qrcode->width;
    int outWidth = ((expectWidth-(padding*2)) / width) * width;
    int ratio = outWidth / width;
    if (padding < 0 || padding>99)
    {
        padding = 30;
    }
    outWidth += (padding * 2);

    std::vector<Pixel> outRgb;

    outRgb.resize(outWidth * outWidth);
    for (int i = 0; i < outWidth; i++)
    {
        for (int j = 0; j < outWidth; j++)
        {
            Pixel& pixel = outRgb[i * outWidth + j];
            if (padding <= i && i < outWidth - padding && padding <= j && j < outWidth - padding)
            {
                int tmpi = (i - padding) / ratio;
                int tmpj = (j - padding) / ratio;
                int index = tmpi * width + tmpj;
                if (qrcode->data[tmpi * width + tmpj] & 0x01)
                {
                    pixel.r = 0x00;
                    pixel.g = 0x00;
                    pixel.b = 0x00;
                }
                else
                {
                    pixel.r = 0xFF;
                    pixel.g = 0xFF;
                    pixel.b = 0xFF;
                }
            }
            else
            {
                pixel.r = 0xFF;
                pixel.g = 0xFF;
                pixel.b = 0xFF;
            }
        }
    }

    buffer.clear();
    svpng(buffer, outWidth, outWidth, (const unsigned char*)outRgb.data(), 0);
    QRcode_free(qrcode);
    return true;
}

QRCodeDialogImpl::QRCodeDialogImpl(wxWindow* parent):QRCodeDialog(parent)
{
    SetIcon(wxICON(qrcodeIcon));
    mTextCtrlQRCodeText->SetHint(wxT("将想要生成二维码的文本输入到这里之后回车"));
    mTextCtrlQRCodeText->SetMaxLength(1000);
    mCheckBoxScale->SetValue(false);
    mChoiceQRecLevel->Select(3);
}

QRCodeDialogImpl::~QRCodeDialogImpl()
{
    mRealPngData.clear();
}

wxString QRCodeDialogImpl::GetVersion()
{
    wxString ret;
    ret.Append("libqrencode https://github.com/fukuchi/libqrencode");
    ret.Append("\r\n");
    ret.Append("svpng 0.1.1 http://github.com/miloyip/svpng");
    return ret;
}

void QRCodeDialogImpl::OnQrCodeTextEntered(wxCommandEvent& event)
{
    GenerateQrCode();
}
void QRCodeDialogImpl::OnChoiceQrcodeLevel(wxCommandEvent& event)
{
    GenerateQrCode();
}
void QRCodeDialogImpl::OnChoiceQREncodeVersion(wxCommandEvent& event)
{
    GenerateQrCode();
}
void QRCodeDialogImpl::OnSpinCtrlPaddingChanged(wxSpinEvent& event)
{
    GenerateQrCode();
}
void QRCodeDialogImpl::OnSpinCtrlPaddingEnter(wxCommandEvent& event)
{
    GenerateQrCode();
}
void QRCodeDialogImpl::OnCheckBoxScale(wxCommandEvent& event)
{
    GenerateQrCode();
}
void QRCodeDialogImpl::OnButtonSaveOringImageClicked(wxCommandEvent& event)
{
    if (mRealPngData.size() == 0)
    {
        wxMessageBox(wxT("还没有生成二维码"), wxEmptyString, wxOK | wxICON_ERROR, this);
        return;
    }
    wxFileDialog dialog(this, wxT("保存二维码"), wxEmptyString, wxT("qrcode.png"), wxT("图像文件 (*.png) | *.png"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (dialog.ShowModal() == wxID_CANCEL)
    {
        return;
    }
    wxFileOutputStream output(dialog.GetPath());
    if (!output.IsOk())
    {
        wxMessageBox(wxT("打开文件失败"), wxEmptyString, wxOK | wxICON_ERROR, this);
        return;
    }
    output.Write(mRealPngData.data(), mRealPngData.size());
    output.Sync();
    output.Close();
}

void QRCodeDialogImpl::GenerateQrCode()
{
    const wxString text = mTextCtrlQRCodeText->GetValue();
    if (text.empty())
    {
        return;
    }
    QRecLevel level = (QRecLevel)mChoiceQRecLevel->GetSelection();
    int ver = mChoiceQREncodeVersion->GetSelection();
    auto utf8toEncodeText = text.ToUTF8();
    auto len = utf8toEncodeText.length();
    bool scale = mCheckBoxScale->GetValue();
    int expectSize = mStaticBitMapQrCode->GetSize().GetWidth(); //scale ? mStaticBitMapQrCode->GetSize().GetWidth() : 0;
    if (RealGenerateQrCode(len,(const unsigned char*)utf8toEncodeText.data(), QR_ECLEVEL_H, QR_MODE_KANJI, 1,
        mSpinCtrlPadding->GetValue(), expectSize, mRealPngData))
    {
        UpdateBitmapIfNeed(scale);
    }
    else
    {
        wxMessageBox(wxT("编码失败"), wxEmptyString, wxOK | wxICON_ERROR, this);
    }
}

void QRCodeDialogImpl::UpdateBitmapIfNeed(bool scale)
{
    if (mRealPngData.size() > 0)
    {
        wxMemoryInputStream pngStream(mRealPngData.data(), mRealPngData.size());
        wxImage image;
        image.LoadFile(pngStream, wxBITMAP_TYPE_PNG);
        if (scale)
        {
            auto size = mStaticBitMapQrCode->GetSize();
            mStaticBitMapQrCode->SetBitmap(wxBitmap(image.Scale(size.GetWidth(), size.GetHeight(), wxIMAGE_QUALITY_HIGH)));
        }
        else
        {
            mStaticBitMapQrCode->SetBitmap(wxBitmap(image));
        }
    }
}
