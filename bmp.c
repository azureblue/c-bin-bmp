#include <stdio.h>
#include <stdlib.h>

static const int HEADER_PLUS_GAPS_LEN = 62;
static const int WIDTH_OFFSET = 0x12;
static const int HEIGHT_OFFSET = 0x16;
static const int LEN_OFFSET = 0x02;
static const int DATA_LEN_OFFSET = 0x22;

static void encodeInt(char * dest, int i) {
    dest[0] = i & 0xFF;
    dest[1] = (i >> 8) & 0xFF;
    dest[2] = (i >> 16) & 0xFF;
    dest[3] = (i >> 24) & 0xFF;
}

static char bmpHeader[] = {
    0x42, 0x4D, 0xBE, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x28, 0x00, 
    0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 
    0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

int writeBinBMP(const char * filePath, const int w, const int h, const int * pixels) {
    int lineLen = (((w + 7) / 8) + 3) / 4 * 4;
    int dataLen = lineLen * h;
    int len = dataLen + HEADER_PLUS_GAPS_LEN;
    char * data = calloc(dataLen, 1);
    if (!data)
        return -1;
    encodeInt(bmpHeader + LEN_OFFSET, len);
    encodeInt(bmpHeader + WIDTH_OFFSET, w);
    encodeInt(bmpHeader + HEIGHT_OFFSET, h);
    encodeInt(bmpHeader + DATA_LEN_OFFSET, dataLen);
    
    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i += 8) {
            int bits = (w - i) > 8 ? 8 : w - i;
            char byte = 0;
            int idx = (h - j - 1) * w + i;
            for (int b = 0; b < bits; b++)
                byte |= (pixels[idx + b] & 1) << (7 - b);
            data[j * lineLen + i / 8] = byte;
        }
    
    FILE * file = fopen(filePath, "wb");
    if (!file) {
        free(data);
        return -1;
    }
    
    fwrite(bmpHeader, 1, HEADER_PLUS_GAPS_LEN, file);
    fwrite(data, 1, dataLen, file);
    free(data);
    fclose(file);
    return 0;
}
