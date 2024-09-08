#include "scanner.h"

char Scanner::consume() {
    buffer.index++;
    if (cur_char == '\n') {
        line = 0;
        column++;
    } 
    else {
        line++;
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