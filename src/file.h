/**
 * aryyya, 2016
 */

#ifndef file_h
#define file_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Configuration constants.
#define FILE_READ_MODE          "r"
#define FILE_WRITE_MODE         "w"
#define FILE_READ_BINARY_MODE   "rb"
#define FILE_WRITE_BINARY_MODE  "wb"
#define FILE_TYPE_TEXT          0
#define FILE_TYPE_BINARY        1
#define FILE_PATH_SIZE          256

// Return codes.
#define FILE_SUCCESS 0
#define FILE_FAILURE 1

/**
 * An object that holds the contents of a file on disk in memory.
 */
typedef struct File {

    // The system path to the file.
    char path[FILE_PATH_SIZE + 1];

    // The contents of the file in memory.
    char *data;

    // The size of the contents of the file.
    size_t size;

} File;

/**
 * Create a new File.
 * @param file The file.
 * @param path The file path.
 * @param mode The file stream mode.
 * @return FILE_SUCCESS on success, FILE_FAILURE on failure.
 * On success, the returned file object must eventually be destroyed with file_free().
 */
int file_new(File *file, const char *path, const char *mode);

/**
 * Destroy a File.
 * @param file The file.
 */
void file_free(File *file);

/**
 * Get the size property of a file.
 * @param file The file.
 * @return The size in bytes.
 */
size_t file_get_size(const File *file);

/**
 * Get the data property of a file.
 * @param file The file.
 * @return A pointer to the data.
 */
const char *file_get_data(const File *file);

/**
 * Dump the state of a File to the console.
 * @param file The file.
 * @param type The type of the file, either FILE_TYPE_TEXT or FILE_TYPE_BINARY.
 */
void file_dump(const File *file, const int type);

/**
 * Get the extension of a path.
 * @param path The path.
 * @param buffer The buffer to put the extension in.
 */
void file_get_extn(const char *path, char *buffer);

/**
 * Get the 'fixed' version of a path (absolute to relative, ".." sequences removed.
 * @param path The path.
 * @param buffer The buffer to put the fixed version in.
 */
void file_fix_path(const char *path, char *buffer);

#endif /* file_h */
