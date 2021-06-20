/*
 * @Author: Ou Yixin
 * @Date: 2021-06-09 23:25:37
 * @LastEditors: Ou Yixin
 * @LastEditTime: 2021-06-20 11:50:02
 * @Description: 
 * @FilePath: /MiniSQL/API/API.cpp
 */
#include "API.hpp"

MiniSQL::MiniSQL()
{
    
}

MiniSQL::~MiniSQL()
{

}

void API::createTable(const std::string &tableName, const std::vector<Column> &columns, const std::string &primaryKey)
{
    auto CM = API::getCatalogManager();
    auto RM = API::getRecordManager();

    if (CM->existTable(tableName))
    {
        std::cout << "ERROR : You have an error in your SQL syntax; tabel named " << tableName << " already exists.\n";
        return ;
    }

    CM->newTable(tableName, columns);
    Table &table = CM->getTable(tableName);
    RM->CreateTable(table);
}

void API::createIndex(const std::string &indexName, const std::string &tableName, const std::string &columnName)
{
    auto CM = API::getCatalogManager();
    auto RM = API::getRecordManager();
    
    if (CM->existIndex(indexName))
    {
        std::cout << "ERROR : You have an error in your SQL syntax; index named " << indexName << " already exists.\n";
        return ;
    }

    Index &index = CM->getIndex(indexName);
    Table &table = CM->getTable(tableName);

    CM->newIndex(indexName, tableName, columnName);
    RM->im->CreateIndex(indexName, table, columnName);
}

void API::dropTable(const std::string &tableName)
{    
    auto CM = API::getCatalogManager();
    auto RM = API::getRecordManager();

    if (!CM->existTable(tableName))
    {
        std::cout << "ERROR : You have an error in your SQL syntax; tabel named " << tableName << " doesn't exist.\n";
        return ;
    }

    Table &table = CM->getTable(tableName);
    RM->DropTable(table);
    CM->dropTable(tableName);
}

void API::dropIndex(const std::string &indexName)
{
    auto CM = API::getCatalogManager();
    auto RM = API::getRecordManager();

    if (!CM->existIndex(indexName))
    {
        std::cout << "ERROR : You have an error in your SQL syntax; index named " << indexName << " doesn't exist.\n";
        return ;
    }
    
    Index &index = CM->getIndex(indexName);
    Table &table = CM->getTable(index.tableName);

    CM->dropIndex(indexName);
    RM->im->DropIndex(indexName, table, index.columnName);
}

void API::insertOn(const std::string &tableName, std::vector<Value> &valueList)
{
    auto CM = API::getCatalogManager();
    auto RM = API::getRecordManager();

    if (!CM->existTable(tableName))
    {
        std::cout << "ERROR : You have an error in your SQL syntax; tabel named " << tableName << " doesn't exist.\n";
        return ;
    }

    Table &table = CM->getTable(tableName);

    int valueNum = valueList.size();
    int columnNum = table.columns.size();
    if (valueNum != columnNum)
    {
        std::cout << "ERROR : You have an error in your SQL syntax; tabel named " << tableName << " has " << columnNum << " columns but " << valueNum << " values are given.\n";
        return ;
    }

    RM->InsertRecord(table, valueList);
}

void API::deleteFrom(const std::string &tableName, std::vector<Condition> &conditionList)
{
    auto CM = API::getCatalogManager();
    auto RM = API::getRecordManager();

    if (!CM->existTable(tableName))
    {
        std::cout << "ERROR : You have an error in your SQL syntax; tabel named " << tableName << " doesn't exist.\n";
        return ;
    }

    Table &table = CM->getTable(tableName);
    for (const auto & cond : conditionList)
    {
        if (!table.hasColumn(cond.columnName))
        {
            std::cout << "ERROR : You have an error in your SQL syntax; tabel named " << tableName << " doesn't have a column named "<< cond.columnName << ".\n";
            return ;
        }
    }

    RM->DeleteRecord(table, conditionList);
}

void API::deleteAll(const std::string &tableName)
{
    auto CM = API::getCatalogManager();
    auto RM = API::getRecordManager();

    if (!CM->existTable(tableName))
    {
        std::cout << "ERROR : You have an error in your SQL syntax; tabel named " << tableName << " doesn't exist.\n";
        return ;
    }

    Table &table = CM->getTable(tableName);

    RM->DeleteAllRecord(table);
}

void API::select(const std::string &tableName, std::vector<Condition> &conditionList)
{
    auto CM = API::getCatalogManager();
    auto RM = API::getRecordManager();

    if (!CM->existTable(tableName))
    {
        std::cout << "ERROR : You have an error in your SQL syntax; tabel named " << tableName << " doesn't exist.\n";
        return ;
    }

    Table &table = CM->getTable(tableName);
    for (const auto & cond : conditionList)
    {
        if (!table.hasColumn(cond.columnName))
        {
            std::cout << "ERROR : You have an error in your SQL syntax; tabel named " << tableName << " doesn't have a column named "<< cond.columnName << ".\n";
            return ;
        }
    }

    std::vector<ValueVec> result = RM->SelectRecord(table, conditionList);
    
    std::cout << std::left;  //设置对齐方式为左对齐，默认为右对齐
    //std::cout << setfill(' ');  //设置空位置填充符号为空格，默认即为空格

    for (const auto &column : table.columns)
    {
        std::cout << std::setw(10) << column.columnName;
    }
    std::cout << "\n";

    for (int i = 0; i < result.size(); ++i)
    {
        for (int j = 0; j < result.at(i).size(); ++j)
        {
            switch (table.columns[j].field)
            {
                case Field::INT:    std::cout << std::setw(10) << std::get<int>(result[i][j]);break;
                case Field::FLOAT:  std::cout << std::setw(10) << std::get<double>(result[i][j]);break;
                case Field::CHAR:   std::cout << std::setw(10) << std::get<std::string>(result[i][j]);break;
            }
        }
        std::cout << "\n";
    }
}

void API::selectAll(const std::string &tableName)
{
    auto CM = API::getCatalogManager();
    auto RM = API::getRecordManager();

    if (!CM->existTable(tableName))
    {
        std::cout << "ERROR : You have an error in your SQL syntax; tabel named " << tableName << " doesn't exist.\n";
        return ;
    }

    Table &table = CM->getTable(tableName);

    std::vector<ValueVec> result = RM->SelectAllRecord(table);
    
    std::cout << std::left;  //设置对齐方式为左对齐，默认为右对齐
    //std::cout << setfill(' ');  //设置空位置填充符号为空格，默认即为空格

    for (const auto &column : table.columns)
    {
        std::cout << std::setw(10) << column.columnName;
    }
    std::cout << "\n";

    for (int i = 0; i < result.size(); ++i)
    {
        for (int j = 0; j < result.at(i).size(); ++j)
        {
            switch (table.columns[j].field)
            {
                case Field::INT:    std::cout << std::setw(10) << std::get<int>(result[i][j]);break;
                case Field::FLOAT:  std::cout << std::setw(10) << std::get<double>(result[i][j]);break;
                case Field::CHAR:   std::cout << std::setw(10) << std::get<std::string>(result[i][j]);break;
            }
        }
        std::cout << "\n";
    }
}

void API::endLoop()
{
    delete rm;
    delete cm;
    exit(0);
}

CatalogManager *API::getCatalogManager()
{
    if (cm == NULL) cm = new CatalogManager();
    return cm;
}

RecordManager *API::getRecordManager()
{
    if (rm == NULL) rm = new RecordManager();
    return rm;
}