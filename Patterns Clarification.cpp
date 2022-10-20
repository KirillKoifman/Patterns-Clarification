#include <iostream>
#include <vector>

struct Feature
{
	int coorI, coorJ;
};

struct Object
{	
	int numRows = 10, numCols = 10;
	std::vector< std::vector<int>> matrix;
	
	std::vector<Feature> features;
	int numFeatures = 0;

	void setMatrixSize(int _rows, int _cols)
	{
		numRows = _rows;
		numCols = _cols;
	}

	template<std::size_t rows, std::size_t cols >

	void setMatrix(int(&array)[rows][cols])
	{
		matrix.assign(numRows, std::vector<int>(numCols));

		features.resize(numRows * numCols);

		int indexX = 0;
		int indexY = 0;
		auto index = features.begin();
		for (auto i = matrix.begin(); i < matrix.end(); i++)
		{
			indexY = 0;
			for (auto j = i->begin(); j < i->end(); j++)
			{
				*j = array[indexX][indexY];
				
				if (array[indexX][indexY] == 1)
				{	
					(*index).coorI = indexX;
					(*index).coorJ = indexY;
					++numFeatures;
					++index;
				}
				++indexY;
			}
			++indexX;
		}
		
		features.resize(numFeatures);
	}

	void printMatrix()
	{
		for (auto i = matrix.begin(); i < matrix.end(); i++)
		{
			for (auto j = i->begin(); j < i->end(); j++)
			{
				std::cout << *j << " ";
			}
			std::cout << '\n';
		}
	}

};

class Clusterization
{
public:	
	std::vector<Object> clusters;
	int numClusters;

	std::vector<Object> objects;
	int numObjects;
	
	double getDistance(int x, int y)
	{
		return std::abs(x - y);
	}

	void initObjects(std::vector<Object> objects)
	{
		numObjects = objects.size();
	}

	void getClusterQuantity()
	{
		
	}
};

int main()
{	 
	int objectClass0Ethalons[2][10][10] = {
											{{0,0,0,0,0,0,0,0,0,0},
											{0,0,0,0,0,0,0,0,0,0},
											{0,0,1,1,1,1,1,0,0,0},
											{0,0,1,0,0,0,1,0,0,0},
											{0,0,1,0,0,0,1,0,0,0},
											{0,0,1,0,0,0,1,0,0,0},
											{0,0,1,0,0,0,1,0,0,0},
											{0,0,1,0,0,0,1,0,0,0},
											{0,0,1,1,1,1,1,0,0,0},
											{0,0,0,0,0,0,0,0,0,0}},
											{{0,0,0,0,0,0,0,0,0,0},
											 {0,0,0,0,0,0,0,0,0,0},
											 {0,0,1,1,1,1,1,0,0,0},
											 {0,0,1,0,0,0,1,0,0,0},
											 {0,0,1,0,0,0,1,0,0,0},
											 {0,0,1,0,0,0,1,0,0,0},
											 {0,0,1,0,0,0,1,0,0,0},
											 {0,0,1,1,0,0,1,0,0,0},
											 {0,0,1,1,1,1,1,0,0,0},
											 {0,0,0,0,0,0,0,0,0,0}}
																};
	
	int identifiableObject1[10][10] = {  {0,0,0,0,0,0,0,0,0,0},
										 {0,0,0,0,0,0,0,0,0,0},
										 {0,0,1,1,1,1,1,0,0,0},
										 {0,0,1,0,0,0,1,0,0,0},
										 {0,0,1,0,0,0,1,0,0,0},
										 {0,0,1,0,0,0,1,0,0,0},
										 {0,0,1,0,0,0,1,0,0,0},
										 {0,0,1,1,1,1,1,0,0,0},
										 {0,0,1,1,1,1,1,0,0,0},
										 {0,0,0,0,0,0,0,0,0,0} };
	/*int identifiableObject2[10][10] = { {0,0,0,0,0,0,0,0,0,0},
										 {0,0,0,0,0,0,0,0,0,0},
										 {0,0,1,1,1,1,1,0,0,0},
										 {0,0,1,0,0,0,0,0,0,0},
										 {0,0,1,0,0,0,1,0,0,0},
										 {0,0,1,0,0,0,1,0,0,0},
										 {0,0,1,0,0,0,1,0,0,0},
										 {0,0,1,0,0,0,1,0,0,0},
										 {0,0,1,1,1,1,1,0,0,0},
										 {0,0,0,0,0,0,0,0,0,0} };*/

	Object objects[2];
	for (int k = 0; k < 2; ++k)
	{
		int rows = sizeof(objectClass0Ethalons[0]) / sizeof(objectClass0Ethalons[0][0]);
		int cols = sizeof(objectClass0Ethalons[0][0]) / sizeof(objectClass0Ethalons[0][0][0]);
		objects[k].setMatrixSize(rows, cols);

		int** temp_arr = new int*[rows];
		for (int i = 0; i < rows; ++i)
			temp_arr[i] = new int[cols];

		for (int i = 0; i < rows; ++i)
			for (int j = 0; j < cols; ++j)
				temp_arr[i][j] = objectClass0Ethalons[k][i][j];

		objects[k].setMatrix(objectClass0Ethalons[k]);
		
		std::cout << "\nMatrix #"<<k + 1<<": \n";
		objects[k].printMatrix();
	}
	

}
