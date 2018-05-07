#ifndef FILE_LIB_H_   /* Include guard */
#define FILE_LIB_H_

void generateFile(char *filePath, int recordsNumber, int recordSize);
void copyFile(char *sourceFileName, char *destFileName, int recordsNumber, int bufferSize);
void sortFile(char *filePath, int recordsNumber, int recordSize);

#endif
