/**
 * aryyya, 2016
 */

#include "mime-types.h"
#include <strings.h>

/**
 * Get the Content-Type identifier string of a given file extension.
 * @param extension The file extension.
 * @return The Content-Type string.
 */
const char *
mime_get_content_type(const char *extension)
{
    // Web.
    if /**/ (strcasecmp(extension, "txt")  == 0)    return MIME_TEXT;
    else if (strcasecmp(extension, "html") == 0)    return MIME_HTML;
    else if (strcasecmp(extension, "css")  == 0)    return MIME_CSS;
    else if (strcasecmp(extension, "js")   == 0)    return MIME_JS;

    // Images.
    else if (strcasecmp(extension, "jpg")  == 0)    return MIME_JPEG;
    else if (strcasecmp(extension, "jpeg") == 0)    return MIME_JPEG;
    else if (strcasecmp(extension, "png")  == 0)    return MIME_PNG;
    else if (strcasecmp(extension, "gif")  == 0)    return MIME_GIF;
    else if (strcasecmp(extension, "bmp")  == 0)    return MIME_BMP;
    else if (strcasecmp(extension, "bm")   == 0)    return MIME_BMP;
    else if (strcasecmp(extension, "svg")  == 0)    return MIME_SVG;

    // Videos.
    else if (strcasecmp(extension, "mp4")  == 0)    return MIME_MP4;

    // Unrecognized.
    else return "";
}
