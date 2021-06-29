#include "wx/imagwebp.h"
#include <wx/mstream.h>
#include <iostream>

class wxWEBPHandlerTest : public wxWEBPHandler
{
public:
    wxWEBPHandlerTest() : wxWEBPHandler()
    {
    }
    bool DoCanReadTestOk()
    {
        std::string data("RIFF____WEBP____");
        wxMemoryInputStream stream(data.c_str(), data.size());
        return DoCanRead(stream);
    }
    bool DoCanReadTestShort()
    {
        std::string data("RIFF____WE");
        wxMemoryInputStream stream(data.c_str(), data.size());
        return !DoCanRead(stream);
    }
    bool DoCanReadTestWrong()
    {
        std::string data("RIFF____WEBX____");
        wxMemoryInputStream stream(data.c_str(), data.size());
        return !DoCanRead(stream);
    }
    bool DoCanReadPublic(wxInputStream& stream) {
        return DoCanRead(stream);
    }
    void AssertRGBRoundtrip() {
        // TODO: prepare reasonable test data
        const int width = 8;
        const int height = 8;
        const size_t testDataSize = width*height*3;
        unsigned char testData[testDataSize] = 
        {
            61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 
            61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 
            61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 
            61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 
            61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 
            61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 
            61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 
            61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61
        };
        wxImage outputImage(width, height, testData, true);
        wxMemoryOutputStream outputStream;
        test.SaveFile(&outputImage, outputStream, true);
        
        wxImage inputImage;
        wxMemoryInputStream inputStream(outputStream);
        assert(test.DoCanReadPublic(inputStream));
        test.LoadFile(&inputImage, inputStream, true);
        assert(inputImage.IsOk());
        
        unsigned char * rgb = inputImage.GetData();
        for (unsigned int i = 0; i < testDataSize; i++) 
        {
            if (testData[i] != rgb[i])
            {
                std::cout << "Expected " << int(testData[i]) << " got " << int(rgb[i]) << " at byte " << i << "." << std::endl;
                assert(testData[i] == rgb[i]);
            }
        }
    }
    void AssertRGBARoundtrip() {
        // TODO
    }
};

int main(int, char**)
{
    wxWEBPHandlerTest test;
    assert(test.DoCanReadTestOk());
    assert(test.DoCanReadTestShort());
    assert(test.DoCanReadTestWrong());
    test.AssertRGBRoundtrip();
    test.AssertRGBARoundtrip();
    std::cout << "All is well." << std::endl;
    return 0;
}
