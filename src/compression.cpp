#include "compression.h"

#include <stdexcept>

extern "C"{
    #include "lz4.h"
    #include "bitshuffle.h"
}

using namespace std;

#define is_little_endian htonl(1) != 1

size_t bsread::get_compression_buffer_size(compression_type compression, size_t n_elements, size_t element_size) {
    switch (compression) {

        case compression_lz4: {
            size_t n_bytes = n_elements * element_size;
            return static_cast<size_t>(LZ4_compressBound(n_bytes)) + 4;
        }

        case compression_bslz4:
            return bshuf_compress_lz4_bound(n_elements, element_size, 0) + 12;

        default:
            throw runtime_error("Cannot determine compression buffer size for unknown compression type.");
    }
}

size_t bsread::compress_buffer(compression_type compression, const char* data, size_t n_elements, size_t element_size,
                               char* buffer, size_t buffer_size) {
    switch (compression) {

        case compression_lz4: {
            return compress_lz4(data, n_elements, element_size, buffer, buffer_size);
        }

        case compression_bslz4:
            return compress_bitshuffle(data, n_elements, element_size, buffer);

        default:
            throw runtime_error("Cannot compress with unknown compression type.");
    }
}

size_t bsread::compress_lz4(const char* data, size_t n_elements, size_t element_size,
                            char* buffer, size_t buffer_size){


    size_t data_len = n_elements * element_size;

    // The bytes should be in big endian (network order).
    if(is_little_endian){
        ((uint32_t*)buffer)[0] = htonl(data_len);
    } else {
        ((uint32_t*)buffer)[0] = data_len;
    }

    size_t compressed_size = LZ4_compress_default(data, &buffer[4], data_len, buffer_size-4);

    if(!compressed_size) throw runtime_error("Error while compressing [LZ4] channel:");
    return compressed_size+4;

}

size_t bsread::decompress_lz4(const char* compressed_data, size_t compressed_size, char* data) {

    uint32_t expected_data_len;

    if(is_little_endian){
        expected_data_len = ntohl(((uint32_t*)compressed_data)[0]);
    } else {
        expected_data_len = ((uint32_t*)compressed_data)[0];
    }

    int decompressed_len = LZ4_decompress_safe(&compressed_data[4], data, compressed_size-4, expected_data_len);

    if (expected_data_len != decompressed_len) throw runtime_error("Expected and decompressed data len do not match.");

    // If the value is not positive, we throw an exception anyway.
    return (size_t)decompressed_len;
}

size_t bsread::compress_bitshuffle(const char* data, size_t n_elements, size_t element_size, char* buffer){

    size_t compressed_size;
    size_t block_size = bshuf_default_block_size(element_size);


    uint64_t uncompressed_data_len = (uint64_t) n_elements * element_size;

    // The system is little endian, convert the 64bit value to big endian (network order).
    if (is_little_endian) {
        uint32_t high_bytes = htonl((uint32_t)(uncompressed_data_len >> 32));
        uint32_t low_bytes = htonl((uint32_t)(uncompressed_data_len & 0xFFFFFFFFLL));
        uncompressed_data_len = (((uint64_t)low_bytes) << 32) | high_bytes;
    }

    ((int64_t*)buffer)[0] = uncompressed_data_len;

    // The block size has to be multiplied by the elm_size before inserting it into the binary header.
    // https://github.com/kiyo-masui/bitshuffle/blob/04e58bd553304ec26e222654f1d9b6ff64e97d10/src/bshuf_h5filter.c#L167
    uint32_t header_block_size = (uint32_t) block_size * element_size;

    ((int32_t*)buffer)[2] = htonl(header_block_size);

    compressed_size = bshuf_compress_lz4(data, &buffer[12], n_elements, element_size, block_size);

    if(!compressed_size) throw runtime_error("Error while compressing [LZ4] channel:");
    return compressed_size+12;
}