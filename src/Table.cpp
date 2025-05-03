#include"Table.h"
#include<stdexcept>
#include<iostream>
#include"exec.h"
inline ExprNode *eval_expr(ExprNode*expr,std::vector<Value> &record,std::vector<std::pair<std::string,DataType>> &fields);
bool Table::isTypeMatch(Value value, DataType type){ //类型匹配
    switch(type){
        case DataType::INT:
            return std::holds_alternative<int>(value);
        case DataType::FLOAT:
            return std::holds_alternative<float>(value);
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
Table* Table::Select(std::vector<std::string> columnNames,ExprNode *cond){
    Table* result=new Table();
    for(auto &name:columnNames){
        size_t index;
        try{
            index=getFieldIndex(name);
        }
        catch(const std::out_of_range& e){
            throw std::invalid_argument("Field not found: " + name);
            return nullptr;
        }
        result->addField(name,fields[index].second);
    }
    for(auto &record:records){
        if(judge_cond(cond,record,fields)){
            std::vector<Value> newRecord;
            for(auto &name:columnNames){
                size_t index=getFieldIndex(name);
                newRecord.push_back(record[index]);
            }
            result->addRecord(newRecord);
        }
    }
    return result;
}
Table* Table::Select(ExprNode *cond){
    Table* result=new Table();
    for(auto field:fields){
        result->addField(field.first,field.second);
    }
    for(auto &record:records){
        if(judge_cond(cond,record,fields)){
            std::vector<Value> newRecord;
            for(auto &value:record){
                newRecord.push_back(value);
            }
            result->addRecord(newRecord);
        }
    }
    return result;
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
void Table::insert(std::vector<Value> values){
    std::vector<Value> record(fields.size());
    if(record.size() != fields.size()){
        throw std::invalid_argument("Record size does not match fields size.");
    }
    for(size_t i=0;i<record.size();i++){
        if(!isTypeMatch(values[i],fields[i].second)){
            throw std::invalid_argument("Type mismatch for field: " + fields[i].first);
        }
        record[i]=values[i];
    }
    records.push_back(record);
}
void Table::Delete(ExprNode *cond){
    for(auto it=records.begin();it!=records.end();){
        if(judge_cond(cond,*it,fields)){
            it=records.erase(it);
        }
        else{
            ++it;
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
ExprNode* make_expr_by_name(std::string name,std::vector<Value> &record,std::vector<std::pair<std::string,DataType>> &fields){
    ExprNode *expr=new ExprNode();
    for(size_t i=0;i<fields.size();i++){
        if(fields[i].first == name){
            switch(fields[i].second){
                case DataType::INT:
                    expr->type=EXPR_INTNUM;
                    expr->intval=std::get<int>(record[i]);
                    break;
                case DataType::FLOAT:
                    expr->type=EXPR_APPROXNUM;
                    expr->floatval=std::get<float>(record[i]);
                    break;
                case DataType::STRING:
                    expr->type=EXPR_STRING;
                    expr->strval=(char *)std::get<std::string>(record[i]).c_str();
                    break;
            }
        }
    }
    return expr;
}
ExprNode* eval_add_expr(ExprNode *l,ExprNode *r,std::vector<Value> &record,std::vector<std::pair<std::string,DataType>> &fields){
    ExprNode *res=new ExprNode();
    if(l==nullptr||r==nullptr){
        throw std::invalid_argument("Invalid expression.");
    }
    ExprNode *lval=eval_expr(l,record,fields);
    ExprNode *rval=eval_expr(r,record,fields);
    if(lval->type==EXPR_INTNUM&&rval->type==EXPR_INTNUM){
        res->intval=lval->intval+rval->intval;
        res->type=EXPR_INTNUM;
    }
    else if(lval->type==EXPR_APPROXNUM&&rval->type==EXPR_APPROXNUM){
        res->floatval=lval->floatval+rval->floatval;
        res->type=EXPR_APPROXNUM;
    }
    else{
        throw std::invalid_argument("Invalid expression.");
    }
    return res;
}
ExprNode* eval_sub_expr(ExprNode *l,ExprNode *r,std::vector<Value> &record,std::vector<std::pair<std::string,DataType>> &fields){
    ExprNode *res=new ExprNode();
    if(l==nullptr||r==nullptr){
        throw std::invalid_argument("Invalid expression.");
    }
    ExprNode *lval=eval_expr(l,record,fields);
    ExprNode *rval=eval_expr(r,record,fields);
    if(lval->type==EXPR_INTNUM&&rval->type==EXPR_INTNUM){
        res->intval=lval->intval-rval->intval;
        res->type=EXPR_INTNUM;
    }
    else if(lval->type==EXPR_APPROXNUM&&rval->type==EXPR_APPROXNUM){
        res->floatval=lval->floatval-rval->floatval;
        res->type=EXPR_APPROXNUM;
    }
    else{
        throw std::invalid_argument("Invalid expression.");
    }
    return res;
}
ExprNode* eval_mul_expr(ExprNode *l,ExprNode *r,std::vector<Value> &record,std::vector<std::pair<std::string,DataType>> &fields){
    ExprNode *res=new ExprNode();
    if(l==nullptr||r==nullptr){
        throw std::invalid_argument("Invalid expression.");
    }
    ExprNode *lval=eval_expr(l,record,fields);
    ExprNode *rval=eval_expr(r,record,fields);
    if(lval->type==EXPR_INTNUM&&rval->type==EXPR_INTNUM){
        res->intval=lval->intval*rval->intval;
        res->type=EXPR_INTNUM;
    }
    else if(lval->type==EXPR_APPROXNUM&&rval->type==EXPR_APPROXNUM){
        res->floatval=lval->floatval*rval->floatval;
        res->type=EXPR_APPROXNUM;
    }
    else{
        throw std::invalid_argument("Invalid expression.");
    }
    return res;
}
ExprNode* eval_div_expr(ExprNode *l,ExprNode *r,std::vector<Value> &record,std::vector<std::pair<std::string,DataType>> &fields){
    ExprNode *res=new ExprNode();
    if(l==nullptr||r==nullptr){
        throw std::invalid_argument("Invalid expression.");
    }
    ExprNode *lval=eval_expr(l,record,fields);
    ExprNode *rval=eval_expr(r,record,fields);
    if(lval->type==EXPR_INTNUM&&rval->type==EXPR_INTNUM){
        res->intval=lval->intval/rval->intval;
        res->type=EXPR_INTNUM;
    }
    else if(lval->type==EXPR_APPROXNUM&&rval->type==EXPR_APPROXNUM){
        res->floatval=lval->floatval/rval->floatval;
        res->type=EXPR_APPROXNUM;
    }
    else{
        throw std::invalid_argument("Invalid expression.");
    }
    return res;
}
ExprNode* eval_mod_expr(ExprNode *l,ExprNode *r,std::vector<Value> &record,std::vector<std::pair<std::string,DataType>> &fields){
    ExprNode *res=new ExprNode();
    if(l==nullptr||r==nullptr){
        throw std::invalid_argument("Invalid expression.");
    }
    ExprNode *lval=eval_expr(l,record,fields);
    ExprNode *rval=eval_expr(r,record,fields);
    if(lval->type==EXPR_INTNUM&&rval->type==EXPR_INTNUM){
        res->intval=lval->intval%rval->intval;
        res->type=EXPR_INTNUM;
    }
    else{
        throw std::invalid_argument("Invalid expression.");
    }
    return res;
}
ExprNode* eval_eq_expr(ExprNode *l,ExprNode *r,std::vector<Value> &record,std::vector<std::pair<std::string,DataType>> &fields){
    ExprNode *res=new ExprNode();
    if(l==nullptr||r==nullptr){
        throw std::invalid_argument("Invalid expression.");
    }
    ExprNode *lval=eval_expr(l,record,fields);
    ExprNode *rval=eval_expr(r,record,fields);
    if(lval->type==EXPR_INTNUM&&rval->type==EXPR_INTNUM){
        res->intval=lval->intval==rval->intval;
        res->type=EXPR_INTNUM;
    }
    else if(lval->type==EXPR_APPROXNUM&&rval->type==EXPR_APPROXNUM){
        res->intval=lval->floatval==rval->floatval;
        res->type=EXPR_INTNUM;
    }
    else if(lval->type==EXPR_STRING&&rval->type==EXPR_STRING){
        std::string left=lval->strval;
        std::string right=rval->strval;
        res->intval=left==right;
        res->type=EXPR_INTNUM;
    }
    else{
        throw std::invalid_argument("Invalid expression.");
    }
    return res;
}
ExprNode* eval_ne_expr(ExprNode *l,ExprNode *r,std::vector<Value> &record,std::vector<std::pair<std::string,DataType>> &fields){
    ExprNode *res=new ExprNode();
    if(l==nullptr||r==nullptr){
        throw std::invalid_argument("Invalid expression.");
    }
    ExprNode *lval=eval_expr(l,record,fields);
    ExprNode *rval=eval_expr(r,record,fields);
    if(lval->type==EXPR_INTNUM&&rval->type==EXPR_INTNUM){
        res->intval=lval->intval!=rval->intval;
        res->type=EXPR_INTNUM;
    }
    else if(lval->type==EXPR_APPROXNUM&&rval->type==EXPR_APPROXNUM){
        res->intval=lval->floatval!=rval->floatval;
        res->type=EXPR_INTNUM;
    }
    else if(lval->type==EXPR_STRING&&rval->type==EXPR_STRING){
        std::string left=lval->strval;
        std::string right=rval->strval;
        res->intval=left!=right;
        res->type=EXPR_INTNUM;
    }
    else{
        throw std::invalid_argument("Invalid expression.");
    }
    return res;
}
ExprNode* eval_lt_expr(ExprNode *l,ExprNode *r,std::vector<Value> &record,std::vector<std::pair<std::string,DataType>> &fields){
    ExprNode *res=new ExprNode();
    if(l==nullptr||r==nullptr){
        throw std::invalid_argument("Invalid expression.");
    }
    ExprNode *lval=eval_expr(l,record,fields);
    ExprNode *rval=eval_expr(r,record,fields);
    if(lval->type==EXPR_INTNUM&&rval->type==EXPR_INTNUM){
        res->intval=lval->intval<rval->intval;
        res->type=EXPR_INTNUM;
    }
    else if(lval->type==EXPR_APPROXNUM&&rval->type==EXPR_APPROXNUM){
        res->intval=lval->floatval<rval->floatval;
        res->type=EXPR_INTNUM;
    }
    else if(lval->type==EXPR_STRING&&rval->type==EXPR_STRING){
        std::string left=lval->strval;
        std::string right=rval->strval;
        res->intval=left<right;
        res->type=EXPR_INTNUM;
    }
    else{
        throw std::invalid_argument("Invalid expression.");
    }
    return res;
}
ExprNode* eval_gt_expr(ExprNode *l,ExprNode *r,std::vector<Value> &record,std::vector<std::pair<std::string,DataType>> &fields){
    ExprNode *res=new ExprNode();
    if(l==nullptr||r==nullptr){
        throw std::invalid_argument("Invalid expression.");
    }
    ExprNode *lval=eval_expr(l,record,fields);
    ExprNode *rval=eval_expr(r,record,fields);
    if(lval->type==EXPR_INTNUM&&rval->type==EXPR_INTNUM){
        res->intval=lval->intval>rval->intval;
        res->type=EXPR_INTNUM;
    }
    else if(lval->type==EXPR_APPROXNUM&&rval->type==EXPR_APPROXNUM){
        res->intval=lval->floatval>rval->floatval;
        res->type=EXPR_INTNUM;
    }
    else if(lval->type==EXPR_STRING&&rval->type==EXPR_STRING){
        std::string left=lval->strval;
        std::string right=rval->strval;
        res->intval=left>right;
        res->type=EXPR_INTNUM;
    }
    else{
        throw std::invalid_argument("Invalid expression.");
    }
    return res;
}
ExprNode* eval_le_expr(ExprNode *l,ExprNode *r,std::vector<Value> &record,std::vector<std::pair<std::string,DataType>> &fields){
    ExprNode *res=new ExprNode();
    if(l==nullptr||r==nullptr){
        throw std::invalid_argument("Invalid expression.");
    }
    ExprNode *lval=eval_expr(l,record,fields);
    ExprNode *rval=eval_expr(r,record,fields);
    if(lval->type==EXPR_INTNUM&&rval->type==EXPR_INTNUM){
        res->intval=lval->intval<=rval->intval;
        res->type=EXPR_INTNUM;
    }
    else if(lval->type==EXPR_APPROXNUM&&rval->type==EXPR_APPROXNUM){
        res->intval=lval->floatval<=rval->floatval;
        res->type=EXPR_INTNUM;
    }
    else if(lval->type==EXPR_STRING&&rval->type==EXPR_STRING){
        std::string left=lval->strval;
        std::string right=rval->strval;
        res->intval=left<=right;
        res->type=EXPR_INTNUM;
    }
    else{
        throw std::invalid_argument("Invalid expression.");
    }
    return res;
}
ExprNode* eval_ge_expr(ExprNode *l,ExprNode *r,std::vector<Value> &record,std::vector<std::pair<std::string,DataType>> &fields){
    ExprNode *res=new ExprNode();
    if(l==nullptr||r==nullptr){
        throw std::invalid_argument("Invalid expression.");
    }
    ExprNode *lval=eval_expr(l,record,fields);
    ExprNode *rval=eval_expr(r,record,fields);
    if(lval->type==EXPR_INTNUM&&rval->type==EXPR_INTNUM){
        res->intval=lval->intval>=rval->intval;
        res->type=EXPR_INTNUM;
    }
    else if(lval->type==EXPR_APPROXNUM&&rval->type==EXPR_APPROXNUM){
        res->intval=lval->floatval>=rval->floatval;
        res->type=EXPR_INTNUM;
    }
    else if(lval->type==EXPR_STRING&&rval->type==EXPR_STRING){
        std::string left=lval->strval;
        std::string right=rval->strval;
        res->intval=left>=right;
        res->type=EXPR_INTNUM;
    }
    else{
        throw std::invalid_argument("Invalid expression.");
    }
    return res;
}
ExprNode* eval_neg_expr(ExprNode *l,std::vector<Value> &record,std::vector<std::pair<std::string,DataType>> &fields){
    ExprNode *res=new ExprNode();
    if(l==nullptr){
        throw std::invalid_argument("Invalid expression.");
    }
    ExprNode *lval=eval_expr(l,record,fields);
    if(lval->type==EXPR_INTNUM){
        res->intval=-lval->intval;
        res->type=EXPR_INTNUM;
    }
    else if(lval->type==EXPR_APPROXNUM){
        res->floatval=-lval->floatval;
        res->type=EXPR_APPROXNUM;
    }
    else{
        throw std::invalid_argument("Invalid expression.");
    }
    return res;
}
ExprNode* eval_not_expr(ExprNode *l,std::vector<Value> &record,std::vector<std::pair<std::string,DataType>> &fields){
    ExprNode *res=new ExprNode();
    if(l==nullptr){
        throw std::invalid_argument("Invalid expression.");
    }
    ExprNode *lval=eval_expr(l,record,fields);
    if(lval->type==EXPR_INTNUM){
        res->intval=!lval->intval;
        res->type=EXPR_INTNUM;
    }
    else{
        throw std::invalid_argument("Invalid expression.");
    }
    return res;
}
ExprNode* is_in_val_list(ExprNode *l,ExprNode *r,std::vector<Value> &record,std::vector<std::pair<std::string,DataType>> &fields){
    ExprNode *res=new ExprNode(),tmp;
    res->type=EXPR_INTNUM;
    res->intval=1;
    if(l->type==EXPR_ERROR){
        throw std::invalid_argument("Invalid expression.");
    }
    ExprNode *lval=eval_expr(l,record,fields);
    ExprNode *rval=r;
    while(rval){
        if(lval->type==EXPR_INTNUM){
            if(lval->intval==rval->intval){
                return res;
            }
        }
        else if(lval->type==EXPR_APPROXNUM){
            if(lval->floatval==rval->floatval){
                return res;
            }
        }
        else if(lval->type==EXPR_STRING){
            std::string left=lval->strval;
            std::string right=rval->strval;
            if(left==right){
                return res;
            }
        }
        rval=rval->next;
    }
    res->intval=0;
    return res;
}
ExprNode* is_in_select(ExprNode *l,SelectNode *r,std::vector<Value> &record,std::vector<std::pair<std::string,DataType>> &fields){
    ExprNode *res=new ExprNode(),tmp;
    res->type=EXPR_INTNUM;
    res->intval=1;
    Table* table=do_select(r);
    std::vector<std::pair<std::string,DataType>> field=table->getFields();
    std::vector<std::vector<Value>> records=table->getRecords();
    if(field.size()!=1){
        throw std::invalid_argument("Invalid expression.");
        res->intval=0;
    }
    else{
        ExprNode *lval=eval_expr(l,record,fields);
        if(lval->type==EXPR_INTNUM){
            if(field[0].second!=DataType::INT){
                throw std::invalid_argument("Invalid expression.");
                res->intval=0;
            }
        }
        else if(lval->type==EXPR_APPROXNUM){
            if(field[0].second!=DataType::FLOAT){
                throw std::invalid_argument("Invalid expression.");
                res->intval=0;
            }
        }
        else if(lval->type==EXPR_STRING){
            if(field[0].second!=DataType::STRING){
                throw std::invalid_argument("Invalid expression.");
                res->intval=0;
            }
        }
        else{
            throw std::invalid_argument("Invalid expression.");
            res->intval=0;
        }
        for(auto &record:records){
            if(lval->type==EXPR_INTNUM){
                if(lval->intval==std::get<int>(record[0])){
                    return res;
                }
            }
            else if(lval->type==EXPR_APPROXNUM){
                if(lval->floatval==std::get<float>(record[0])){
                    return res;
                }
            }
            else if(lval->type==EXPR_STRING){
                std::string left=lval->strval;
                std::string right=std::get<std::string>(record[0]);
                if(left==right){
                    return res;
                }
            }
        }
        res->intval=0;
    }
    return res;
}
inline ExprNode *eval_expr(ExprNode*expr,std::vector<Value> &record,std::vector<std::pair<std::string,DataType>> &fields){
    if(expr == nullptr){
        return nullptr;
    }
    ExprNode *l,*r,*res;
    int flag=0;
    switch(expr->type){
        case EXPR_INTNUM:
        case EXPR_APPROXNUM:
        case EXPR_STRING:
        case EXPR_NULL:
        case EXPR_ERROR:
        case EXPR_LAZY:
            return expr;
        case EXPR_NAME:
            return make_expr_by_name(expr->strval,record,fields);
        case EXPR_ADD:
            return eval_add_expr(expr->l,expr->r,record,fields);
        case EXPR_SUB:
            return eval_sub_expr(expr->l,expr->r,record,fields);
        case EXPR_MUL:
            return eval_mul_expr(expr->l,expr->r,record,fields);
        case EXPR_DIV:
            return eval_div_expr(expr->l,expr->r,record,fields);
        case EXPR_MOD:
            return eval_mod_expr(expr->l,expr->r,record,fields);
        case EXPR_EQ:
            return eval_eq_expr(expr->l,expr->r,record,fields);
        case EXPR_NE:
            return eval_ne_expr(expr->l,expr->r,record,fields);
        case EXPR_LT:
            return eval_lt_expr(expr->l,expr->r,record,fields);
        case EXPR_GT:
            return eval_gt_expr(expr->l,expr->r,record,fields);
        case EXPR_LE:
            return eval_le_expr(expr->l,expr->r,record,fields);
        case EXPR_GE:
            return eval_ge_expr(expr->l,expr->r,record,fields);
        case EXPR_NEG:
            return eval_neg_expr(expr->l,record,fields);
        case EXPR_NOT:
            return eval_not_expr(expr->l,record,fields);
        case EXPR_NOT_IN_VAL_LIST:
            flag=1;
        case EXPR_IN_VAL_LIST:
            res=is_in_val_list(expr->l,expr->r,record,fields);
            res->intval^=flag;
            return res;
        case EXPR_NOT_IN_SELECT:
            flag=1;
        case EXPR_IN_SELECT:
            res=is_in_select(expr->l,expr->select,record,fields);
            res->intval^=flag;
            return res;
        default:
            throw std::invalid_argument("Invalid expression.");
    }
}
inline int Table::judge_cond(ExprNode *cond,std::vector<Value> &record,std::vector<std::pair<std::string,DataType>> &fields){
    if(cond == nullptr){
        return 1;
    }
    ExprNode *res = eval_expr(cond, record, fields);
    if(res==nullptr){
        throw std::invalid_argument("Condition evaluation error.");
        return 0;
    }
    else{
        return res->intval;
    }
}
void Table::update(SetNode *set,ExprNode *cond){
    std::vector<int> indexs;
    std::vector<Value> newValues;
    while(set){
        try{
            indexs.push_back(getFieldIndex(set->column));
        }
        catch(const std::out_of_range& e){
            throw std::invalid_argument("Field not found");
            return;
        }
        if(set->expr->type==EXPR_INTNUM){
            newValues.push_back(set->expr->intval);
        }
        else if(set->expr->type==EXPR_APPROXNUM){
            newValues.push_back(set->expr->floatval);
        }
        else if(set->expr->type==EXPR_STRING){
            newValues.push_back(set->expr->strval);
        }
        else{
            throw std::invalid_argument("Invalid expression.");
            return;
        }
        set=set->next;
    }
    for(size_t i=0;i<indexs.size();i++){
        if(!isTypeMatch(newValues[i],fields[indexs[i]].second)){
            throw std::invalid_argument("Type mismatch for field: " + fields[indexs[i]].first);
        }
    }
    for(auto &record:records){
        if(judge_cond(cond,record,fields)){
            for(int i=0;i<indexs.size();i++){
                record[indexs[i]]=newValues[i];
            }
        }
    }
}