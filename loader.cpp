#include <iostream>
#include <vector>
#include "tinyxml2.h"

using namespace tinyxml2;
class Loader
{
public:
    std::vector<std::vector<std::vector<int>>> examples;
    std::vector<int> classes; //каждый элементы вектора - id класса сответсвующего образца
    std::vector<std::vector<std::vector<int>>> tasks;
    bool load_instance(const char* fileName)
    {
        XMLDocument doc;
        if(doc.LoadFile(fileName) != XMLError::XML_SUCCESS)
        {
            std::cout << "Error openning input XML file."<<std::endl;
            return false;
        }
        XMLElement* root;
        root = doc.FirstChildElement("root");
        XMLElement* objects = root->FirstChildElement("examples");
        for(auto object=objects->FirstChildElement("object"); object; object = object->NextSiblingElement("object"))
        {
            std::vector<std::vector<int>> example;
            classes.push_back(object->IntAttribute("class"));
            for(auto row = object->FirstChildElement("row"); row; row = row->NextSiblingElement("row"))
            {
                std::vector<int> line;
                std::string values = row->GetText();
                for(char value : values)
                {
                    if(value == '1')
                        line.push_back(1);
                    else if(value == '0')
                        line.push_back(0);
                }
                example.push_back(line);
            }
            examples.push_back(example);
        }
        XMLElement* task = root->FirstChildElement("tasks");
        for(auto object=task->FirstChildElement(); object; object = object->NextSiblingElement("object"))
        {
            std::vector<std::vector<int>> example;
            for(auto row = object->FirstChildElement("row"); row; row = row->NextSiblingElement("row"))
            {
                std::vector<int> line;
                std::string values = row->GetText();
                for(char value : values)
                {
                    if(value == '1')
                        line.push_back(1);
                    else if(value == '0')
                        line.push_back(0);
                }
                example.push_back(line);
            }
            tasks.push_back(example);
        }
        return true;
    }
    void print()
    {
        for(int i = 0; i < examples.size(); i++)
        {
            std::cout<<"\nObject "<<i<<" class="<<classes[i]<<"\n";
            for(int j=0; j<examples[i].size(); j++) {
                for (int k = 0; k < examples[i][j].size(); k++)
                    std::cout<<examples[i][j][k]<<" ";
                std::cout<<"\n";
            }
        }
        for(int i = 0; i < tasks.size(); i++)
        {
            std::cout<<"\nTask "<<i<<" class=Unknown\n";
            for(int j=0; j<tasks[i].size(); j++) {
                for (int k = 0; k < tasks[i][j].size(); k++)
                    std::cout<<tasks[i][j][k]<<" ";
                std::cout<<"\n";
            }
        }
    }
};

int main(int argc, char* argv[]) //argc - argumnet counter, argv - argument values
{
    for(int i=0; i<argc; i++)
        std::cout<<argv[i]<<"\n";
    if(argc<2)
    {
        std::cout << "Name of the input XML file is not specified."<<std::endl;
        return 1;
    }
    Loader loader;
    loader.load_instance(argv[1]);
    loader.print();
    return 0;
}