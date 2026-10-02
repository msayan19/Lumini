#include "format.h"
#include <errno.h>
#include <sys/stat.h>

/* --- Helper: Validate LBP Header --- */
static bool validate_lpb_header(const lbp_header h) {
    /* 1.1 Check Identifier */
    if (memcmp(h.identifier, LBP_IDENTIFIER, LBP_IDENTIFIER_BYTE_COUNT) != 0) {
        return false;
    }
    /* 1.2 Check ABI Version */
    if (h.abi_version > ABI_VERSION) {
        return false;
    }
    /* 1.3 Sanity checks on sizes */
    if (h.heap_size == 0 && h.instruction_count == 0 && h.data_size == 0) {
        return false;
    }
    return true;
}

/* --- Public: Validate complete LBP file structure --- */
bool validate_lbp_file(const lbp_file* f) {
    /* Step 0. Check for null pointers */
    if (f == NULL) {
        return false;
    }
    /* Step 1. Verify Header */
    if (!validate_lpb_header(f->header)) {
        return false;
    }
    /* Step 2. Verify Payloads */
    if (f->program == NULL) {
        return false;
    }
    if (f->header.data_size != 0 && f->data == NULL) {
        return false;
    }
    /* Step 3. Check status */
    if (f->status != LPB_FILE_STATE_LOADED) {
        return false;
    }
    return true;
}

/* --- Public: Initialize an LBP file structure --- */
bool init_lbp_file(lbp_file* file, uint32_t abi_version, uint32_t instruction_count, uint32_t data_size,
                   uint32_t heap_size, uint32_t flags) {
    if (file == NULL) {
        return false;
    }
    
    /* Reset structure */
    memset(file, 0, sizeof(lbp_file));
    
    /* Set status to uninitialized */
    file->status = LBP_FILE_STATE_INITIALIZED;
    
    /* Copy identifier */
    memcpy(file->header.identifier, LBP_IDENTIFIER, LBP_IDENTIFIER_BYTE_COUNT);
    
    /* Set metadata */
    file->header.abi_version = abi_version;
    file->header.instruction_count = instruction_count;
    file->header.data_size = data_size;
    file->header.heap_size = heap_size;
    file->header.flags = flags;
    
    /* Clear payload pointers */
    file->program = NULL;
    file->data = NULL;
    
    return true;
}

/* --- Public: Load LBP file from disk --- */
bool load_lbp(lbp_file* file, const char* filepath) {
    /* Step 0. Check for null pointers */
    if (file == NULL || filepath == NULL) {
        return false;
    }
    
    /* Reset file structure first */
    memset(file, 0, sizeof(lbp_file));
    file->status = LBP_FILE_STATE_UNLOADED;
    
    /* Step 1. Open file */
    FILE* f = fopen(filepath, "rb");
    if (f == NULL) {
        fprintf(stderr, "Error: failed to open '%s': %s\n", 
                filepath, strerror(errno));
        return false;
    }
    
    /* Step 2. Get file size */
    struct stat statbuf;
    int fd = fileno(f);
    if (fd == -1) {
        fclose(f);
        fprintf(stderr, "Error: failed to get file descriptor for '%s': %s\n", 
                filepath, strerror(errno));
        return false;
    }
    if (fstat(fd, &statbuf) != 0) {
        fclose(f);
        fprintf(stderr, "Error: failed to stat '%s': %s\n", 
                filepath, strerror(errno));
        return false;
    }
    
    /* Step 3. Validate file size bounds */
    if (statbuf.st_size > (off_t)LBP_FILE_MAX_SIZE || 
        statbuf.st_size < (off_t)LBP_FILE_MIN_SIZE) {
        fprintf(stderr, "Error: file size out of bounds for '%s' (actual size: %ld, expected size: %u-%u)\n", 
                filepath, (long)statbuf.st_size, LBP_FILE_MIN_SIZE, LBP_FILE_MAX_SIZE);
        fclose(f);
        return false;
    }
    
    /* Step 4. Read header */
    if (fread(&file->header, LBP_HEADER_SIZE, 1, f) != 1) {
        perror("Error: failed to read header");
        fclose(f);
        return false;
    }
    
    /* Step 5. Validate header */
    if (!validate_lpb_header(file->header)) {
        fprintf(stderr, "Error: invalid header in '%s'\n", filepath);
        fclose(f);
        return false;
    }
    
    /* Step 6. Calculate expected file size */
    uint64_t expected_size = (uint64_t)LBP_HEADER_SIZE + 
                             file->header.data_size + 
                             ((uint64_t)file->header.instruction_count * INSTRUCTION_BYTES);
    
    if ((uint64_t)statbuf.st_size != expected_size) {
        fprintf(stderr, "Error: file size mismatch for '%s' (got %ld, expected %lu)\n", 
                filepath, (long)statbuf.st_size, (unsigned long)expected_size);
        fclose(f);
        return false;
    }
    
    /* Step 7. Allocate and load program payload */
    uint32_t program_size = file->header.instruction_count * INSTRUCTION_BYTES;
    file->program = (uint8_t*)malloc(program_size);
    if (file->program == NULL) {
        fprintf(stderr, "Error: failed to allocate program buffer (%u bytes)\n", program_size);
        fclose(f);
        return false;
    }
    
    if (fread(file->program, 1, program_size, f) != program_size) {
        perror("Error: failed to read program payload");
        free(file->program);
        file->program = NULL;
        fclose(f);
        return false;
    }
    
    /* Step 8. Allocate and load data payload (if any) */
    if (file->header.data_size > 0) {
        file->data = (uint8_t*)malloc(file->header.data_size);
        if (file->data == NULL) {
            fprintf(stderr, "Error: failed to allocate data buffer (%u bytes)\n", 
                    file->header.data_size);
            free(file->program);
            file->program = NULL;
            fclose(f);
            return false;
        }
        
        if (fread(file->data, 1, file->header.data_size, f) != file->header.data_size) {
            perror("Error: failed to read data payload");
            free(file->program);
            free(file->data);
            file->program = NULL;
            file->data = NULL;
            fclose(f);
            return false;
        }
    }
    
    /* Step 9. Mark as loaded */
    file->status = LPB_FILE_STATE_LOADED;
    
    fclose(f);
    return true;
}

/* --- Public: Write LBP file to disk --- */
bool write_lbp(const lbp_file* file, const char* filepath) {
    /* Step 0. Check for null pointers */
    if (file == NULL || filepath == NULL) {
        return false;
    }
    
    /* Step 1. Validate file structure */
    if (!validate_lbp_file(file)) {
        fprintf(stderr, "Error: invalid LBP file structure for writing\n");
        return false;
    }
    
    /* Step 2. Open file for writing */
    FILE* f = fopen(filepath, "wb");
    if (f == NULL) {
        fprintf(stderr, "Error: failed to create '%s': %s\n", 
                filepath, strerror(errno));
        return false;
    }
    
    /* Step 3. Write header */
    if (fwrite(&file->header, LBP_HEADER_SIZE, 1, f) != 1) {
        perror("Error: failed to write header");
        fclose(f);
        return false;
    }
    
    /* Step 4. Write program payload */
    uint32_t program_size = file->header.instruction_count * INSTRUCTION_BYTES;
    if (fwrite(file->program, 1, program_size, f) != program_size) {
        perror("Error: failed to write program payload");
        fclose(f);
        return false;
    }
    
    /* Step 5. Write data payload (if any) */
    if (file->header.data_size > 0) {
        if (fwrite(file->data, 1, file->header.data_size, f) != file->header.data_size) {
            perror("Error: failed to write data payload");
            fclose(f);
            return false;
        }
    }
    
    /* Step 6. Close file */
    if (fclose(f) != 0) {
        fprintf(stderr, "Error: failed to close '%s': %s\n", 
                filepath, strerror(errno));
        return false;
    }
    
    return true;
}

/* --- Public: Free LBP file resources --- */
void free_lbp(lbp_file* file) {
    if (file == NULL) {
        return;
    }
    
    if (file->program != NULL) {
        free(file->program);
        file->program = NULL;
    }
    
    if (file->data != NULL) {
        free(file->data);
        file->data = NULL;
    }
    
    /* Reset status */
    file->status = LBP_FILE_STATE_UNLOADED;
    
    /* Clear header */
    memset(&file->header, 0, sizeof(lbp_header));
}