#include "wx/imagwebp.h"
#include <wx/mstream.h>
#include "rgb_64x64_webpcolors.h"
#include "gray_64x64_gradient.h"
#include <iostream>
#include <wx/wfstream.h>

class wxWEBPHandlerTest : public wxWEBPHandler
{
private:
    std::string outputFileName;
    bool DoRoudtrip(wxImage & savingImage, wxImage & loadingImage)
    {
        wxMemoryOutputStream outputStream;
        savingImage.SetOption(wxIMAGE_OPTION_QUALITY, 100);
        SaveFile(&savingImage, outputStream, true);
        if (!outputFileName.empty()) {
            wxFileOutputStream outputFile(outputFileName);
            SaveFile(&savingImage, outputFile, true);
        }
        wxMemoryInputStream inputStream(outputStream);
        wxCHECK_MSG(DoCanRead(inputStream), false, "DoCanRead returned false on generated data.");
        inputStream.SeekI(0);
        // the fact that the above reset is neccessary contradicts the comment "it's ok to modify the stream position here" in DoCanRead
        // TODO: discuss
        LoadFile(&loadingImage, inputStream, true);
        wxCHECK_MSG(loadingImage.IsOk(), false, "Loading image failed.");
        return true;
    }
    bool AssertSimilarBytes(const unsigned char * input, const unsigned char * reference, size_t length, int difference_threshold)
    {
        for (size_t i = 0; i < length; i++) 
        {
            //std::cout << int(input[i] - reference[i]) << " ";
            wxCHECK_MSG(abs(input[i] - reference[i]) <= difference_threshold, false, "Image bytes differ too much.");
        }
        //std::cout << std::endl;
        return true;
    }
public:
    wxWEBPHandlerTest(const std::string & outputFileName = "") : wxWEBPHandler(), outputFileName(outputFileName)
    {
    }
    bool AssertDoCanReadTrueWhenOk()
    {
        std::cout << "AssertDoCanReadTrueWhenOk" <<  std::endl;
        std::string data("RIFF____WEBP____");
        wxMemoryInputStream stream(data.c_str(), data.size());
        wxCHECK_MSG(DoCanRead(stream), false, "DoCanRead returned false on valid data.");
        return true;
    }
    bool AssertDoCanReadFalseWhenShort()
    {
        std::cout << "AssertDoCanReadFalseWhenShort" <<  std::endl;
        std::string data("RIFF____WE");
        wxMemoryInputStream stream(data.c_str(), data.size());
        wxCHECK_MSG(!DoCanRead(stream), false, "DoCanRead returned true on short data.");
        return true;
    }
    bool AssertDoCanReadFalseWhenWrong()
    {
        std::cout << "AssertDoCanReadFalseWhenWrong" <<  std::endl;
        std::string data("RIFF____WEBX____");
        wxMemoryInputStream stream(data.c_str(), data.size());
        wxCHECK_MSG(!DoCanRead(stream), false, "DoCanRead returned true on invalid data.");
        return true;
    }
    bool AssertRGBRoundtrip() {
        std::cout << "AssertRGBRoundtrip" <<  std::endl;
        static const int side = 64;
        unsigned char * rgb_reference = rgb_64x64_webpcolors;
        bool static_data = true; // the data is static, wxImage shall not free it
        wxImage savingImage(side, side, rgb_reference, static_data);
        wxImage loadingImage;
        wxCHECK_MSG(DoRoudtrip(savingImage, loadingImage), false, "Roudtrip failed.");

        static const int rgb_difference_threshold = 5; // webp is a lossy format. allow some differences
        const unsigned char * rgb = loadingImage.GetData();
        wxCHECK_MSG(AssertSimilarBytes(rgb, rgb_reference, side*side*3, rgb_difference_threshold), false, "AssertSimilarBytes failed on RGB data.");
        return true;
    }
    bool AssertRGBARoundtrip() {
        std::cout << "AssertRGBARoundtrip" <<  std::endl;
        static const int side = 64;
        unsigned char * rgb_reference = rgb_64x64_webpcolors;
        unsigned char * alpha_reference = gray_64x64_gradient;
        bool static_data = true; // the data is static, wxImage shall not free it
        wxImage savingImage(side, side, rgb_reference, alpha_reference, static_data);
        wxImage loadingImage;
        wxCHECK_MSG(DoRoudtrip(savingImage, loadingImage), false, "Roudtrip failed.");

        // webp is a lossy format. allow some differences
        static const int alpha_difference_threshold = 5;
        const unsigned char * alpha = loadingImage.GetAlpha();
        wxCHECK_MSG(AssertSimilarBytes(alpha, alpha_reference, side*side*1, alpha_difference_threshold), false, "AssertSimilarBytes failed on RGB data.");

        // when an alpha channel is involved, rgb seems to be even more lossy
        static const int rgb_difference_threshold = 30;
        const unsigned char * rgb = loadingImage.GetData();
        wxCHECK_MSG(AssertSimilarBytes(rgb, rgb_reference, side*side*3, rgb_difference_threshold), false, "AssertSimilarBytes failed on RGB data.");
        return true;
    }
    bool AssertLoadAnimation() {
        // TODO
        return true;
    }
};

int main(int, char**)
{
    bool result = true;
    wxWEBPHandlerTest test;
    result &= test.AssertDoCanReadTrueWhenOk();
    result &= test.AssertDoCanReadFalseWhenShort();
    result &= test.AssertDoCanReadFalseWhenWrong();
    result &= test.AssertRGBRoundtrip();
    result &= test.AssertRGBARoundtrip();
    result &= test.AssertLoadAnimation();
    if (result) {
        std::cout << "All is well." << std::endl;
    } else {
        std::cout << "Issue detected." << std::endl;
    }
    return !result;
}
