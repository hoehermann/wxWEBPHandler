/////////////////////////////////////////////////////////////////////////////
// Name:        wx/imagwebp.h
// Purpose:     wxImage WebP handler
// Author:      Hermann Höhne
// Copyright:   (c) Hermann Höhne
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_IMAGWEBP_H_
#define _WX_IMAGWEBP_H_

#include "wx/image.h"

//-----------------------------------------------------------------------------
// wxWEBPHandler
//-----------------------------------------------------------------------------

#if wxUSE_LIBWEBP
class WXDLLIMPEXP_CORE wxWEBPHandler : public wxImageHandler
{
public:
    inline wxWEBPHandler()
    {
        m_name = wxT("WebP file");
        m_extension = wxT("webp");
        //m_type = wxBITMAP_TYPE_WEBP; type omitted for now
        m_mime = wxT("image/webp");
    }

#if wxUSE_STREAMS
    virtual bool LoadFile(wxImage *image, wxInputStream& stream, bool verbose=true, int index=-1) wxOVERRIDE;
    virtual bool SaveFile(wxImage *image, wxOutputStream& stream, bool verbose=true) wxOVERRIDE;
protected:
    virtual bool DoCanRead(wxInputStream& stream) wxOVERRIDE;
    virtual int DoGetImageCount(wxInputStream &stream) wxOVERRIDE;
#endif // wxUSE_STREAMS

private:
    wxDECLARE_DYNAMIC_CLASS(wxWEBPHandler);
};

#endif // wxUSE_LIBWEBP

#endif // _WX_IMAGWEBP_H_
