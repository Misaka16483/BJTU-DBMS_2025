#pragma once
#include"Table.h"
class Database{
    std::vector<Table> tables; //表
    std::string name; //数据库名
public:
    Database(std::string name):name(name){} //构造函数
    void createTable(std::string tableName);
};