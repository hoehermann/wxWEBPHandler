/////////////////////////////////////////////////////////////////////////////
// Name:        wx/imagwebp.hpp
// Purpose:     wxImage WebP handler
// Author:      Hermann Höhne
// Copyright:   (c) Hermann Höhne
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "wx/image.h"

//-----------------------------------------------------------------------------
// wxWEBPHandler
//-----------------------------------------------------------------------------

#if wxUSE_WEBP
class /*WXDLLIMPEXP_CORE*/ wxWEBPHandler : public wxImageHandler // NOTE: WXDLLIMPEXP_CORE exists in reference, but I do not know how to use it correctly
{
public:
    inline wxWEBPHandler()
    {
        m_name = wxT("WebP file");
        m_extension = wxT("webp");
        //m_type = wxBITMAP_TYPE_INVALID; // no idea what to choose here
        m_mime = wxT("image/webp");
    }

#if wxUSE_STREAMS
    virtual bool LoadFile(wxImage *image, wxInputStream& stream, bool verbose=true, int index=-1) /*wxOVERRIDE*/;
    virtual bool SaveFile(wxImage *image, wxOutputStream& stream, bool verbose=true) /*wxOVERRIDE*/;
protected:
    virtual bool DoCanRead(wxInputStream& stream) /*wxOVERRIDE*/;
    // NOTE: wxOVERRIDE exists in reference, but I do not know how to use it correctly
#endif

private:
    wxDECLARE_DYNAMIC_CLASS(wxWEBPHandler);
};
#else
    #error "Trying to include wxWEBPHandler without wxUSE_WEBP."
#endif
