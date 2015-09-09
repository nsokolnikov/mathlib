#include "stdafx.h"
#include <matrix.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "mnist.h"

mnist_data load_mnist(
	std::wstring dataPath)
{
	mnist_data result;
	std::ifstream::pos_type tsize;
	size_t size;

	for (int filecount = 0; filecount < 10; filecount++)
	{
		std::wstringstream test;
		test << dataPath << L"\\data" << filecount << L".data";

		std::wcout << L"Reading data file '" << test.str() << L"'\r\n";

		std::ifstream infile = std::ifstream(test.str(), std::ios::in | std::ios::binary | std::ios::ate);
		test.clear();

		if (infile.is_open())
		{
			tsize = infile.tellg();
			size = static_cast<size_t>(tsize);
			infile.seekg(0, std::ios::beg);

			std::vector<char> buffer;
			buffer.resize(D784::rank);

			std::vector<double> data;
			data.resize(D784::rank);

			while (infile.tellg() < tsize)
			{
				infile.read(std::addressof(buffer[0]), buffer.size());

				std::transform(
					buffer.cbegin(), buffer.cend(),
					data.begin(),
					[](const char ch)
					{
						return ((double)(unsigned char)ch) / 255.0;
					});

				result.insert(
					result.end(),
					std::move(std::make_pair(filecount, algebra::vector<D784>(data))));
			}

			infile.close();
		}
		else
		{
			std::cout<<"The file couldn't be read";
		}
	}

	return result;
}
