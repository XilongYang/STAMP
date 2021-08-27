// Copyright (c) 2021 Xilong Yang.
// This code is licensed under MIT license (see LICENSE.txt for details)

#include "session-reflector.h"

#include <iostream>
#include <string>

using std::cout;
using std::endl;

int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "Usage: stamp-server [port]" << endl;
        return -1;
    }

    SessionReflector reflector(std::stoi(argv[1]), true, true);
    reflector.start_session();

    return 0;
}
