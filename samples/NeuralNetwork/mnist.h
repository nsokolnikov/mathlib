#pragma once
#include "stdafx.h"
#include <matrix.h>
struct D28 : public algebra::dimension<28> {};
std::vector<std::pair<int, algebra::matrix<D28, D28>>> load_mnist();
