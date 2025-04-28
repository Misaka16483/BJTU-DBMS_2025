#pragma once
extern "C"{
    #include"ast.h"
}
#include<string>
#include"Database.h"
void exec(std::string &sql);
Table* do_select(SelectNode *selectNode);
void do_insert(InsertNode *insert);
void do_delete(DeleteNode *deleteNode);
void do_update(UpdateNode *updateNode);