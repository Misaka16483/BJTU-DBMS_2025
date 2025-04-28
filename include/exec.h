#pragma once
extern "C"{
    #include"ast.h"
}
#include<string>
#include"Database.h"
void exec(std::string &sql);
Table* do_select(SelectNode *selectNode);

