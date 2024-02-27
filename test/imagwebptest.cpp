#include "wx/imagwebp.h"
#include <wx/mstream.h>
#include "rgb_64x64_webpcolors.h"
//#include <iostream>
//#include <wx/wfstream.h>

class wxWEBPHandlerTest : public wxWEBPHandler
{
public:
    wxWEBPHandlerTest() : wxWEBPHandler()
    {
    }
    void AssertDoCanReadTrueWhenOk()
    {
        std::string data("RIFF____WEBP____");
        wxMemoryInputStream stream(data.c_str(), data.size());
        assert(DoCanRead(stream));
    }
    void AssertDoCanReadFalseWhenShort()
    {
        std::string data("RIFF____WE");
        wxMemoryInputStream stream(data.c_str(), data.size());
        assert(!DoCanRead(stream));
    }
    void AssertDoCanReadFalseWhenWrong()
    {
        std::string data("RIFF____WEBX____");
        wxMemoryInputStream stream(data.c_str(), data.size());
        assert(!DoCanRead(stream));
    }
    void AssertRGBRoundtrip() {
        static const int side = 64;
        static const int bytes_per_pixel = 3;
        static const int difference_threshold = 5; // webp is a lossy format. allow some differences
        unsigned char * reference = rgb_64x64_webpcolors;
        bool static_data = true; // the data is static, wxImage shall not free it
        wxImage outputImage(side, side, reference, static_data);

        wxMemoryOutputStream outputStream;
        outputImage.SetOption(wxIMAGE_OPTION_QUALITY, 100);
        SaveFile(&outputImage, outputStream, true);
        
        //wxFileOutputStream outputFile("test.webp");
        //SaveFile(&outputImage, outputFile, true);

        wxMemoryInputStream inputStream(outputStream);
        assert(DoCanRead(inputStream));
        wxImage inputImage;
        LoadFile(&inputImage, inputStream, true);
        assert(inputImage.IsOk());
        
        const unsigned char * rgb = inputImage.GetData();
        for (unsigned int i = 0; i < side*side*bytes_per_pixel; i++) 
        {
            //std::cout << int(reference[i] - rgb[i]) << " ";
            assert(abs(reference[i] - rgb[i]) <= difference_threshold);
        }
        //std::cout << std::endl;
    }
    void AssertRGBARoundtrip() {
        // TODO
    }
    void AssertLoadAnimation() {
        // TODO
    }
};

int main(int, char**)
{
    wxWEBPHandlerTest test;
    test.AssertDoCanReadTrueWhenOk();
    test.AssertDoCanReadFalseWhenShort();
    test.AssertDoCanReadFalseWhenWrong();
    test.AssertRGBRoundtrip();
    test.AssertRGBARoundtrip();
    test.AssertLoadAnimation();
    std::cout << "All is well." << std::endl;
    return 0;
}
