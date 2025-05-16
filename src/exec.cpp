#include"exec.h"
#include"File.h"
#include<iostream>
#include<vector>
#include<stdexcept>
#include<variant>
#include<filesystem>
using namespace std;
namespace fs=std::filesystem;
void exec_single(string& sql);
void exec(string& sql){
    int len=sql.length();
    int prev=0;
    for(int i=0;i<len;i++){
        if(sql[i]==';'){
            try{
                string single=sql.substr(prev,i+1);
                if(single=="EXIT;"||single=="QUIT;"){
                    exit(0);
                }
                if(single=="COMMIT;"){
                    saveDatabase(db);
                    continue;
                }
                string str=sql.substr(prev,i+1);
                exec_single(str);
                prev=i+1;
            }
            catch(exception &e){
                cout<<"Error: "<<e.what()<<endl;
            }
        }
    }
}
void exec_single(string& sql){
    SqlAst *root = parse_sql((char *)sql.c_str());
    if(root == nullptr){
        throw runtime_error("Failed to parse SQL statement.");
    }
    else if(root->type == SELECT_STMT){
        do_select(root->select)->printTable();
    }
    else if(root->type == DELETE_STMT){
        do_delete(root->Delete);
    }
    else if(root->type == INSERT_STMT){
        do_insert(root->insert);
    }
    else if(root->type == UPDATE_STMT){
        do_update(root->update);
    }
    else if(root->type == USE_STMT){
        loadDatabase(root->use->db);
    }
    else if(root->type == CREATE_STMT){
        do_create(root->create);
    }
    else{
        throw runtime_error("Unknown SQL statement.");
    }
}
std::vector<std::vector<Value>> joinTables(std::vector<Table *> &tables,int startIndex){
    if(startIndex == tables.size()-1){
        return tables[startIndex]->getRecords();
    }
    std::vector<std::vector<Value>> result;
    std::vector<std::vector<Value>> records1 = tables[startIndex]->getRecords();
    std::vector<std::vector<Value>> records2 = joinTables(tables, startIndex + 1);
    for(const auto &record1 : records1){
        for(const auto &record2 : records2){
            std::vector<Value> newRecord = record1;
            newRecord.insert(newRecord.end(), record2.begin(), record2.end());
            result.push_back(newRecord);
        }
    }
    return result;
}
Table* do_select(SelectNode *selectNode){
    std::string tableName=selectNode->table_head->table;
    if(db==nullptr){
        throw runtime_error("Database not initialized.");
        return nullptr;
    }
    else{
        /*Table *table=db->getTable(tableName);
        if(table==nullptr){
            throw runtime_error("Table not found.");
            return nullptr;
        }
        std::vector<std::string> fieldNames;
        ExprNode *col=selectNode->column_head;
        while(col){
            if(col->type==SELECT_ALL){
                return table->Select(selectNode->where);
            }
            else{
                fieldNames.push_back(col->text);
                
                col=col->next;
            }
        }
        return table->Select(fieldNames,selectNode->where);*/
        std::vector<Table *> tables;
        TableNode *tableNode = selectNode->table_head;
        while(tableNode){
            std::string tableName = tableNode->table;
            Table *table = db->getTable(tableName);
            if(table == nullptr){
                throw std::runtime_error("Table not found: " + tableName);
            }
            tables.push_back(table);
            tableNode = tableNode->next;
        }
        if(tables.size() == 1){
            Table *table=db->getTable(tableName);
            if(table==nullptr){
                throw runtime_error("Table not found.");
                return nullptr;
            }
            std::vector<std::string> fieldNames;
            ExprNode *col=selectNode->column_head;
            while(col){
                if(col->type==SELECT_ALL){
                    return table->Select(selectNode->where);
                }
                else{
                    fieldNames.push_back(col->text);
                    
                    col=col->next;
                }
            }
            return table->Select(fieldNames,selectNode->where);
        }
        else{
            // Join the tables
            Table *result = new Table("result");
            for(Table *table : tables){
                std::vector<std::pair<std::string, DataType>> fields = table->getFields();
                for(auto &field : fields){
                    result->addField(table->getName()+"."+field.first, field.second, table->getName());
                }
            }
            std::vector<std::vector<Value>> records;
            records=joinTables(tables,0);
            for(const auto &record : records){
                result->addRecord(record);
            }
            std::vector<std::string> tableNames;
            ExprNode *col=selectNode->column_head;
            while(col){
                if(col->type==SELECT_ALL){
                    return result;
                }
                else{
                    tableNames.push_back(col->text);
                    col=col->next;
                }
            }
            return result->Select(tableNames,selectNode->where);
        }

    }
    return nullptr;
}
void do_insert(InsertNode *insert){
    if(insert==nullptr || insert->table ==nullptr||insert->value_list_head==nullptr){
        throw std::invalid_argument("Invalid insert statement.");
    }
    std::string tableName=insert->table;
    if(db==nullptr){
        throw std::runtime_error("Database not initialized.");
    }
    else{
        Table *table=db->getTable(tableName);
        if(table==nullptr){
            throw std::runtime_error("Table not found.");
        }
        std::vector<Value> values;
        for(ValueListNode *vl = insert->value_list_head;vl;vl=vl->next){
            for(ExprNode *v=vl->head;v;v=v->next){
                if(v->type==EXPR_INTNUM){
                    values.push_back(v->intval);
                }
                else if(v->type==EXPR_APPROXNUM){
                    values.push_back(v->floatval);
                }
                else if(v->type==EXPR_STRING){
                    values.push_back(v->strval);
                }
            }
        }
        table->insert(values);
    }
    return;
}
void do_delete(DeleteNode *deleteNode){
    if(deleteNode==nullptr || deleteNode->table==nullptr){
        throw std::invalid_argument("Invalid delete statement.");
    }
    std::string tableName=deleteNode->table;
    if(db==nullptr){
        throw std::runtime_error("Database not initialized.");
    }
    else{
        Table *table=db->getTable(tableName);
        if(table==nullptr){
            throw std::runtime_error("Table not found.");
        }
        table->Delete(deleteNode->where);
    }
}
void do_update(UpdateNode *updateNode){
    if(updateNode==nullptr || updateNode->table==nullptr || updateNode->where==nullptr||updateNode->set_head==nullptr){
        throw std::invalid_argument("Invalid update statement.");
    }
    std::string tableName=updateNode->table;
    if(db==nullptr){
        throw std::runtime_error("Database not initialized.");
    }
    else{
        Table *table=db->getTable(tableName);
        if(table==nullptr){
            throw std::runtime_error("Table not found.");
        }
        for(SetNode *set=updateNode->set_head;set;set=set->next){
        }
        table->update(updateNode->set_head,updateNode->where);
    }
}
void do_create(CreateNode *createNode){
    if(createNode==nullptr){
        throw std::invalid_argument("Invalid create statement.");
    }
    if(createNode->type==CREATE_DATABASE){
        std::string dbName=createNode->name;
        fs::path dbPath("./data/"+dbName);
        if(fs::exists(dbPath)){
            throw std::runtime_error("Database already exists.");
        }
        else{
            fs::create_directory(dbPath);
        }
    }
    else if(createNode->type==CREATE_TABLE){
        std::string tableName=createNode->name;
        if(db==nullptr){
            throw std::runtime_error("Database not initialized.");
        }
        else{
            Table *table=new Table(tableName);
            FieldListNode *fieldList=createNode->field_list_head;
            while(fieldList){
                FieldNode *field=fieldList->field;
                std::string fieldName=field->name;
                DataType fieldType;
                if(field->type==FIELD_TYPE_INT){
                    fieldType=DataType::INT;
                }
                else if(field->type==FIELD_TYPE_FLOAT){
                    fieldType=DataType::FLOAT;
                }
                else if(field->type==FIELD_TYPE_STRING){
                    fieldType=DataType::STRING;
                }
                table->addField(fieldName,fieldType);
                fieldList=fieldList->next;
            }
            db->insertTable(*table);
        }
        
    }
}