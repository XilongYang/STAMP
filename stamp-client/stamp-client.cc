// Copyright (c) 2021 Xilong Yang.
// This code is licensed under MIT license (see LICENSE.txt for details)

#include "session-sender.h"
#include <getopt.h>
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::stoi;


int main(int argc, char** argv) {
//    const char* short_options = "pP:c:";
//    option long_options[] = {
//            {"passwd", no_argument, NULL, 'p'},
//            {"port", required_argument, NULL, 'P'},
//            {"count", required_argument, NULL, 'c'},
//            {0, 0, 0, 0}
//    };
//    int opt;
//    int option_index = 0;
//    while((opt = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {
//        cout << "opt = " << static_cast<char>(opt) << "    ";
//        if (optarg) {
//            cout << "optarg = " << optarg << "    ";
//        }
//        cout << "optind = " << optind << "    ";
//        cout << "argv[optind - 1]= " << argv[optind - 1] << "    ";
//        cout << "option_index= " << option_index << endl;
//    }

    SessionSender sender(20223);

    auto result = sender.start_session("127.0.0.1");

    cout << result.time.front() << endl;
    cout << result.packet_lose << endl;

    return 0;
}