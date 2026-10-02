#pragma once
#include <stdint.h>
#include <stddef.h>


/**
 * Initializes the global Zstd compression and decompression contexts.
 * Call this once at application startup.
 * @return 1 if initialization succeeded, 0 if it failed.
 */
int init_zstd(void);

/**
 * Compress source buffer into target buffer using the initialized context.
 * @param source_buffer      Pointer to raw data to compress.
 * @param source_size        Size of the raw input data in bytes.
 * @param target_buffer      Allocated memory destination for compressed data.
 * @param max_target_size    Allocated capacity of the target buffer.
 * @return Size of the compressed data generated, or 0 if compression failed.
 */
size_t zstd_compress(const uint8_t* source_buffer, size_t source_size, uint8_t* target_buffer, size_t max_target_size);

/**
 * Decompress source buffer into target buffer using the initialized context.
 * @param source_buffer      Pointer to the compressed data payload.
 * @param source_size        Size of the compressed data payload in bytes.
 * @param target_buffer      Allocated memory destination for uncompressed data.
 * @param max_target_size    Allocated capacity of the target buffer.
 * @return Size of the decompressed data generated, or 0 if decompression failed.
 */
size_t zstd_decompress(const uint8_t* source_buffer, size_t source_size, uint8_t* target_buffer, size_t max_target_size);

/**
 * Frees the global Zstd contexts and releases internal memory blocks back to the OS.
 * Call this once at application shutdown.
 */
void close_zstd(void);



