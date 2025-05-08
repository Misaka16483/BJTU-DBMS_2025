#include<sstream>
#include<filesystem>
#include"File.h"
namespace fs=std::filesystem;
void loadTable(Table* table, std::ifstream& ifs){
    std::string line;
    std::getline(ifs, line);
    std::istringstream getField(line);
    std::vector<std::string> fieldNames;
    std::vector<DataType> fieldTypes;
    std::string tmp;
    while(getField>>tmp){
        fieldNames.push_back(tmp);
    }
    std::getline(ifs,line);
    std::istringstream getType(line);
    while(getType>>tmp){
        if(tmp=="int"){
            fieldTypes.push_back(DataType::INT);
        }
        else if(tmp=="float"){
            fieldTypes.push_back(DataType::FLOAT);
        }
        else if(tmp=="string"){
            fieldTypes.push_back(DataType::STRING);
        }
    }
    for(size_t i=0;i<fieldNames.size();i++){
        table->addField(fieldNames[i],fieldTypes[i]);
    }
    while (std::getline(ifs, line)) {
        std::istringstream iss(line);
        std::vector<Value> record;
        Value value;
        std::string tmpString;
        int tmpInt;
        float tmpFloat;
        for(int i=0;i<fieldNames.size();i++){
            if(fieldTypes[i]==DataType::INT){
                iss>>tmpInt;
                value=tmpInt;
            }
            else if(fieldTypes[i]==DataType::FLOAT){
                iss>>tmpFloat;
                value=tmpFloat;
            }
            else if(fieldTypes[i]==DataType::STRING){
                iss>>tmpString;
                value=tmpString;
            }
            record.push_back(value);
        }
        if(record.size() != table->getFields().size()) {
            throw std::runtime_error("Record size does not match table fields");
        }
        table->addRecord(record);
    }
}
void loadDatabase(std::string name){
    delete db;
    db=new Database(name);
    fs::path dbPath("./data/"+name);
    if(!fs::exists(dbPath)){
        throw std::runtime_error("Database does not exist");
    }
    std::cout<<"Loading database "<<name<<std::endl;
    for(auto& p:fs::directory_iterator(dbPath)){
        std::cout<<p.path()<<std::endl;
        if(p.path().extension() == ".csv"){
            std::ifstream ifs(p.path());
            if(!ifs.is_open()){
                throw std::runtime_error("Failed to open table file");
            }
            std::string tableName = p.path().stem().string();
            std::cout<<tableName<<std::endl;
            Table* table=new Table(tableName);
            loadTable(table, ifs);
            db->insertTable(*table);
        }
    }
}
void saveTable(Table* table, std::ofstream& ofs){
    std::vector<std::pair<std::string,DataType>> fields=table->getFields();
    std::vector<std::vector<Value>> records=table->getRecords();
    for(auto &field:fields){
        ofs<<field.first<<" ";
    }
    ofs<<std::endl;
    for(auto &field:fields){
        if(field.second==DataType::INT){
            ofs<<"int ";
        }
        else if(field.second==DataType::FLOAT){
            ofs<<"float ";
        }
        else if(field.second==DataType::STRING){
            ofs<<"string ";
        }
    }
    ofs<<std::endl;
    for(auto &record:records){
        for(auto &value:record){
            if(std::holds_alternative<int>(value)){
                ofs<<std::get<int>(value)<<" ";
            }
            else if(std::holds_alternative<float>(value)){
                ofs<<std::get<float>(value)<<" ";
            }
            else if(std::holds_alternative<std::string>(value)){
                ofs<<std::get<std::string>(value)<<" ";
            }
        }
        ofs<<std::endl;
    }
    ofs.close();
}
void saveDatabase(Database* db){
    fs::path dbPath("./data/"+db->getName());
    if(!fs::exists(dbPath)){
        fs::create_directory(dbPath);
    }
    std::vector<Table> &tables=db->tables;
    for(auto &table:db->tables){
        std::ofstream ofs("./data/"+db->getName()+"/"+table.getName()+".csv");
        if(!ofs.is_open()){
            throw std::runtime_error("Failed to open table file");
        }
        saveTable(&table, ofs);
    }
}