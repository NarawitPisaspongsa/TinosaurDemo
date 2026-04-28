#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shader_load.h"

char* get_shader_content(const char* fileName) {
    FILE *fp;
    // Use "rb" to get the exact binary size without translation
    if (fopen_s(&fp, fileName, "rb") != 0 || fp == NULL) {
        printf("ERROR: Could not open file %s\n", fileName);
        return NULL; 
    }

    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    rewind(fp); // Go back to start instead of closing and reopening

    // Allocate size + 1 for the null terminator
    char* shaderContent = (char*)malloc(size + 1);
    if (shaderContent == NULL) {
        fclose(fp);
        return NULL;
    }

    size_t bytesRead = fread(shaderContent, 1, size, fp);
    shaderContent[bytesRead] = '\0'; // Manually null terminate

    fclose(fp);
    return shaderContent;
}
