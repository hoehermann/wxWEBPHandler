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

#include "wx/imagwebp.hpp"
#include "webp/decode.h"

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


bool wxWEBPHandler::LoadFile(wxImage *image, wxInputStream& stream, bool verbose, int WXUNUSED(index)) {
    image->Destroy(); // all examples do this, so I do so as well
    // TODO: do not read entire file into memory
    wxMemoryOutputStream mos;
    stream.Read(mos);
    wxStreamBuffer * mosb = mos.GetOutputStreamBuffer();
    const uint8_t * data = reinterpret_cast<uint8_t *>(mosb->GetBufferStart());
    size_t data_size = mosb->GetBufferSize();
    WebPBitstreamFeatures features;
    VP8StatusCode status = WebPGetFeatures(data, data_size, &features);
    if (status != VP8_STATUS_OK) {
        if (verbose) {
           wxLogError("WebP: GetFeatures not OK.");
        }
        return false;
    }
    if (features.has_alpha) {
        wxLogWarning("WebP: Alpha channel not implemented.");
        // TODO: create image first, then write interleaved alpha channel directly into image using
        // unsigned char * rgb = image->GetData() and * a = image->GetAlpha()
    }
    uint8_t * rgb = WebPDecodeRGB(data, data_size, &features.width, &features.height);
    bool static_data = false; // will call free() on rgb as needed. hopefully it is compatible with WebPFree
    image->Create(features.width, features.height, rgb, static_data);
    image->SetMask(false);
    return true;
}

bool wxWEBPHandler::SaveFile(wxImage *image, wxOutputStream& stream, bool WXUNUSED(verbose)) {
    // not implemented
    return false;
}

bool wxWEBPHandler::DoCanRead(wxInputStream& stream)
{
    // it's ok to modify the stream position here
    // blunt approach: read entire stream into memory, try to get info via library
    /*
    wxMemoryOutputStream mos;
    stream.Read(mos);
    wxStreamBuffer * mosb = mos.GetOutputStreamBuffer();
    const uint8_t * data = reinterpret_cast<uint8_t *>(mosb->GetBufferStart());
    size_t data_size = mosb->GetBufferSize();
    return WebPGetInfo(data, data_size, nullptr, nullptr);
    */
    // manual approach: check header according to https://developers.google.com/speed/webp/docs/riff_container
    // might be less reliable, but much faster
    const std::string riff = "RIFF";
    const std::string webp = "WEBP";
    const int buffer_size = 12;
    char buffer[buffer_size];
    stream.Read(buffer, buffer_size);
    if (stream.LastRead() != buffer_size) {
        return false;
    }
    return std::string(buffer, 4) == riff && std::string(&buffer[8], 4) == webp;
}

#endif // wxUSE_STREAMS

#endif // wxUSE_IMAGE && wxUSE_WEBP

