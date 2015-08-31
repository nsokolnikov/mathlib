#include "stdafx.h"
#include <matrix.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

struct D28 : public algebra::dimension<28> {};
//Uses getline. Really slow.
std::vector<std::pair<int, algebra::matrix<D28, D28>>> old_load_mnist() {
	std::vector<std::pair<int, algebra::matrix<D28, D28>>> result;

	std::ifstream infile;
	infile = std::ifstream("mnist_train.csv");
	if (!infile) {
		std::cout<<"File didn't load";
	}

	//remove later, keeps track of progress(out of 60k)
	int counter = 0;
	std::string line = "";
	std::stringstream test;

	while (std::getline(infile, line)) {
		std::stringstream linestream(line);
		std::string word = "";

		//change to use ubyte later
		std::vector<double> data;

		//first element is tag
		std::getline(linestream, word, ',');
		int tag = atoi(word.c_str());

		while (std::getline(linestream, word, ',')) {
			data.push_back(atof(word.c_str()));
		}

		//progress counter
		if (counter % 1000 == 0) {
			test << counter << '\n';
			std::cout << test.str().c_str();
			test.str("");
		}
		++counter;


		auto result_pair = std::make_pair(tag, algebra::matrix<D28, D28>(data));
		result.push_back(result_pair);
	}
	return result;

}

//Uses C read and pointer arithmetic. Fast.
std::vector<std::pair<int, algebra::matrix<D28, D28>>> load_mnist() {
	std::vector<std::pair<int, algebra::matrix<D28, D28>>> result;
	std::ifstream::pos_type tsize;
	size_t size;

	for (int filecount = 0; filecount < 10; filecount++) {
		std::stringstream test;
		test << "data" << filecount << ".data";
		std::ifstream infile = std::ifstream(test.str(), std::ios::in | std::ios::binary | std::ios::ate);
		test.str("");
		if (infile.is_open()) {
			std::streamoff* memblock;
			double* double_arr;
			tsize = infile.tellg();
			size = static_cast<size_t>(tsize);
			memblock = new std::streamoff[size];
			infile.seekg(0, std::ios::beg);
			infile.read((char*)memblock, size);
			infile.close();

			double_arr = new double[size];
			for (size_t l = 0; l < size; l++) {
				double_arr[l] = (double)memblock[l];
			}
			for (int i = 0; i < 1000; i++) {
				std::vector<double> data(double_arr + (784 * i), double_arr + (784 * (i + 1)));
				auto result_pair = std::make_pair(filecount, algebra::matrix<D28, D28>(data));
				result.push_back(result_pair);
			}
		}
		else {
			std::cout<<"The file couldn't be read";
		}
	}
	return result;

}
