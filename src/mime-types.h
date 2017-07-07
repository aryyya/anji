/**
 * aryyya, 2016
 */

#ifndef mime_types_h
#define mime_types_h

// Web.
#define MIME_TEXT "text/plain"
#define MIME_HTML "text/html"
#define MIME_CSS  "text/css"
#define MIME_JS   "application/javascript"

// Images.
#define MIME_JPEG "image/jpeg"
#define MIME_PNG  "image/png"
#define MIME_GIF  "image/gif"
#define MIME_BMP  "image/bmp"
#define MIME_SVG  "image/svg+xml"

// Videos.
#define MIME_MP4  "video/mp4"

/**
 * Get the Content-Type identifier string of a given file extension.
 * @param extension The file extension.
 * @return The Content-Type string.
 */
const char *mime_get_content_type(const char *extension);

#endif /* mime_types_h */
