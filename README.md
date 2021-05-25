# wxWEBPHandler

wxWidgets wxImage handler for reading WebP image files.

Saving image with alpha channel is currently not supported.

## Usage

    class SampleApp : public wxApp
    {
        public:
            virtual bool OnInit() {
                wxImage::AddHandler(new wxWEBPHandler);
            }
    };
