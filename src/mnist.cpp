#include "stdafx.h"
#include <unittest.h>
#include <matrix.h>
#include <mnist.h>
#include <string>
#include <fstream>
#include <sstream>
std::vector<std::pair<int, algebra::matrix<D28, D28>>> load_mnist() {
	std::vector<std::pair<int, algebra::matrix<D28, D28>>> result;
	
	std::ifstream infile;
	infile = std::ifstream("mnist_train.csv");
	if (!infile) {
		test::log("File didn't load");
	}

	//remove later, keeps track of progress(out of 60k)
	int counter = 0;
	std::string line = "";
	std::stringstream test;

	while (std::getline(infile, line)) {
		std::stringstream strstr(line);
		std::string word = "";

		//change to use ubyte later
		std::vector<double> data;
		std::getline(strstr, word, ',');
		int tag = atoi(word.c_str());

		while (std::getline(strstr, word, ',')) {
			data.push_back(atof(word.c_str()));
		}

		//progress counter
		if (counter % 1000 == 0) {
			test << counter << '\n';
			test::verbose(test.str().c_str());
			test.str("");
		}
		++counter;
		
		
		auto result_pair = std::make_pair(tag, algebra::matrix<D28, D28>(data));
		result.push_back(result_pair);
	}
	return result;

}