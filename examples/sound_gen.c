#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned char* data;
    size_t size;
} ByteStream;

/**
 * Reads a music.mp3 file and returns its byte stream
 * @param filename Path to the MP3 file
 * @return ByteStream structure containing the file data and size
 *         Returns NULL data and 0 size on error
 */
ByteStream read_mp3_file(const char* filename) {
    ByteStream stream = {NULL, 0};
    FILE* file = NULL;

    // Open file in binary mode
    file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Could not open file '%s'\n", filename);
        return stream;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size <= 0) {
        fprintf(stderr, "Error: Invalid file size\n");
        fclose(file);
        return stream;
    }

    // Allocate memory for the file data
    stream.data = (unsigned char*)malloc(file_size);
    if (!stream.data) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        return stream;
    }

    // Read file data
    size_t bytes_read = fread(stream.data, 1, file_size, file);
    if (bytes_read != (size_t)file_size) {
        fprintf(stderr, "Error: Could not read complete file\n");
        free(stream.data);
        stream.data = NULL;
        fclose(file);
        return stream;
    }

    stream.size = file_size;
    fclose(file);

    return stream;
}

/**
 * Frees the memory allocated for a ByteStream
 * @param stream Pointer to the ByteStream to free
 */
void free_byte_stream(ByteStream* stream) {
    if (stream && stream->data) {
        free(stream->data);
        stream->data = NULL;
        stream->size = 0;
    }
}

/**
 * Example usage of the read_mp3_file function
 */
int main() {
    const char* filename = "music.mp3";

    ByteStream mp3_stream = read_mp3_file(filename);

    if (mp3_stream.data) {
        printf("Successfully loaded MP3 file: %s\n", filename);
        printf("File size: %zu bytes\n", mp3_stream.size);

        // Example: Print first few bytes as hex
        printf("First 16 bytes (hex): ");
        for (int i = 0; i < 16 && i < (int)mp3_stream.size; i++) {
            printf("%02X ", mp3_stream.data[i]);
        }
        printf("\n");

        // Clean up memory
        free_byte_stream(&mp3_stream);
    } else {
        printf("Failed to load MP3 file: %s\n", filename);
        return 1;
    }

    return 0;
}
