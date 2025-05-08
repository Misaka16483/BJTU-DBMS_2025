#include <iostream>
#include"exec.h"
extern "C"{
#include"ast.h"
}
#include"Database.h"
int main(int, char**){

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
