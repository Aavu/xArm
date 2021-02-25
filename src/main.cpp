//
// Created by Raghavasimhan Sankaranarayanan on 2/24/21.
//

#include <iostream>
#include <vector>
#include "defines.h"
#include "xArm.h"

int main(int argc, char const *argv[])
{
    std::vector<std::string> IPs {"192.168.1.236", "192.168.1.204"};
    CxArms xArms(IPs);

    std::vector<std::string> csvFiles {"../../forest1a.csv", "../../forest1b.csv"};
    int err = xArms.loadCSV(csvFiles);
    if (err != STATUS_OK) {
        std::cerr << "Error loading CSV files" << std::endl;
        return err;
    }

    xArms.startDancing();
}
