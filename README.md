# wxWEBPHandler

wxWidgets wxImage handler for reading WebP image files.

Animations will only have their first frame shown.

## Usage

    class SampleApp : public wxApp
    {
        public:
            virtual bool OnInit() {
                wxImage::AddHandler(new wxWEBPHandler);
            }
    };
