# wxWEBPHandler

wxWidgets wxImage handler for WebP reading image files.

Alpha channel is currently not supported.

## Usage

    class SampleApp : public wxApp
    {
        public:
            virtual bool OnInit() {
                wxImage::AddHandler(new wxWEBPHandler);
            }
    };
