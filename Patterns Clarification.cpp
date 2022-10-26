#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "tinyxml2.h"
#include "loader.h"

using namespace tinyxml2;

struct Cluster
{
    std::vector<std::vector<double>> coordinates; // координаты образа(шаблона)
    std::vector<std::vector<double>> coordinates_diff; // разность координат объекта и ядра класса
    std::vector<std::vector<double>> covariational_matrix; // матрица ковариации
    std::vector<std::vector<double>> ucS; // (x-y)^T * (S+E)^(-1) (неполное значенеи EM_distance)
    std::vector<std::vector<double>> core; // ядро клаcтера(класса)
    double EM_distance = 0.0;// (x-y)^T * (S+E)^(-1) * (x-y) - расстояние Евклида-Махаланобиса
};

//функция вычисляет часть расстояния Евклида-Махаланобиса (S+E)^-1
void countEM(std::vector<std::vector<double>>& M) 
{
    double temp;
    
    //объявляем единичную матрицу
    std::vector<std::vector<double>> E(M.size(), std::vector<double>(M.size(), 0.0));

    //определяем единичную матрицу E и складываем с матрицей ковариации S
    for (int i = 0; i < M.size(); i++)
        for (int j = 0; j < M.size(); j++)
        {
            if (i == j)  E[i][j] = 1.0;
            M[i][j] += E[i][j];
        }

    for (int a = 0; a < M.size(); a++)
    {
        temp = M[a][a];
        //все элементы a-ой строки матрицы M, кроме a-ого и до него, делим на разрешающий элемент temp матрицы M(математического ожидания)
        for (int b = a + 1; b < M.size(); b++)
            M[a][b] = M[a][b] / temp;

        //все элементы a-ой строки единичной матрицы E делим на разрешающий элемент temp матрицы M(математичсекого ожидания)
        for (int b = 0; b < M.size(); b++)
            E[a][b] = E[a][b] / temp;

        //элемент, соответствующий  разрещающему temp - делим на самого себя, чтобы получить '1'
        M[a][a] /= temp;

        if (a > 0) 
        {
            for (int i = 0; i < a; i++)
            {   
                //вычисляем элементы матрицы E(единичная матрица)
                for (int j = 0; j < M.size(); j++)
                    E[i][j] = E[i][j] - E[a][j] * M[i][a]; 
                
                //вычисляем элементы матрицы M(мат.ожидание)
                for (int j = M.size() - 1; j >= a; j--)
                    M[i][j] = M[i][j] - M[a][j] * M[i][a]; 
            }
        }
        for (int i = a + 1; i < M.size(); i++)
        {
            for (int j = 0; j < M.size(); j++)
                E[i][j] = E[i][j] - E[a][j] * M[i][a];

            for (int j = M.size() - 1; j >= a; j--)
                M[i][j] = M[i][j] - M[a][j] * M[i][a];
        }

    }

    //теперь исходная матрица - единичная, а единичная стала обратной.
    for (int i = 0; i < M.size(); i++)
        for (int b = 0; b < M.size(); b++)
            M[i][b] = E[i][b];
}


int main(int argc, char* argv[]) //argc - argumnet counter, argv - argument values
{
    //произодим загрузку входных данных( данные по кластерам(классам), образцам(шаблонам) кластеров(классов) и рассматриваемых объетам)
    for(int i=0; i<argc; i++)
        std::cout<<argv[i]<<"\n";
    if(argc<2)
    {
        std::cout << "Name of the input XML file is not specified."<<std::endl;
        return 1;
    }
    Loader loader;
    loader.load_instance(argv[1]);
    loader.print_examples();

    //определяем число..
    int clustersNum = 4;//кластеров( классов) объектов
    int clusterPatternsNum = 4;//образов(шаблонов) в каждом кластере(классе)
    int reviewedObjectsNum = 4;//рассматривемых объектов
    
    // формируем двумерный массив классов-объектов
    std::vector<std::vector<Cluster>> classesObjectsMatrix(clustersNum, std::vector<Cluster>(clusterPatternsNum)); 

    //устанавливаем размеры матрицы для каждого объекта( как для образов(шаблонов) классов(кластеров), так и для рассматриваемых объектов)
    int height = 10, width = 10;
    
    // определяем размеры матриц и значение каждого их элемента( устанавливаем 0-ое значение п.у.)
    for (int i0 = 0;i0 < clustersNum;i0++) 
        for (int j0 = 0;j0 < clusterPatternsNum;j0++) 
        {
            classesObjectsMatrix[i0][j0].coordinates.resize(height);
            classesObjectsMatrix[i0][j0].core.resize(height);
            classesObjectsMatrix[i0][j0].covariational_matrix.resize(height * width);
            classesObjectsMatrix[i0][j0].coordinates_diff.resize(height);
            classesObjectsMatrix[i0][j0].ucS.resize(height);
            classesObjectsMatrix[i0][j0].EM_distance = 0;

            for (int i1 = 0;i1 < height;i1++) 
            {
                classesObjectsMatrix[i0][j0].coordinates[i1].resize(width);
                classesObjectsMatrix[i0][j0].core[i1].resize(width);
                classesObjectsMatrix[i0][j0].coordinates_diff[i1].resize(width);
                classesObjectsMatrix[i0][j0].ucS[i1].resize(width);

                for (int j1 = 0;j1 < width;j1++) 
                {
                    classesObjectsMatrix[i0][j0].coordinates[i1][j1] = 0;
                    classesObjectsMatrix[i0][j0].core[i1][j1] = 0;
                    classesObjectsMatrix[i0][j0].ucS[i1][j1] = 0;
                    classesObjectsMatrix[i0][j0].coordinates_diff[i1][j1] = 0;
                }
            }

            //определяем матрицу коварицации
            for (int i1 = 0;i1 < height * width;i1++) 
            {
                classesObjectsMatrix[i0][j0].covariational_matrix[i1].resize(height * width);

                for (int j1 = 0;j1 < height * width;j1++) 
                    classesObjectsMatrix[i0][j0].covariational_matrix[i1][j1] = 0;
            }
        }

    // считываем образы кластеров и переопределяем их матрицы координат
    for (int step = 0; step < clustersNum; step++)
        for (int pattern = 0;pattern < clusterPatternsNum; pattern++)
            for (int i = 0; i < height; i++)
                for (int j = 0; j < width; j++)
                    classesObjectsMatrix[step][pattern].coordinates[i][j] = loader.examples[step*clustersNum + pattern][i][j];


    // вычисляем значения ядер кластеров
    for (int step = 0;step < clustersNum;step++) 
        for (int i = 0;i < height;i++) 
            for (int j = 0;j < width;j++) 
            {
                for (int pattern = 0;pattern < clusterPatternsNum;pattern++) 
                    classesObjectsMatrix[step][0].core[i][j] += classesObjectsMatrix[step][pattern].coordinates[i][j];

                classesObjectsMatrix[step][0].core[i][j] /= clusterPatternsNum;
            }

    // вычисляем матрицу ковариации S
    for (int step = 0;step < clustersNum;step++) 
    {
        for (int i = 0;i < height * width;i++) 
        {
            for (int j = 0;j < height * width;j++) 
            {
                for (int pattern = 0;pattern < clusterPatternsNum;pattern++) 
                    classesObjectsMatrix[step][0].covariational_matrix[i][j] += (classesObjectsMatrix[step][pattern].coordinates[i / height][i % height] - classesObjectsMatrix[step][0].core[i / height][i % height]) * (classesObjectsMatrix[step][0].coordinates[j / width][j % width] - classesObjectsMatrix[step][0].core[j / width][j % width]);

                classesObjectsMatrix[step][0].covariational_matrix[i][j] /= (height * width - 1);
            }
        }

        // преобразуем S (матрицу ковариации) в (S+E)^-1
        countEM(classesObjectsMatrix[step][0].covariational_matrix);
    }

    int stepVal = 0;
    //считываем образы(шаблоны) и вычисляем (x - y)^T
    for (int st = 0; st < reviewedObjectsNum; st++) 
    {
        for (int i = 0;i < height;i++) 
            for (int j = 0;j < width;j++) 
                for (int step = 0;step < clustersNum;step++) 
                    classesObjectsMatrix[step][0].coordinates_diff[i][j] = loader.tasks[st][i][j] - classesObjectsMatrix[step][0].core[i][j];
    
        for (int step = 0;step < clustersNum;step++)
            for (int i = 0;i < classesObjectsMatrix[step][0].ucS.size();i++)
                for (int j = 0;j < classesObjectsMatrix[step][0].ucS[i].size();j++)
                    classesObjectsMatrix[step][0].ucS[i][j] = 0;


        //вычисляем (x - y)^T * (S+E)^-1
        for (int i = 0;i < height * width;i++)
            for (int j = 0;j < height * width;j++)
                for (int step = 0;step < clustersNum;step++)
                    classesObjectsMatrix[step][0].ucS[i / width][i % width] += (classesObjectsMatrix[step][0].coordinates_diff[j / width][j % width] * classesObjectsMatrix[step][0].covariational_matrix[i][j]);

        //вычисляем (x - y)^T * (S+E)^-1 * (x - y) (расстояние Евклида-Махаланобиса)
        for (int i = 0;i < height;i++)
            for (int j = 0;j < width;j++)
                for (int step = 0;step < clustersNum;step++)
                    classesObjectsMatrix[step][0].EM_distance += (classesObjectsMatrix[step][0].ucS[i][j] * classesObjectsMatrix[step][0].coordinates_diff[i][j]);


        std::vector < std::pair<double, int >> best_val;

        //вычисляем sqrt((x - y)^T * (S+E)^-1 * (x - y)) (расстояние Евклида-Махаланобиса)
        for (int step = 0;step < clustersNum;step++) 
        {
            classesObjectsMatrix[step][0].EM_distance = sqrt(classesObjectsMatrix[step][0].EM_distance);
            best_val.push_back(std::make_pair(classesObjectsMatrix[step][0].EM_distance, step));
        }

        std::sort(best_val.begin(), best_val.end());

        for (int i = 0; i < 100; i++)
            std::cout << "_";
        std::cout << '\n';

        loader.print_tasks(st, loader.classes[best_val[0].second * 4]);

        //выводим данные(в виде матрциы) о ядре рассматриваемого кластера
        std::cout << "Core " << stepVal+1<<" of cluster \'"<<loader.classes[stepVal * 4] << "\':\n";
        
        for (int i = 0; i < 75; i++)
            std::cout << "_";
        std::cout << '\n';

        for (int i = 0;i < height;i++) 
        {
            for (int j = 0;j < width;j++) 
            {
                for (int pattern = 0;pattern < clusterPatternsNum;pattern++) 
                    classesObjectsMatrix[stepVal][0].core[i][j] += classesObjectsMatrix[stepVal][pattern].coordinates[i][j];

                classesObjectsMatrix[stepVal][0].core[i][j] /= clusterPatternsNum;
                std::cout << classesObjectsMatrix[stepVal][0].core[i][j] << "\t";
            }
            std::cout << '\n';
        }

        for (int i = 0; i < 75; i++)
            std::cout << "_";

        std::cout << '\n';
        if(stepVal!=clustersNum)
        ++stepVal;

        for (int step = 0;step < clustersNum;step++) 
        {
            std::cout << "Distance of cluster \'" << loader.classes[step * 4] << "\' : ";
            std::cout << classesObjectsMatrix[step][0].EM_distance << std::endl;
        }
    }
}
