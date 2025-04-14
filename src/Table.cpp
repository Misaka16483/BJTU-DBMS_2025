#include"Table.h"
#include<stdexcept>
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
size_t Table::getFiledIndex(const std::string& name) const{
    for(size_t i=0;i<fields.size();i++){
        if(fields[i].first == name){
            return i;
        }
    }
    throw std::out_of_range("Field not found: " + name);
}
std::vector<std::vector<Value>> Table::Select(
    const std::function<bool(const std::vector<Value>&)>& condition) const 
{
    std::vector<std::vector<Value>> result;
    for (const auto& row : records) {
        if (condition(row)) {
            result.push_back(row);
        }
    }
    return result;
}
std::vector<std::vector<Value>> Table::Select() const{
    return records;
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