
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "fileLib.h"

#ifdef SYSTEM_FUNTIONS

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define HANDLE int
#define READ(handle, buffer, size) read(handle, buffer, size)
#define OPENW(path) open(path, O_WRONLY | O_CREAT, S_IRWXU)
#define OPENR(path) open(path, O_RDONLY)
#define OPENWR(path) open(path, O_RDWR)
#define CLOSE(handle) close(handle)
#define WRITE(handle, buffer, size) write(handle, buffer, recordSize)

#define NOT_READ(handle, buffer, size) !read(handle, buffer, size)
#define NOT_WRITTEN(handle, buffer, size) !write(handle, buffer, recordSize)
#define SEEK_BEGINNING(handle, size) lseek(handle, size, SEEK_SET)
#define SEEK_CURRENT(handle, size) lseek(handle, size, SEEK_CUR)


#else

#define HANDLE FILE *
#define READ(handle, buffer, size) fread(buffer, sizeof(char), size, handle)
#define OPENW(path) fopen(path, "w")
#define OPENR(path) fopen(path, "r")
#define OPENWR(path) fopen(path, "r+")
#define CLOSE(handle) fclose(handle)
#define WRITE(handle, buffer, size) fwrite(buffer, sizeof(char), size, handle)

#define NOT_READ(handle, buffer, size) \
        fread(buffer, sizeof(char), size, handle) != (unsigned int) size
#define NOT_WRITTEN(handle, buffer, size) \
        fwrite(buffer, sizeof(char), size, handle) != (unsigned int) size
#define SEEK_BEGINNING(handle, size) fseek(handle, size, 0)
#define SEEK_CURRENT(handle, size) fseek(handle, size, 1)

#endif



void generateFile(char *filePath, int recordsNumber, int recordSize) {


    HANDLE handle = OPENW(filePath);
    HANDLE randHandle = OPENR("/dev/urandom");


    char* buffer = (char*) malloc(recordSize * sizeof(char));

    if (handle && randHandle) {
        for (int i = 0; i < recordsNumber; i++) {

            if(NOT_READ(randHandle, buffer,recordSize)) {
            printf("Reading from /dev/urandom failed, shutting down.");
            CLOSE(handle);
            CLOSE(randHandle);
            exit(EXIT_FAILURE);
            }

            for (int j = 0; j < recordSize - 1; j++) {
                if(buffer[j] < 0) buffer[j] = -buffer[j];
                buffer[j] = 48 + buffer[j] % 74;
            }

            buffer[recordSize - 1] = 10; //newline for readability

            if (NOT_WRITTEN(handle, buffer, recordSize)) {
                printf("Writing to %s failed, shutting down.", filePath);
                CLOSE(handle);
                CLOSE(randHandle);
                exit(EXIT_FAILURE);
            }
        }
    }
    CLOSE(handle);
    CLOSE(randHandle);
}


void copyFile(char *sourceFileName, char *destFileName, int recordsNumber, int bufferSize) {

    HANDLE sourceFile = OPENR(sourceFileName);
    HANDLE destFile = OPENW(destFileName);
    char* buffer = (char*) malloc(bufferSize * sizeof(char));

    if(sourceFile && destFile) {
        for(int i=0; i < recordsNumber; i++) {
            if(NOT_READ(sourceFile,buffer,bufferSize)) {
                printf("Reading from %s failed, shutting down.",sourceFileName);
                CLOSE(sourceFile);
                CLOSE(destFile);
                exit(EXIT_FAILURE);
            }
            if(NOT_WRITTEN(destFile,buffer,bufferSize)) {
                printf("Writing to %s failed, shutting down.",destFileName);
                CLOSE(sourceFile);
                CLOSE(destFile);
                exit(EXIT_FAILURE);
            }
        }
    }
    CLOSE(sourceFile);
    CLOSE(destFile);
}

void sortFile(char *filePath, int recordsNumber, int recordSize) {

    HANDLE handle = OPENWR(filePath);
    char* bufferOne = (char*) malloc(recordSize * sizeof(char));
    char* bufferTwo = (char*) malloc(recordSize * sizeof(char));

    if(handle) {

        for(int i=1; i < recordsNumber; i++) {
            SEEK_BEGINNING(handle,i*recordSize);
            READ(handle,bufferOne,recordSize);
            int j=0;
            while(1) {
                SEEK_BEGINNING(handle,j*recordSize);
                READ(handle,bufferTwo,recordSize);
                if(j>=i || bufferOne[0]<bufferTwo[0]) {
                    break;
                }
                j++;
            }

            SEEK_CURRENT(handle,-recordSize);
            WRITE(handle,bufferOne,recordSize);
            for(int k = j+1; k < i+1; k++) {
                SEEK_BEGINNING(handle,k*recordSize);
                READ(handle,bufferOne,recordSize);
                SEEK_CURRENT(handle,-recordSize);
                WRITE(handle,bufferTwo,recordSize);
                strncpy(bufferTwo,bufferOne,recordSize);
            }

        }
    }
    CLOSE(handle);
}
