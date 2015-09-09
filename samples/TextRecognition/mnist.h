#pragma once

#include "stdafx.h"
#include <matrix.h>

struct D784 : public algebra::dimension<784> {};

typedef std::pair<int, algebra::vector<D784>> mnist_digit;
typedef std::vector<mnist_digit> mnist_data;

mnist_data load_mnist(std::wstring dataPath);
