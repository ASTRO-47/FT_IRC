#include "utils.hpp"

void toLower(std::string& input){
    for (size_t i = 0; i < input.size(); i++)
        input[i] = std::tolower(static_cast<unsigned char>(input[i]));
}