#include <iostream>
#include"exec.h"
extern "C"{
#include"ast.h"
}
#include"Database.h"
int main(int, char**){
    db=new Database("test_db");
    db->createTable("TEST_TABLE",{"ID","NAME","AGE"},{DataType::INT,DataType::STRING,DataType::INT});
    Table* table=db->getTable("TEST_TABLE");
    table->addRecord({1, "Alice", 30});
    table->addRecord({2, "Bob", 25});
    table->addRecord({3, "Charlie", 35});
    table->addRecord({4, "David", 20});
    table->addRecord({5, "Eve", 10});
    while(true){
        std::cout<<"sql> ";
        std::string sql;
        std::getline(std::cin, sql);
        for(char &c:sql){
            if(isalpha(c)){
                c=toupper(c);
            }
        }
        try{
            exec(sql);
        }
        catch(std::exception &e){
            std::cout<<"Error: "<<e.what()<<std::endl;
        }
    }
    return 0;
}
