#include "wx/imagwebp.h"
#include <wx/mstream.h>
#include "rgb_0.h"
#include <iostream>

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
        wxMemoryOutputStream outputStream;
        wxImage outputImage(16, 16, rgb_0, true);
        outputImage.SetOption(wxIMAGE_OPTION_QUALITY, 100);
        SaveFile(&outputImage, outputStream, true);
        
        wxImage inputImage;
        wxMemoryInputStream inputStream(outputStream);
        assert(DoCanRead(inputStream));
        LoadFile(&inputImage, inputStream, true);
        assert(inputImage.IsOk());
        
        unsigned char * rgb = inputImage.GetData();
        for (unsigned int i = 0; i < sizeof(rgb_0); i++) 
        {
            assert(abs(rgb_0[i] - rgb[i]) < 3);
        }
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
