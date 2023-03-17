#pragma once

#include <json.hpp>

namespace Chaotirender
{
    void parseJSONFile(std::string json_file, nlohmann::json& j);
}