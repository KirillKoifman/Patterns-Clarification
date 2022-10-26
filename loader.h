#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "tinyxml2.h"

using namespace tinyxml2;
class Loader
{
public:
    // каждый элемент вектора - номер определённого образца(шаблона) кластера(класса)
    std::vector<std::vector<std::vector<int>>> examples;

    //каждый элемент вектора - номер кластера(класса) сответсвующего образца(шаблона)
    std::vector<std::string> classes;

    // каждый элемент вектора - номер рассматривемого объекта
    std::vector<std::vector<std::vector<int>>> tasks;

    bool load_instance(const char* fileName)
    {
        XMLDocument doc;
        if (doc.LoadFile(fileName) != XMLError::XML_SUCCESS)
        {
            std::cout << "Error openning input XML file." << std::endl;
            return false;
        }
        XMLElement* root;
        root = doc.FirstChildElement("root");
        XMLElement* objects = root->FirstChildElement("examples");
        for (auto object = objects->FirstChildElement("object"); object; object = object->NextSiblingElement("object"))
        {
            std::vector<std::vector<int>> example;
            classes.push_back(object->Attribute("class"));
            for (auto row = object->FirstChildElement("row"); row; row = row->NextSiblingElement("row"))
            {
                std::vector<int> line;
                std::string values = row->GetText();
                for (char value : values)
                {
                    if (value == '1')
                        line.push_back(1);
                    else if (value == '0')
                        line.push_back(0);
                }
                example.push_back(line);
            }
            examples.push_back(example);
        }
        XMLElement* task = root->FirstChildElement("tasks");
        for (auto object = task->FirstChildElement(); object; object = object->NextSiblingElement("object"))
        {
            std::vector<std::vector<int>> example;
            for (auto row = object->FirstChildElement("row"); row; row = row->NextSiblingElement("row"))
            {
                std::vector<int> line;
                std::string values = row->GetText();
                for (char value : values)
                {
                    if (value == '1')
                        line.push_back(1);
                    else if (value == '0')
                        line.push_back(0);
                }
                example.push_back(line);
            }
            tasks.push_back(example);
        }
        return true;
    }

    //функция, которая выводит изображение каждого образца(шаблона) кластера(класса) в виде матрицы с 2-мя типами символов
    void print_examples()
    {
        for (int i = 0; i < examples.size(); i++)
        {
            std::cout << "\nObject " << i + 1 << " class=" << classes[i] << "\n";
            for (int j = 0; j < examples[i].size(); j++) {
                for (int k = 0; k < examples[i][j].size(); k++)
                    if (examples[i][j][k] == 0) std::cout << '*';
                    else std::cout << '@';

                std::cout << "\n";
            }
        }
    }

    //функция, которая выводит изображение каждого рассматриваемого в виде матрицы с 2-мя типами символов
    void print_tasks(int i, std::string className)
    {
        std::cout << "\nTask " << i + 1 << " supposed class = " << className << "\n";
        for (int j = 0; j < tasks[i].size(); j++)
        {
            for (int k = 0; k < tasks[i][j].size(); k++)
                if (tasks[i][j][k] == 0) std::cout << '*';
                else std::cout << '@';

            std::cout << "\n";
        }
    }
};