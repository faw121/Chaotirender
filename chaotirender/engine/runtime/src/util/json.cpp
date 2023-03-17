#include <runtime/util/json.h>

#include <fstream>
#include <iostream>

namespace Chaotirender
{
    void parseJSONFile(std::string json_file, nlohmann::json& j)
    {
        std::ifstream in_stream;
        in_stream.open(json_file);
        if (!in_stream.is_open())
        {
            std::cout << "fail to open file: " << json_file << "\n";
            return;
        }
        in_stream >> j;
        in_stream.close();
    }
}