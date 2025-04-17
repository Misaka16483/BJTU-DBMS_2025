#include <iostream>
#include"Table.h"
int main(int, char**){
    Table table("test_table");
    table.addField("id", DataType::INT);
    table.addField("name", DataType::STRING);
    table.addField("age", DataType::INT);
    table.addRecord({1, "Alice", 30});
    table.addRecord({2, "Bob", 25});
    table.addRecord({3, "Charlie", 35});
    table.addRecord({4, "David", 20});
    table.addRecord({5, "Eve", 10});
    size_t index = table.getFieldIndex("age");
    table.update("age",20,[index](const std::vector<Value>& row) {
        return std::get<int>(row[index]) > 20; // Select rows where age > 20
    });
    table.insert({"id", "name", "age"}, {6, "Frank", 28});
    auto res=table.Select();
    for(auto &v:res){
        printRecord(v);
    }
    return 0;
}
