#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK_SIZE 4096
//#define MCA_DEBUG

bool convert(FILE *infile, FILE *outfile);
int copyFile(FILE *infile, FILE *outfile, size_t size, size_t chunk_size);

void reverse(unsigned char *dest, unsigned char *src, size_t size);

int main(int argc, char *argvs[]) {
    if (argc == 3) {
        bool success = false;
        FILE *infile, *outfile;
        infile = fopen(argvs[1], "rb");
        outfile = fopen("temp.txt", "wb");
        if (! ferror(infile) && ! ferror(outfile)) {
            if (convert(infile, outfile)) {
                success = true;
            }
            else {
                printf("Convertation error!\n");
            }
        }
        else {
            printf("file open error!\n");
        }
        fclose(infile);
        fclose(outfile);

        if (success) {
            char str_buff[512];
            snprintf(str_buff, 512, "uncomp.exe %s %s", "temp.txt", argvs[2]);
            if (system(str_buff) == 0) {
                printf("complete!\n");
            }
            else {
                printf("uncompressing error!\n");
            }
        }
        #ifndef MCA_DEBUG
            remove("temp.txt");
        #endif
    }
    else {
        printf("incorrect args!");
    }
}

bool convert(FILE *infile, FILE *outfile) {
    unsigned char inbuff[CHUNK_SIZE];
    fseek(infile, 8192, SEEK_SET); // test
    if (fread(inbuff, 1, 5, infile) != 5) {
        printf("file read error!");
        return false;
    }
    int data_size;
    unsigned char comp_type;
    reverse((unsigned char*)(&data_size), inbuff, 4);
    memcpy(&comp_type, inbuff + 4, 1);
    #ifdef MCA_DEBUG
        printf("Converted data size: %i\nCompressing type: %i\n", data_size, comp_type);
    #endif

    int bytes_copied = copyFile(infile, outfile, data_size, CHUNK_SIZE);
    if (bytes_copied == -1) {
        printf("file copy error!");
        return false;
    }
    else {
        #ifdef MCA_DEBUG
            printf("bytes copied: %i\n", bytes_copied);
        #endif
    }
    
    return true;    
}


int copyFile(FILE *infile, FILE *outfile, size_t size, size_t chunk_size) {
    unsigned char *inbuff = (unsigned char*)malloc(chunk_size);

    bool success = true;
    int nbytes, bytes_copied = 0, data_size = size;
    do {
        if (data_size > chunk_size) nbytes = chunk_size; else nbytes = data_size;
        nbytes = fread(inbuff, 1, nbytes, infile);

        if (ferror(infile)) {
            printf("file read error!");
            success = false;
            break;
        }

        if (nbytes == 0) break;

        if (fwrite(inbuff, 1, nbytes, outfile) != nbytes) {
            printf("file write error!");
            success = false;
            break;
        }
        data_size -= nbytes;
        bytes_copied += nbytes;
    } while(data_size > 0);
    free(inbuff);

    if (! success) return -1;
    return bytes_copied;
}

void reverse(unsigned char *dest, unsigned char *src, size_t size) {
    for (size_t i = 0; i < size; i++) *(dest + size - 1 - i) = *(src + i);
}