/**
 * aryyya, 2016
 */

#include "file.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Private:

// Struct initializer.
static File zero = { 0 };

// Public:

/**
 * Create a new File.
 * @param file The file.
 * @param path The file path.
 * @param mode The file stream mode.
 * @return FILE_SUCCESS on success, FILE_FAILURE on failure.
 * On success, the returned file object must eventually be destroyed with file_free().
 */
int
file_new(File *file, const char *path, const char *mode)
{
    *file = zero;

    FILE *file_stream = NULL;
    size_t objects_read = 0;

    // Save file path name.
    strncpy(file->path, path, FILE_PATH_SIZE + 1);

    // Open file as a stream.
    file_stream = fopen(path, mode);
    if (file_stream == NULL) goto error;

    // Determine file size.
    fseek(file_stream, 0, SEEK_END);
    file->size = ftell(file_stream);
    rewind(file_stream);

    // Allocate enough memory to hold file contents.
    file->data = malloc(sizeof (char) * file->size);
    if (file->data == NULL) goto error;

    // Read file contents into memory.
    objects_read = fread(file->data, file->size, 1, file_stream);
    if (objects_read != 1) goto error;

    // Close file stream.
    fclose(file_stream);

    return FILE_SUCCESS;

error:
    if (file_stream) fclose(file_stream);
    if (file->data)  free(file->data);
    return FILE_FAILURE;
}

/**
 * Destroy a File.
 * @param file The file.
 */
void
file_free(File *file)
{
    if (file->data) free(file->data);
}

/**
 * Get the size property of a file.
 * @param file The file.
 * @return The size in bytes.
 */
size_t
file_get_size(const File *file)
{
    return file->size;
}

/**
 * Get the data property of a file.
 * @param file The file.
 * @return A pointer to the data.
 */
const char *
file_get_data(const File *file)
{
    return file->data;
}

/**
 * Dump the state of a File to the console.
 * @param file The file.
 * @param type The type of the file, either FILE_TYPE_TEXT or FILE_TYPE_BINARY.
 */
void
file_dump(const File *file, const int type)
{
    fprintf(stdout, "File->path: %s\n", file->path);
    fprintf(stdout, "File->type: %s\n", type == FILE_TYPE_TEXT ? "FILE_TYPE_TEXT" : "FILE_TYPE_BINARY");
    fprintf(stdout, "File->size: %lu\n", file->size);
    fprintf(stdout, "File->data:\n");
    switch (type) {

        // Display in plain ASCII for text files.
        case FILE_TYPE_TEXT:
            for (size_t i = 0; i < file->size; i++) {
                fputc(file->data[i], stdout);
            }
            break;

        // Display each byte in 2 digit hexadecimal numbers for binary files.
        case FILE_TYPE_BINARY:
            for (size_t i = 0; i < file->size; i++) {
                fprintf(stdout, "%.2x ", (unsigned char) file->data[i]);
            }
            break;
    }
    fputc('\n', stdout);
}

/**
 * Get the extension of a path.
 * @param path The path.
 * @param buffer The buffer to put the extension in.
 */
void
file_get_extn(const char *path, char *buffer)
{
    const char *ext = strrchr(path, '.');
    strncpy(buffer, (ext == NULL || ext == path) ? "\0" : ext + 1, FILE_PATH_SIZE);
}

/**
 * Get the 'fixed' version of a path (absolute to relative, ".." sequences removed.
 * @param path The path.
 * @param buffer The buffer to put the fixed version in.
 */
void
file_fix_path(const char *path, char *buffer)
{
    if (strcmp(path, "/") == 0) {
        strncpy(buffer, "index.html", FILE_PATH_SIZE);
    } else if (path[0] == '/' && strlen(path) > 1) {
        strncpy(buffer, path + 1, FILE_PATH_SIZE);
    }
}
