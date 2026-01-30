#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

struct Config {
    int TEST_ITERATIONS = 0;
    int LGK = 0;
    std::vector<int> SKETCH_SIZES;
};

inline Config parseArguments(int argc, char** argv) {
    Config cfg;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if ((arg == "-i" || arg == "--iterations") && i + 1 < argc) {
            cfg.TEST_ITERATIONS = std::stoi(argv[++i]);
        } else if ((arg == "-k" || arg == "--lgk") && i + 1 < argc) {
            cfg.LGK = std::stoi(argv[++i]);
        } else if ((arg == "-s" || arg == "--sizes") && i + 1 < argc) {
            std::stringstream ss(argv[++i]);
            std::string token;
            while (std::getline(ss, token, ',')) {
                cfg.SKETCH_SIZES.push_back(std::stoi(token));
            }
        } else if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: sketch_perf -i <iters> -k <lgk> -s <sizes>\n";
            std::exit(0);
        }
    }
    return cfg;
}