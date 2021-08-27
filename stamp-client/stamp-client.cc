// Copyright (c) 2021 Xilong Yang.
// This code is licensed under MIT license (see LICENSE.txt for details)

#include "session-sender.h"
#include <getopt.h>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    const char* short_options = "pP:c:";
    option long_options[] = {
            {"passwd", no_argument, NULL, 'p'},
            {"port", required_argument, NULL, 'P'},
            {"count", required_argument, NULL, 'c'},
            {0, 0, 0, 0}
    };

    SessionSender sender;

    int opt;
    while((opt = getopt_long(argc, argv, short_options, long_options, nullptr)) != -1) {
        switch (opt) {
            case 'c':
                if (optarg == nullptr) {
                    std::cerr << "Usage: stamp-client [-c count -P port -p] address." << std::endl;
                    return -1;
                }
                sender.count_ = std::stoul(optarg);
                break;
            case 'P':
                if (optarg == nullptr) {
                    std::cerr << "Usage: stamp-client [-c count -P port -p] address." << std::endl;
                    return -1;
                }
                sender.port_ = std::stoi(optarg);
                break;
            case 'p':
                sender.set_auth_mode(true);
                break;
            default:
                return -1;
        }
    }

    if (argc - 1 != optind) {
        std::cerr << "Usage: stamp-client [-c count -P port -p] address." << std::endl;
        return -1;
    }

    auto result = sender.start_session(argv[optind]);
    for (auto t : result.time) {
        std::cout << t << std::endl;
    }
    std::cout << result.packet_lose << std::endl;

    return 0;
}