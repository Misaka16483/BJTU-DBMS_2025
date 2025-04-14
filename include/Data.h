#pragma once
#include<variant>
#include<string>
#include<vector>
using Value = std::variant<int, double, std::string>;
enum class DataType{
    INT,
    DOUBLE,
    STRING
};
void printValue(Value v);
void printRecord(std::vector<Value>);