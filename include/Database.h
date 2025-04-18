#pragma once
#include"Table.h"
class Database{
    std::vector<Table> tables; //表
    std::string name; //数据库名
public:
    Database(std::string name):name(name){} //构造函数
    void createTable(std::string tableName,std::vector<std::string> fieldNames,std::vector<DataType> fieldTypes); //创建表
    void dropTable(std::string tableName); //删除表
    Table* getTable(std::string tableName); //获取表
};
extern Database *db;