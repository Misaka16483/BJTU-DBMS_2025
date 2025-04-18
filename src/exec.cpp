#include"exec.h"
#include"Database.h"
#include<iostream>
#include<vector>
#include<stdexcept>
using namespace std;
void exec_single(string& sql);
Table* do_select(SelectNode *selectNode,int subq);
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
        do_select(root->select,0)->printTable();
    }
    else if(root->type == DELETE_STMT){

    }
    else if(root->type == INSERT_STMT){

    }
    else if(root->type == UPDATE_STMT){

    }
}
Table* do_select(SelectNode *selectNode,int subq){
    if(!subq){
        std::string tableName=selectNode->table_head->table;
        if(db==nullptr){
            throw runtime_error("Database not initialized.");
            return nullptr;
        }
        else{
            Table *table=db->getTable(tableName);
            if(table==nullptr){
                throw runtime_error("Table not found.");
                return nullptr;
            }
            std::vector<std::string> fieldNames;
            ExprNode *col=selectNode->column_head;
            while(col){
                if(col->type==SELECT_ALL){
                    return table->Select();
                }
                else{
                    fieldNames.push_back(col->text);
                    col=col->next;
                }
            }
            return table->Select(fieldNames,[selectNode](const std::vector<Value>& row) {
                if(selectNode->where==nullptr){
                    return true;
                }
                else{
                    return true;
                    //remain to implement the where condition
                }
            });
        }
    }
    return nullptr;
}