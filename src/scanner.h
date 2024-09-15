#ifndef SCANNER_H
#define SCANNER_H

#include <cstdio>

#define BUFFER_SIZE 128

class Scanner {
private:
    struct Buffer {
        char data[BUFFER_SIZE];
        int index = 0;
        int size = 0;
    };

    FILE* file;
    Buffer buffer;
    char cur_char;
public:
    int line = 1;
    int column = 1;

    Scanner(FILE* file) : file(file) {

        buffer.size = (int)fread(buffer.data, sizeof(char), BUFFER_SIZE, file);

        if (buffer.size < BUFFER_SIZE) {
            buffer.data[buffer.size] = EOF;
        }

        cur_char = buffer.data[0];
    }

    inline char peek() {
        return cur_char;
    }

    char consume();
};

#endif