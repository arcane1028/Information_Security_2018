#include <stdio.h>
#include <stdlib.h>

int GetFileSize(FILE *fp) {

    int offset_bkup;
    int fsize;

    offset_bkup = ftell(fp);
    fseek(fp, 0, SEEK_END);

    fsize = ftell(fp);
    fseek(fp, offset_bkup, SEEK_SET);

    printf("File Size = %d\n", fsize);

    return fsize;
}

void Encrypt(char *buff, int key) {
    // 대문자
    if (*buff > 64 && *buff < 91) {
        if ((*buff + key) > 90) {
            *buff = (char)((*buff + key)-26);
        }else{
            *buff = (char)(*buff + key);
        }
    }
    // 소문자
    if (*buff > 96 && *buff < 123) {
        if ((*buff + key) > 122) {
            *buff = (char)((*buff + key)-26);
        }else{
            *buff = (char)(*buff + key);
        }
    }
}

void Decrypt(char *buff, int key) {
    // 대문자
    if (*buff > 64 && *buff < 91) {
        if ((*buff - key) < 65) {
            *buff = (char)((*buff - key)+26);
        }else{
            *buff = (char)(*buff - key);
        }
    }
    // 소문자
    if (*buff > 96 && *buff < 123) {
        if ((*buff - key) < 97) {
            *buff = (char)((*buff - key)+26);
        }else{
            *buff = (char)(*buff - key);
        }
    }
}

int main() {

    char inputFileName[256];
    char outputFileName[256];
    FILE *input_FD;
    FILE *output_FD;
    int mode;
    int key;

    printf(">> Input File Name :");
    scanf("%s", inputFileName);

    printf(">> Input Mode [0 : Encrypt, 1 : Decrypt ] : ");
    scanf("%d", &mode);

    printf(">> Input Key : ");
    scanf("%d", &key);

    if (mode == 0) {
        sprintf(outputFileName, "encrypt.txt");
    } else if (mode == 1) {
        sprintf(outputFileName, "decrypt.txt");
    } else {
        printf("[!] Mode Error !\n");
        exit(1);
    }

    input_FD = fopen(inputFileName, "rb");
    output_FD = fopen(outputFileName, "wb");
    int fileSize = GetFileSize(input_FD);
    char buff;
    int i;
    for (i = 0; i < fileSize; ++i) {
        fread(&buff, sizeof(char), 1, input_FD);

        if (mode == 0) {
            Encrypt(&buff, key);
        } else if (mode == 1) {
            Decrypt(&buff, key);
        }

        fwrite(&buff, sizeof(char), 1, output_FD);
    }
    fclose(output_FD);
    fclose(input_FD);

    return 0;
}