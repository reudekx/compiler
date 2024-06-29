#ifndef PARSER_H
#define PARSER_H

#include "enums.h"
#include "lexer.h"

#include <vector>

namespace Parser
{
    const std::vector<int>& parse();
    extern int init();
}

#endif