#include <iostream>

#include "processor/DocumentProcessor.h"

int main(int argc, char* argv[]) {
    DocumentProcessor processor(ParcerFactory::GetInstance());
    if (argc < 2) {
        std::cout << "File name required\n";
        return 0;
    }
    std::vector<std::string> filters;
    for (auto i = 2; i < argc; i++) {
        filters.push_back(std::string(argv[i]));
    }
    auto info = processor.Process(
        argv[1],
        filters);
    return 0;
}