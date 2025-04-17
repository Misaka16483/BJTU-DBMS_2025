#include"Data.h"
#include<iostream>
void printValue(Value v){
    std::visit([](auto&& arg) {
        std::cout << arg ;
    }, v);
}
void printRecord(std::vector<Value> record){
    for(auto &v:record){
        printValue(v);
        std::cout<<" ";
    }
    std::cout<<std::endl;
}