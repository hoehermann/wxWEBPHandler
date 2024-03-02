/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/imagwebp.cpp
// Purpose:     wxImage WebP handler
// Author:      Hermann Höhne
// Copyright:   (c) Hermann Höhne
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

// based on code by Sylvain Bougnoux and Khral Steelforge (https://forums.wxwidgets.org/viewtopic.php?t=39212)

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_IMAGE && wxUSE_WEBP

#include "wx/imagwebp.h"
#include "webp/demux.h"
#include "webp/decode.h"
#include "webp/encode.h"
#include <vector>

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

//-----------------------------------------------------------------------------
// wxWEBPHandler
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxWEBPHandler, wxImageHandler);

#if wxUSE_STREAMS

#include <wx/mstream.h>
#include <memory>
#include <functional>

typedef std::unique_ptr<WebPDemuxer, std::function<void(WebPDemuxer*)>> WebPDemuxerPtr;

bool DecodeWebPDataIntoImage(wxImage *image, WebPData *webp_data, bool verbose) {
    WebPBitstreamFeatures features;
    VP8StatusCode status = WebPGetFeatures(webp_data->bytes, webp_data->size, &features);
    if (status != VP8_STATUS_OK) 
    {
        if (verbose) 
        {
           wxLogError("WebP: GetFeatures not OK.");
        }
        return false;
    }
    image->Create(features.width, features.height, false); // this allocates memory
    if (features.has_alpha) 
    {
        // image has alpha channel. needs to be decoded, then re-ordered.
        uint8_t * rgba = WebPDecodeRGBA(webp_data->bytes, webp_data->size, &features.width, &features.height);
        if (rgba == NULL) 
        {
            if (verbose) 
            {
               wxLogError("WebP: WebPDecodeRGBA failed.");
            }
            return false;
        }
        image->InitAlpha();
        unsigned char * rgb = image->GetData();
        unsigned char * alpha = image->GetAlpha();
        for (unsigned int index_pixel = 0; index_pixel < image->GetWidth() * image->GetHeight(); index_pixel++)
        {
            unsigned int index_rgba = index_pixel*4; // in RGBA, 1 pixel is 4 bytes
            unsigned int index_rgb = index_pixel*3; // in RGB, 1 pixel is 3 bytes
            unsigned int index_alpha = index_pixel; // alpha channel, 1 pixel is 1 byte
            rgb[index_rgb++] = rgba[index_rgba++]; // R
            rgb[index_rgb++] = rgba[index_rgba++]; // G
            rgb[index_rgb++] = rgba[index_rgba++]; // B
            alpha[index_alpha] = rgba[index_rgba]; // A
        }
        WebPFree(rgba);
    }
    else
    {
        // image has no alpha channel. decode into target buffer directly.
        int buffer_size = image->GetWidth() * image->GetHeight() * 3;
        int stride = image->GetWidth() * 3;
        uint8_t * output_buffer = WebPDecodeRGBInto(webp_data->bytes, webp_data->size, image->GetData(), buffer_size, stride);
        if (output_buffer == NULL)
        {
            if (verbose) 
            {
               wxLogError("WebP: WebPDecodeRGBInto failed.");
            }
            return false;
        }
    }
    image->SetMask(false); // all examples do this, so I do so as well
    return true;
}

bool DecodeWebPFrameIntoImage(wxImage *image, int index, WebPDemuxerPtr & demuxer, bool verbose) 
{
    bool ok = false;
    // apparently, index can be -1 for "don't care", but libwebp does care
    if (index < 0) 
    {
        index = 0;
    }
    WebPIterator iter;
    // wxImageHandler index starts from 0 (first frame)
    // WebPDemuxGetFrame to starts from 1 (0 means "last frame")
    if (WebPDemuxGetFrame(demuxer.get(), index+1, &iter)) 
    {
        ok = DecodeWebPDataIntoImage(image, &iter.fragment, verbose);
        WebPDemuxReleaseIterator(&iter);
    }
    return ok;
}

WebPDemuxerPtr CreateDemuxer(wxInputStream& stream, bool verbose = false) {
    wxMemoryOutputStream * mos = new wxMemoryOutputStream;
    stream.Read(*mos); // this reads the entire file into memory
    // TODO: only read data as needed since WebPDemux can operate on partial data. could save some bandwidth with e.g. DoGetImageCount
    wxStreamBuffer * mosb = mos->GetOutputStreamBuffer();
    WebPData * webp_data = new WebPData;
    webp_data->bytes = reinterpret_cast<uint8_t *>(mosb->GetBufferStart());
    webp_data->size = mosb->GetBufferSize();
    WebPDemuxerPtr demux
    (
        WebPDemux(webp_data), 
        [mos, webp_data](WebPDemuxer * demux) 
            {
                WebPDemuxDelete(demux);    
                delete webp_data;    
                delete mos; // delete the buffer after the WebPDemuxer is deleted
            }
    );
    if (demux == nullptr)
    {
        if (verbose)
        {
            wxLogError("WebP: WebPDemux failed.");
        }
    }
    return demux;
}

bool wxWEBPHandler::LoadFile(wxImage *image, wxInputStream& stream, bool verbose, int index)
{
    image->Destroy(); // all examples do this, so I do so as well
    bool ok = false;
    WebPDemuxerPtr demux = CreateDemuxer(stream, verbose);
    if (nullptr != demux) 
    {
        ok = DecodeWebPFrameIntoImage(image, index, demux, verbose);
    }
    return ok;
}

bool wxWEBPHandler::SaveFile(wxImage *image, wxOutputStream& stream, bool verbose)
{
    float quality_factor = 90;
    if (image->HasOption(wxIMAGE_OPTION_QUALITY))
    {
        quality_factor = image->GetOptionInt(wxIMAGE_OPTION_QUALITY);
    }
    size_t output_size = 0;
    uint8_t * output = NULL;
    unsigned char * rgb = image->GetData();
    if (image->HasAlpha())
    {
        unsigned char * alpha = image->GetAlpha();
        int stride = image->GetWidth() * 4; // stride is the "width" of a "line" in bytes
        std::vector<unsigned char> rgba(stride * image->GetHeight());
        for (unsigned int index_pixel = 0; index_pixel < image->GetWidth() * image->GetHeight(); index_pixel++)
        {
            unsigned int index_rgba = index_pixel*4; // in RGBA, 1 pixel is 4 bytes
            unsigned int index_rgb = index_pixel*3; // in RGB, 1 pixel is 3 bytes
            unsigned int index_alpha = index_pixel; // alpha channel, 1 pixel is 1 byte
            rgba[index_rgba++] = rgb[index_rgb++]; // R
            rgba[index_rgba++] = rgb[index_rgb++]; // G
            rgba[index_rgba++] = rgb[index_rgb++]; // B
            rgba[index_rgba] = alpha[index_alpha]; // A
        }
        output_size = WebPEncodeRGBA(rgba.data(), image->GetWidth(), image->GetHeight(), stride, quality_factor, &output);
    }
    else
    {
        int stride = image->GetWidth() * 3; // stride is the "width" of a "line" in bytes
        output_size = WebPEncodeRGB(rgb, image->GetWidth(), image->GetHeight(), stride, quality_factor, &output);
    }
    stream.WriteAll(output, output_size);
    WebPFree(output);
    return (output_size > 0 && stream.LastWrite() == output_size);
}

int wxWEBPHandler::DoGetImageCount(wxInputStream & stream) {
    int frame_count = -1;
    WebPDemuxerPtr demux = CreateDemuxer(stream);
    if (nullptr != demux)
    {
        frame_count = WebPDemuxGetI(demux.get(), WEBP_FF_FRAME_COUNT);
    }
    return frame_count;
}

bool wxWEBPHandler::DoCanRead(wxInputStream& stream)
{
    // check header according to https://developers.google.com/speed/webp/docs/riff_container
    const std::string riff = "RIFF";
    const std::string webp = "WEBP";
    const int buffer_size = 12;
    char buffer[buffer_size];
    // it's ok to modify the stream position here
    stream.Read(buffer, buffer_size);
    if (stream.LastRead() != buffer_size) 
    {
        return false;
    }
    return std::string(buffer, 4) == riff && std::string(&buffer[8], 4) == webp;
}

#endif // wxUSE_STREAMS

#endif // wxUSE_IMAGE && wxUSE_WEBP
