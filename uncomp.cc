#include <stdio.h>
#include "zlib.h"

#define CHUNK_SIZE 4096

bool decompress_file(FILE *infile, FILE *outfile);

int main(int argc, char *argvs[]) {
    int return_code = 0;
    char *infilename, *outfilename;
    if (argc == 3) {
        infilename = argvs[1];
        outfilename = argvs[2];
        FILE *infile = fopen(infilename, "rb");
        FILE *outfile = fopen(outfilename, "wb");
        if (! decompress_file(infile, outfile)) {
            printf("decompress error!\n");
            return_code = 1;
        }
        else {
            printf("success!\n");
        }
        fclose(infile);
        fclose(outfile);
    }
    else {
        printf("incorrect args!\n");
        return_code = 2;
    }

    return return_code;
}

bool decompress_file(FILE *infile, FILE *outfile) {
    unsigned char inbuff[CHUNK_SIZE];
    unsigned char outbuff[CHUNK_SIZE];
    z_stream stream = {0};

    if (inflateInit(&stream) != Z_OK) {
        printf("init error!\n");
        inflateEnd(&stream);
        return false;
    }

    int result;
    do {
        stream.avail_in = fread(inbuff, 1, CHUNK_SIZE, infile);
        if (ferror(infile)) {
            printf("read file error!\n");
            inflateEnd(&stream);
            return false;
        }

        if (stream.avail_in == 0) break;

        stream.next_in = inbuff;

        do {
            stream.avail_out = CHUNK_SIZE;
            stream.next_out = outbuff;
            result = inflate(&stream, Z_NO_FLUSH);

            if(result == Z_NEED_DICT || result == Z_DATA_ERROR || result == Z_MEM_ERROR) {
                printf("inflate error!\n");
                inflateEnd(&stream);
                return false;
            }

            int nbytes = CHUNK_SIZE - stream.avail_out;
            if (fwrite(outbuff, 1, nbytes, outfile) != nbytes || ferror(outfile)) {
                printf("fwrite error!\n");
                inflateEnd(&stream);
                return false;
            }
        } while (stream.avail_out == 0);
    } while(result != Z_STREAM_END);

    inflateEnd(&stream);
    return result == Z_STREAM_END;
}