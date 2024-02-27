#include "wx/imagwebp.h"
#include <wx/mstream.h>
#include "rgb_64x64_webpcolors.h"
#include "gray_64x64_gradient.h"
#include <iostream>
#include <wx/wfstream.h>

class wxWEBPHandlerTest : public wxWEBPHandler
{
private:
    void DoRoudtrip(wxImage & savingImage, wxImage & loadingImage)
    {
        wxMemoryOutputStream outputStream;
        savingImage.SetOption(wxIMAGE_OPTION_QUALITY, 100);
        SaveFile(&savingImage, outputStream, true);
        wxFileOutputStream outputFile("test.webp");
        SaveFile(&savingImage, outputFile, true);
        wxMemoryInputStream inputStream(outputStream);
        assert(DoCanRead(inputStream));
        LoadFile(&loadingImage, inputStream, true);
        assert(loadingImage.IsOk());
    }
    void AssertSimilarBytes(const unsigned char * input, const unsigned char * reference, size_t length, int difference_threshold)
    {
        for (size_t i = 0; i < length; i++) 
        {
            //std::cout << int(input[i] - reference[i]) << " ";
            assert(abs(input[i] - reference[i]) <= difference_threshold);
        }
        //std::cout << std::endl;
    }
public:
    wxWEBPHandlerTest() : wxWEBPHandler()
    {
    }
    void AssertDoCanReadTrueWhenOk()
    {
        std::cout << "AssertDoCanReadTrueWhenOk" <<  std::endl;
        std::string data("RIFF____WEBP____");
        wxMemoryInputStream stream(data.c_str(), data.size());
        assert(DoCanRead(stream));
    }
    void AssertDoCanReadFalseWhenShort()
    {
        std::cout << "AssertDoCanReadFalseWhenShort" <<  std::endl;
        std::string data("RIFF____WE");
        wxMemoryInputStream stream(data.c_str(), data.size());
        assert(!DoCanRead(stream));
    }
    void AssertDoCanReadFalseWhenWrong()
    {
        std::cout << "AssertDoCanReadFalseWhenWrong" <<  std::endl;
        std::string data("RIFF____WEBX____");
        wxMemoryInputStream stream(data.c_str(), data.size());
        assert(!DoCanRead(stream));
    }
    void AssertRGBRoundtrip() {
        std::cout << "AssertRGBRoundtrip" <<  std::endl;
        static const int side = 64;
        static const int difference_threshold = 5; // webp is a lossy format. allow some differences
        unsigned char * rgb_reference = rgb_64x64_webpcolors;
        bool static_data = true; // the data is static, wxImage shall not free it
        wxImage savingImage(side, side, rgb_reference, static_data);
        wxImage loadingImage;
        DoRoudtrip(savingImage, loadingImage);
        const unsigned char * rgb = loadingImage.GetData();
        AssertSimilarBytes(rgb, rgb_reference, side*side*3, difference_threshold);
    }
    void AssertRGBARoundtrip() {
        std::cout << "AssertRGBARoundtrip" <<  std::endl;
        static const int side = 64;
        static const int difference_threshold = 5; // webp is a lossy format. allow some differences
        unsigned char * rgb_reference = rgb_64x64_webpcolors;
        unsigned char * alpha_reference = gray_64x64_gradient;
        bool static_data = true; // the data is static, wxImage shall not free it
        wxImage savingImage(side, side, rgb_reference, alpha_reference, static_data);
        wxImage loadingImage;
        DoRoudtrip(savingImage, loadingImage);
        const unsigned char * rgb = loadingImage.GetData();
        AssertSimilarBytes(rgb, rgb_reference, side*side*3, difference_threshold);
        const unsigned char * alpha = loadingImage.GetAlpha();
        AssertSimilarBytes(alpha, alpha_reference, side*side*1, difference_threshold);
    }
    void AssertLoadAnimation() {
        // TODO
    }
};

int main(int, char**)
{
    wxWEBPHandlerTest test;
    //test.AssertDoCanReadTrueWhenOk();
    //test.AssertDoCanReadFalseWhenShort();
    //test.AssertDoCanReadFalseWhenWrong();
    //test.AssertRGBRoundtrip();
    test.AssertRGBARoundtrip();
    test.AssertLoadAnimation();
    std::cout << "All is well." << std::endl;
    return 0;
}
