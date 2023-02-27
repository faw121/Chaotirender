#pragma once

#include <iostream>

#include <glm/gtx/string_cast.hpp>

#define LOG_VEC(vec, info) std::cout << #info ": " << glm::to_string(vec) << std::endl;