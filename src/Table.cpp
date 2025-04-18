#include"Table.h"
#include<stdexcept>
#include<iostream>
bool Table::isTypeMatch(Value value, DataType type){ //类型匹配
    switch(type){
        case DataType::INT:
            return std::holds_alternative<int>(value);
        case DataType::DOUBLE:
            return std::holds_alternative<double>(value);
        case DataType::STRING:
            return std::holds_alternative<std::string>(value);
        default:
            return false;
    }
}
void Table::addField(std::string fieldName,DataType type){ //添加字段
    for(auto &name:fields){
        if(name.first == fieldName){
            throw std::invalid_argument("Field already exists: " + fieldName);
            return;
        }
    }
    fields.push_back({fieldName,type});
}
void Table::addRecord(std::vector<Value> record){ //添加记录
    if(record.size() != fields.size()){
        throw std::invalid_argument("Record size does not match fields size.");
    }
    for(size_t i = 0; i < fields.size(); ++i){
        if(!isTypeMatch(record[i],fields[i].second)){
            throw std::invalid_argument("Type mismatch for field: " + fields[i].first);
        }
    }
    records.push_back(record);
}
std::string Table::getName(){ //获取表名
    return name;
}
std::vector<std::vector<Value>> Table::getRecords(){ //获取记录
    return records;
}
std::vector<std::pair<std::string,DataType>> Table::getFields(){ //获取字段
    return fields;
}
size_t Table::getFieldIndex(const std::string& name) const{
    for(size_t i=0;i<fields.size();i++){
        if(fields[i].first == name){
            return i;
        }
    }
    throw std::out_of_range("Field not found: " + name);
}
Table* Table::Select(std::vector<std::string> columnNames,const std::function<bool(const std::vector<Value>&)>& condition){
    Table* result=new Table();
    std::vector<size_t> indexs(columnNames.size());
    for(int i=0;i<columnNames.size();i++){
        size_t index=getFieldIndex(columnNames[i]);
        result->addField(columnNames[i],fields[index].second);
        indexs[i]=index;
    }
    for(auto &record:records){
        if(condition(record)){
            std::vector<Value> newRecord(columnNames.size());
            for(int i=0;i<columnNames.size();i++){
                newRecord[i]=record[indexs[i]];
            }
            result->addRecord(newRecord);
        }
    }
    return result;
}
Table* Table::Select(){
    return this;
}
void Table::Delete(){
    records.clear();
}
void Table::Delete(const std::function<bool(const std::vector<Value>&)>& condition){
    auto it=records.begin();
    while(it!=records.end()){
        if(condition(*it)){
            it=records.erase(it);
        }else{  
            ++it;
        }
    }
}
void Table::insert(std::vector<std::string> names,std::vector<Value> values){
    std::vector<int> indexs;
    for(auto &name:names){
        int index;
        try{
            index=getFieldIndex(name);
        }
        catch(const std::out_of_range& e){
            throw std::invalid_argument("Field not found: " + name);
            return;
        }
        indexs.push_back(index);
    }
    std::vector<Value> record(fields.size());
    for(size_t i=0;i<indexs.size();i++){
        if(!isTypeMatch(values[i],fields[indexs[i]].second)){
            throw std::invalid_argument("Type mismatch for field: " + fields[indexs[i]].first);
        }
        record[indexs[i]]=values[i];
    }
    records.push_back(record);
}
void Table::update(std::string name,Value newValue,const std::function<bool(const std::vector<Value>&)>& condition){
    size_t index;
    try{
        index=getFieldIndex(name);
    }
    catch(const std::out_of_range& e){
        throw std::invalid_argument("Field not found: " + name);
        return;
    }
    if(!isTypeMatch(newValue,fields[index].second)){
        throw std::invalid_argument("Type mismatch for field: " + fields[index].first);
    }
    for(auto &record:records){
        if(condition(record)){
            record[index]=newValue;
        }
    }
}
void Table::printTable(){
    for(auto &field:fields){
        std::cout<<field.first<<"\t";
    }
    std::cout<<std::endl;
    std::cout<<"--------------------------------------------------"<<std::endl;
    for(auto &record:records){
        for(auto &value:record){
            printValue(value);
            std::cout<<"\t";
        }
        std::cout<<std::endl;
    }
}