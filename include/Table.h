#pragma once
#include"Data.h"
#include"ast.h"
#include<functional>
class Table{
    std::vector<std::pair<std::string,DataType>> fields; //字段
    std::vector<std::string> fieldOrigin; //字段原表名
    std::vector<std::vector<Value>> records; //记录
    std::string name; //表名
    bool isTypeMatch(Value value, DataType type); //类型匹配
    inline int judge_cond(ExprNode *cond,std::vector<Value> &record,std::vector<std::pair<std::string,DataType>> &fields);
    inline int judge_cond(ExprNode *cond,std::vector<Value> &record,std::vector<std::pair<std::string,DataType>> &fields,std::vector<std::string> &fieldOrigin);
public:
    Table(std::string name):name(name){} //构造函数
    Table(){}
    void addField(std::string fieldName,DataType type); //添加字段
    void addField(std::string fieldName,DataType type,std::string origin);
    void removeField(std::string fieldName); //删除字段
    void addRecord(std::vector<Value> record); //添加记录
    std::vector<std::pair<std::string,DataType>> getFields(); //获取字段
    std::vector<std::vector<Value>> getRecords(); //获取记录
    std::string getName();
    size_t getFieldIndex(const std::string& name) const;
    size_t getFieldIndex(const std::string& name,const std::string& tableName) const;
    Table* Select(ExprNode *cond) ; //选择所有记录
    Table* Select(std::vector<std::string> columnNames,ExprNode *cond);
    Table* Select(std::vector<std::string> tableNames,std::vector<std::string> columnNames,ExprNode *cond);
    void Delete(ExprNode *cond); //删除记录
    void insert(std::vector<std::string> names,std::vector<Value> values); //插入记录
    void update(SetNode *set,ExprNode *cond); //更新记录
    void printTable();
    void insert(std::vector<Value> values);
};