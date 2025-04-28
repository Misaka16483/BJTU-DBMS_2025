#pragma once
#include<variant>
#include<string>
#include<vector>
using Value = std::variant<int, float, std::string>;
enum class DataType{
    INT,
    FLOAT,
    STRING
};
void printValue(Value v);
void printRecord(std::vector<Value>);