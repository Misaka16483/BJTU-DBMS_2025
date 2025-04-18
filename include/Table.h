#pragma once
#include"Data.h"
#include<functional>
class Table{
    std::vector<std::pair<std::string,DataType>> fields; //字段
    std::vector<std::vector<Value>> records; //记录
    std::string name; //表名

    bool isTypeMatch(Value value, DataType type); //类型匹配

public:
    Table(std::string name):name(name){} //构造函数
    Table(){}
    void addField(std::string fieldName,DataType type); //添加字段
    void removeField(std::string fieldName); //删除字段
    void addRecord(std::vector<Value> record); //添加记录
    std::vector<std::pair<std::string,DataType>> getFields(); //获取字段
    std::vector<std::vector<Value>> getRecords(); //获取记录
    std::string getName();
    size_t getFieldIndex(const std::string& name) const;
    Table* Select() ; //选择所有记录
    Table* Select(std::vector<std::string> columnNames,const std::function<bool(const std::vector<Value>&)>& condition);
    void Delete();
    void Delete(const std::function<bool(const std::vector<Value>&)>& condition);
    void insert(std::vector<std::string> names,std::vector<Value> values); //插入记录
    void update(std::string name,Value newValue,const std::function<bool(const std::vector<Value>&)>& condition); //更新记录
    //void updateRecord(int index, std::vector<Value> newRecord); //更新记录
    void printTable();
};