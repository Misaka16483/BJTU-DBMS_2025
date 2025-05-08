#pragma once
#include"Data.h"
#include"Database.h"
#include"Table.h"
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<filesystem>
void loadTable(Table* table, std::ifstream& ifs);
void loadDatabase(std::string);
void saveTable(Table* table, std::ofstream& ofs);
void saveDatabase(Database* db);