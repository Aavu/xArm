//
// Created by Raghavasimhan Sankaranarayanan on 2/24/21.
//

#ifndef XARM_XARM_H

#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>

#include "thread.h"
#include "defines.h"

#ifdef USE_XARM
#include <xarm/wrapper/xarm_api.h>
#else
typedef float fp32;
typedef void XArmAPI;
#endif



class CxArms
{
public:
    /// Constructor of the xArm array
    /// \param ip A vector of ip addresses of each arm. The index of each ip is assigned as the id of that arm.
    explicit CxArms(const std::vector<std::string>& ip);

    /// House keeping happens here
    ~CxArms();

    /// Load the csv data onto memory.
    /// \param filePath The file path to the csv data
    /// \param id id of the arm to assign the data to. If id < 0, then the data is copied to all the arms
    /// \return Status
    error_t loadCSV(const std::string& filePath, size_t id = -1);

    /// Load the csv data onto memory.
    /// \param filePaths The file paths of the csv data corresponding to each arm
    /// \return Status
    error_t loadCSV(const std::vector<std::string>& filePaths);

    /// Start the RT thread to move to the positions
    error_t startDancing();

    void callback();

private:
    // The ip addresses of the arms
    std::vector<std::string> m_ip;

    // Number of Arms
    size_t m_iNumArms = 0;

    // The default servo angles of the joints
    fp32 m_fDefaultServoAngle[N_JOINTS] = {0.0, 0.0, 0.0, 1.5708, 0.0, 1.5708, 0.0};

    // The angles of the N-joints for each command for each arm
    std::vector<std::vector<std::vector<fp32>>> m_servoAngles; // angles of each arm, each line

    // Number of commands in the csv file
    size_t m_iNumCommands = 0;

    // Container to hold references to the arms that are initialized. The Index correspond to the IDs of the arms.
    std::vector<XArmAPI *> m_arms;

    // The thread that handles the realtime process
    CThread* m_pRtThread = nullptr;

    /// Initialize each arm as return a pointer to the XArmAPI object
    /// \param ip The Ip address of the arm
    /// \return Pointer to the XArmAPI object. Nullptr is returned of there is any error in initialization.
    XArmAPI* initArmAPI(const std::string& ip);

    /// Reset the arm
    /// \param _id The arm id
    /// \param deAlloc Whether to release allocated memory
    /// \return Status
    error_t reset(int _id = -1, bool deAlloc = false); // if id is < 0, reset all
};

#endif // XARM_XARM_H