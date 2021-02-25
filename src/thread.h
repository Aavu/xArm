
//
// Created by Raghavasimhan Sankaranarayanan on 2/24/21.
//

#ifndef XARM_THREAD_H
#define XARM_THREAD_H

#include <iostream>
#include <future>
#include <functional>
#include <thread>
#include <pthread.h>
#include <sched.h>
#include <sys/mman.h>
#include <climits>
#include "type_traits"
#include "defines.h"

class CThread {
public:
    enum sched_policy_t {
        Other = SCHED_OTHER,
        FIFO = SCHED_FIFO,
        RR = SCHED_RR
    };

    typedef std::function<void()> functor_t;

    // Refer : https://stackoverflow.com/questions/52944309/passing-variadic-template-to-pthread-create
    template<typename Callable, typename... Args>
    explicit CThread(size_t stack_size, sched_policy_t policy, int priority, Callable && func, Args &&... args) {
        auto task_ptr = std::make_shared<std::packaged_task<decltype(func(args...))()>>(
                std::bind(std::forward<Callable>(func), std::forward<Args>(args)...)
        );

        auto* functor = new functor_t([task_ptr]() {
            (*task_ptr)();
        });

        pthread_attr_t attr {0};
        if (_set_attributes(attr, stack_size, policy, priority) == STATUS_OK)
            m_bJoinable = pthread_create(&m_id, &attr, &run, functor) == STATUS_OK;

        pthread_attr_destroy(&attr);
    }

    bool joinable() const { return m_bJoinable; }

    void join() const {
        if (joinable())
            pthread_join(m_id, nullptr);
    }

private:
    pthread_t m_id = -1;
    bool m_bJoinable = false;

    // Refer: https://wiki.linuxfoundation.org/realtime/documentation/howto/applications/application_base
    static error_t _set_attributes(pthread_attr_t& attr, size_t stack_size, sched_policy_t policy, int priority) {
        if (mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
            std::cerr << "mlockall failed" << std::endl;
            return STATUS_FAILURE;
        }

        // Initialize pthread attributes (default values)
        auto ret = pthread_attr_init(&attr);
        if (ret != STATUS_OK) {
            std::cerr << "init pthread attributes failed\n";
            return ret;
        }

        // Set a specific stack size
        ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN + stack_size);
        if (ret) {
            std::cerr << "pthread setstacksize failed\n";
            return ret;
        }

        // Set scheduler policy and priority of pthread
        ret = pthread_attr_setschedpolicy(&attr, (int)policy);
        if (ret) {
            std::cerr << "pthread setschedpolicy failed\n";
            return ret;
        }

        sched_param param {};
        param.sched_priority = 80;
        ret = pthread_attr_setschedparam(&attr, &param);
        if (ret) {
            std::cerr << "pthread setschedparam failed\n";
            return ret;
        }

        // Use scheduling parameters of attr
        ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        if (ret) {
            std::cerr << "pthread setinheritsched failed\n";
            return ret;
        }

        return STATUS_OK;
    }

    static void* run(void* f)
    {
        auto* functor = (functor_t*)f;
        (*functor)();
        delete functor;
        return nullptr;
    }
};


#endif //XARM_THREAD_H
