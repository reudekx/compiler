#include "scanner.h"

Scanner::Scanner(FILE* file) : file(file) {

    buffer.size = (int)fread(buffer.data, sizeof(char), BUFFER_SIZE, file);

    if (buffer.size < BUFFER_SIZE) {
        buffer.data[buffer.size] = EOF;
    }

    cur_char = buffer.data[0];
}

char Scanner::consume() {
    buffer.index++;
    if (cur_char == '\n') {
        column = 1;
        line++;
    } 
    else {
        column++;
    }

    if (buffer.index == buffer.size) {
        buffer.index = 0;
        buffer.size = (int)fread(buffer.data, sizeof(char), BUFFER_SIZE, file);
        if (buffer.size < BUFFER_SIZE) {
            buffer.data[buffer.size] = EOF;
        }
    }

    cur_char = buffer.data[buffer.index];
    return cur_char;
}