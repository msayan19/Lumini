#include "wrappers.h"
#include <zstd.h>

// Global or file-scoped persistent contexts
static ZSTD_CCtx* g_cctx = NULL;
static ZSTD_DCtx* g_dctx = NULL;

int init_zstd(void) {
    if (!g_cctx) {
        g_cctx = ZSTD_createCCtx();
    }
    if (!g_dctx) {
        g_dctx = ZSTD_createDCtx();
    }
    
    if (!g_cctx || !g_dctx) {
        if (g_cctx) { ZSTD_freeCCtx(g_cctx); g_cctx = NULL; }
        if (g_dctx) { ZSTD_freeDCtx(g_dctx); g_dctx = NULL; }
        return 0;
    }
    return 1;
}

size_t zstd_compress(const uint8_t* source_buffer, size_t source_size, uint8_t* target_buffer, size_t max_target_size) {
    if (!g_cctx || !source_buffer || !target_buffer || max_target_size == 0) {
        return 0;
    }

    // Reset session parameters cleanly without dropping internal memory blocks
    ZSTD_CCtx_reset(g_cctx, ZSTD_reset_session_only);

    // Apply performance tweaks for large datasets (up to 4GB)
    ZSTD_CCtx_setParameter(g_cctx, ZSTD_c_compressionLevel, 3);
    ZSTD_CCtx_setParameter(g_cctx, ZSTD_c_nbWorkers, 4); // Parallel multi-threaded encoding
    ZSTD_CCtx_setParameter(g_cctx, ZSTD_c_enableLongDistanceMatching, 1);
    ZSTD_CCtx_setParameter(g_cctx, ZSTD_c_windowLog, 27); // 128MB matching window

    ZSTD_inBuffer input = { source_buffer, source_size, 0 };
    ZSTD_outBuffer output = { target_buffer, max_target_size, 0 };

    // Process and flush everything synchronously into the destination buffer
    size_t const result = ZSTD_compressStream2(g_cctx, &output, &input, ZSTD_e_end);

    if (ZSTD_isError(result) || result > 0) {
        return 0; // Overrun or compression failure
    }

    return output.pos;
}

size_t zstd_decompress(const uint8_t* source_buffer, size_t source_size, uint8_t* target_buffer, size_t max_target_size) {
    if (!g_dctx || !source_buffer || !target_buffer || max_target_size == 0) {
        return 0;
    }

    // Reset decompression state for the new buffer
    ZSTD_DCtx_reset(g_dctx, ZSTD_reset_session_only);

    ZSTD_inBuffer input = { source_buffer, source_size, 0 };
    ZSTD_outBuffer output = { target_buffer, max_target_size, 0 };

    // Standard streaming decompression (Natively single-threaded, highly optimized)
    size_t const result = ZSTD_decompressStream(g_dctx, &output, &input);

    if (ZSTD_isError(result) || result > 0) {
        return 0; // Buffer capacity too small or corrupted stream frame
    }

    return output.pos;
}

void close_zstd(void) {
    if (g_cctx) {
        ZSTD_freeCCtx(g_cctx);
        g_cctx = NULL;
    }
    if (g_dctx) {
        ZSTD_freeDCtx(g_dctx);
        g_dctx = NULL;
    }
}
