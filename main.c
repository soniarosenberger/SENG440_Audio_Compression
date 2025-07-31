// main.c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define WAV_HEADER_SIZE 44


// Prototypes for μ-law routines
uint8_t  MuLawCompress(int16_t sample);
int16_t  MuLawDecompress(uint8_t code);


// Top-level I/O routines
void CompressToRaw(const char *wavPath, const char *rawPath);
void DecompressFromRaw(const char *rawPath, const char *wavPath);

// Main
int main(int argc, char *argv[]) {
    const char *INPUT_WAV      = "input.wav"; // placeholder filepaths
    const char *COMPRESSED_RAW = "compressed.raw";
    const char *OUTPUT_WAV     = "output.wav";

    clock_t startC = clock(); // compression stopwatch
    CompressToRaw(INPUT_WAV, COMPRESSED_RAW);
    clock_t endC   = clock();

    clock_t startD = clock(); // decompression stopwatch
    DecompressFromRaw(COMPRESSED_RAW, OUTPUT_WAV);
    clock_t endD   = clock();

    double tC = (double)(endC   - startC) / CLOCKS_PER_SEC;
    double tD = (double)(endD   - startD) / CLOCKS_PER_SEC;
    double tT = (double)(endD   - startC) / CLOCKS_PER_SEC; // calculates total elapsed time

    printf("Compression time:   %.3f s\n", tC);
    printf("Decompression time: %.3f s\n", tD);
    printf("Total time:         %.3f s\n", tT);

    return 0;
}

//----------------------------------------------------------------------
// Read WAV header, then encode every 16-bit sample to 8-bit μ-law
void CompressToRaw(const char *wavPath, const char *rawPath) {
    FILE *wav = fopen(wavPath, "rb");
    if (!wav) {
        perror("fopen(input.wav)");
        exit(EXIT_FAILURE);
    }

    // Skip over the 44-byte WAV header
    uint8_t header[WAV_HEADER_SIZE];
    if (fread(header, 1, WAV_HEADER_SIZE, wav) != WAV_HEADER_SIZE) {
        fprintf(stderr, "Error reading WAV header\n");
        fclose(wav);
        exit(EXIT_FAILURE);
    }

    FILE *raw = fopen(rawPath, "wb");
    if (!raw) {
        perror("fopen(compressed.raw)");
        fclose(wav);
        exit(EXIT_FAILURE);
    }

    int16_t sample;
    while (fread(&sample, sizeof(sample), 1, wav) == 1) {
        uint8_t code = MuLawCompress(sample);
        fwrite(&code, sizeof(code), 1, raw);
    }

    fclose(raw);
    fclose(wav);
}

//----------------------------------------------------------------------
// Re-read original WAV header, write it out (adjusted for 16-bit PCM),
// then decode every μ-law byte back into a 16-bit sample
void DecompressFromRaw(const char *rawPath, const char *wavPath) {
    // Reopen original WAV to grab its header fields
    FILE *orig = fopen("input.wav", "rb");
    if (!orig) {
        perror("fopen(input.wav for header)");
        exit(EXIT_FAILURE);
    }
    uint8_t header[WAV_HEADER_SIZE];
    if (fread(header, 1, WAV_HEADER_SIZE, orig) != WAV_HEADER_SIZE) {
        fprintf(stderr, "Error reading original WAV header\n");
        fclose(orig);
        exit(EXIT_FAILURE);
    }
    fclose(orig);

    // TODO: adjust header[].bitsPerSample = 16, byteRate, blockAlign, data sizes...
    //       so that the new WAV header reflects 16-bit PCM data of length = raw file size

    FILE *raw = fopen(rawPath, "rb");
    if (!raw) {
        perror("fopen(compressed.raw)");
        exit(EXIT_FAILURE);
    }
    FILE *wav = fopen(wavPath, "wb");
    if (!wav) {
        perror("fopen(output.wav)");
        fclose(raw);
        exit(EXIT_FAILURE);
    }

    // Write the (possibly adjusted) header into the output WAV
    if (fwrite(header, 1, WAV_HEADER_SIZE, wav) != WAV_HEADER_SIZE) {
        fprintf(stderr, "Error writing WAV header\n");
        fclose(raw);
        fclose(wav);
        exit(EXIT_FAILURE);
    }

    uint8_t code;
    while (fread(&code, sizeof(code), 1, raw) == 1) {
        int16_t sample = MuLawDecompress(code);
        fwrite(&sample, sizeof(sample), 1, wav);
    }

    fclose(wav);
    fclose(raw);
}

//----------------------------------------------------------------------
// Stubs for μ-law routines—replace these 
uint8_t MuLawCompress(int16_t sample) {
    // TODO: implement μ-law encoding here
    return 0;
}

int16_t MuLawDecompress(uint8_t code) {
    // TODO: implement μ-law decoding here
    return 0;
}
