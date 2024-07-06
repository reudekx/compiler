#ifndef LEXER_H
#define LEXER_H

#include "enums.h"

namespace Lexer {
extern int getToken();
extern char *getLexeme();
extern int init(const char *FilePath);
extern int quit();
} // namespace Lexer

#endif
