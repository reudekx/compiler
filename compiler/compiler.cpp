#include "enums.h"
#include "lexer.h"
#include "parser.h"

#include <iostream>
#include <vector>

int main()
{

    std::cout << "Compiler started successfully!" << std::endl;

    Lexer::init("code2.txt");

    Parser::init();
    const std::vector<int>& BytecodeList = Parser::parse();

    // ByteCodeList 출력
    for (int i = 0; i < (int)BytecodeList.size(); i++)
    {
        std::cout << BytecodeList[i] << " " << EnumString[type_bc][BytecodeList[i]] << std::endl;
    }

    std:: cout << "Operator detected = " << BytecodeList.size() << std::endl;    

    Lexer::quit();
}
