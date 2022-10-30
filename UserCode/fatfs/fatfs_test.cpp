#include "fatfs_test.h"
#include <iostream>

using namespace std;

#define ReadBufferSize 4096
static FIL file;
static FATFS fs;
static const char WriteBuffer[] = "This is test file\n";
static uint8_t ReadBuffer[ReadBufferSize];

void FatfsTest(const char *file_path)
{
    FRESULT res;

    res = f_mount(&fs, "0", 1);
    if (res == FR_OK) {
        cout << "f_mount OK!" << endl;
    } else {
        cout << "f_mount failed: " << (int)res << endl;
    }

    res = f_open(&file, file_path, FA_OPEN_APPEND | FA_READ | FA_WRITE);
    if (res == FR_OK) {
        cout << "f_open OK!" << endl;
    } else {
        cout << "f_open failed: " << (int)res << endl;
    }

    UINT bytes_written;
    res = f_write(&file, WriteBuffer, sizeof(WriteBuffer), &bytes_written);
    if (res == FR_OK) {
        cout << "f_write OK! bytes_written: " << bytes_written << endl;
    } else {
        cout << "f_write failed: " << (int)res << endl;
    }
    f_close(&file);

    res = f_open(&file, file_path, FA_READ);

    UINT bytes_read;
    res = f_read(&file, ReadBuffer, ReadBufferSize, &bytes_read);
    if (res == FR_OK) {
        cout << "f_read OK! bytes_read: " << bytes_read << endl;
    } else {
        cout << "f_read failed: " << (int)res << endl;
    }
    for (uint i = 0; i < bytes_read; i++) {
        cout << (char)ReadBuffer[i];
    }
    cout << endl;

    f_close(&file);
    if (res == FR_OK) {
        cout << "f_close OK!" << endl;
    } else {
        cout << "f_close failed: " << (int)res << endl;
    }
}
