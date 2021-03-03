//
// Created by Raghavasimhan Sankaranarayanan on 2/24/21.
//

#include "xArm.h"
#include <sstream>

CxArms::CxArms(const std::vector<std::string>& ip) :    m_ip(ip)
                                                        ,m_iNumArms(ip.size())
                                                        ,m_arms(m_iNumArms)
{
    for (size_t i = 0; i < m_iNumArms; ++i) {
        m_arms[i] = initArmAPI(m_ip[i]);
    }
    m_servoAngles.resize(m_iNumArms);
}

CxArms::~CxArms()
{
    if (m_pRtThread)
        m_pRtThread->join();
    reset(-1, true);
}

error_t CxArms::loadCSV(const std::string& filePath, size_t id) {
    std::ifstream inFile(filePath);
    if (!inFile.is_open())
        return STATUS_FAILURE;
    if (id >= m_iNumArms)
        return STATUS_FAILURE;

    std::vector<fp32> fAngles(N_JOINTS);

    std::string line, val;

    while (std::getline(inFile, line))
    {
        std::istringstream ss(line);
        for (int _j=0; _j<N_JOINTS; ++_j) {
            std::getline(ss, val, ',');
            fAngles[_j] = strtof(val.c_str(), nullptr);
        }

        if (id < 0) {
            for (size_t _i = 0; _i < m_iNumArms; ++_i) {
                m_servoAngles[_i].push_back(fAngles);
            }
        } else {
            m_servoAngles[id].push_back(fAngles);
        }
    }

    return STATUS_OK;
}

error_t CxArms::loadCSV(const std::vector<std::string>& filePaths) {
    for (size_t i = 0; i<filePaths.size(); ++i) {
        auto err = loadCSV(filePaths[i], i);
        if (err != STATUS_OK)
            return err;
    }
    m_iNumCommands = m_servoAngles[0].size();
    return STATUS_OK;
}

XArmAPI* CxArms::initArmAPI(const std::string& ip) {
#ifdef USE_XARM
    auto* m_pArm = new XArmAPI(ip);
    std::this_thread::sleep_for(std::chrono::milliseconds (500));
    m_pArm->clean_error();
    m_pArm->clean_warn();
    m_pArm->motion_enable(true);
    m_pArm->set_mode(0);
    m_pArm->set_state(0);

    int ret = m_pArm->set_servo_angle(m_fDefaultServoAngle, true);
    std::this_thread::sleep_for(std::chrono::milliseconds (500));
    return (ret != 0) ? m_pArm : nullptr;
#else
    return nullptr;
#endif
}

error_t CxArms::reset(int _id, bool deAlloc) {
    if (_id < 0) {
#ifdef USE_XARM
        for (auto* arm : m_arms)
        {
            arm->reset();
            if (deAlloc) {
                delete arm;
                arm = nullptr;
            }
        }
#endif
        return STATUS_OK;
    } else if (_id >= m_iNumArms) {
        return STATUS_FAILURE;
    }

#ifdef USE_XARM
    m_arms[_id]->reset();
    if (deAlloc) {
        delete m_arms[_id];
    }
#endif
    return STATUS_OK;
}

void CxArms::callback () {
    auto timeNow = std::chrono::system_clock::now();
    for (int _i = 0; _i < m_iNumCommands; ++_i) {

#ifdef USE_XARM
        for (int a = 0; a< m_iNumArms; ++a) {
            m_arms[a]->set_servo_angle(m_servoAngles[a][_i].data(), true);
        }
#else
    for (int a = 0; a < m_iNumArms; ++a) {
        for (int _j=0; _j<N_JOINTS; ++_j) {
                std::cout << m_servoAngles[a][_i][_j] << "\t";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
#endif

        std::chrono::system_clock::time_point timePoint = timeNow + std::chrono::milliseconds(4 * _i);
        std::this_thread::sleep_until(timePoint);
    }
}

error_t CxArms::startDancing() {
    m_pRtThread = new CThread(4096*8, CThread::FIFO, 80, std::bind(&CxArms::callback, this));
    if (m_pRtThread < 0)
        return STATUS_FAILURE;

    return STATUS_OK;
}
