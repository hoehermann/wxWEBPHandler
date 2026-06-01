# wxWEBPHandler

wxWidgets wxImage handler for reading WebP image files.

**NOTE:** The code of this handler has been merged into wxWidgets. WebP support is included in wxWidgets v3.3.0.

Animations will only have their first frame shown.

## Usage

    class SampleApp : public wxApp
    {
        public:
            virtual bool OnInit() {
                wxImage::AddHandler(new wxWEBPHandler);
            }
    };
