#include<Database.h>
using namespace std;
Database *db=nullptr;
void Database::createTable(std::string tableName,std::vector<std::string> fieldNames,std::vector<DataType> fieldTypes){
    Table table(tableName); //创建表对象
    for(size_t i=0;i<fieldNames.size();i++){
        table.addField(fieldNames[i],fieldTypes[i]); //添加字段
    }
    tables.push_back(table); //将表对象添加到数据库中
}
void Database::dropTable(std::string tableName){
    for(int i=0;i<tables.size();i++){
        if(tables[i].getName()==tableName){ //找到要删除的表
            tables.erase(tables.begin()+i); //删除表
            break;
        }
    }
}
Table* Database::getTable(std::string tableName){
    for(int i=0;i<tables.size();i++){
        if(tables[i].getName()==tableName){ //找到要获取的表
            return &tables[i]; //返回表对象
        }
    }
    return nullptr; //未找到表
}
std::string Database::getName(){
    return name; //返回数据库名
}
void Database::insertTable(Table table){
    this->tables.push_back(table);
}