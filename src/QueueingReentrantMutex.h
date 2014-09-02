/*
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_QUEUEINGREENTRANTMUTEX_H
#define SHAREMIND_QUEUEINGREENTRANTMUTEX_H

#include <cassert>
#include <condition_variable>
#include <cstddef>
#include <thread>
#include "QueueingMutex.h"


namespace sharemind {

class QueueingReentrantMutex {

public: /* Types: */

    class Lock {

    public: /* Types: */

        enum nolock_t { nolock };

    public: /* Methods: */

        inline Lock(QueueingReentrantMutex & mutex) noexcept
            : m_mutex(mutex)
            , m_lock(mutex.m_mutex, QueueingMutex::Lock::nolock)
        { lock__(mutex); }

        inline Lock(QueueingReentrantMutex & mutex, const nolock_t) noexcept
            : m_mutex(mutex)
            , m_lock(mutex.m_mutex, QueueingMutex::Lock::nolock)
            , m_locked(false)
        {}

        inline ~Lock() noexcept {
            if (m_locked)
                unlock();
        }

        inline void lock() noexcept {
            assert(!m_locked);
            lock__(m_mutex);
        }

        inline bool try_lock() noexcept {
            assert(!m_locked);
            const std::thread::id myId = std::this_thread::get_id();
            m_lock.lock();
            if (m_mutex.m_locked && m_mutex.m_owner != myId) {
                m_lock.unlock();
                return false;
            }
            m_mutex.m_locked = true;
            m_mutex.m_owner = myId;
            ++m_mutex.m_counter;
            m_lock.unlock();
            m_locked = true;
            return true;
        }

        inline void unlock() noexcept {
            assert(m_locked);
            m_lock.lock();
            assert(m_mutex.m_locked);
            assert(m_mutex.m_owner == std::this_thread::get_id());
            assert(m_mutex.m_counter > 0u);
            if (--m_mutex.m_counter == 0u) {
                m_locked = false;
                m_mutex.m_cond.notify_all();
            }
            m_lock.unlock();
        }

    private: /* Methods: */

        inline void lock__(QueueingReentrantMutex & mutex) noexcept {
            const std::thread::id myId = std::this_thread::get_id();
            m_lock.lock();
            while (mutex.m_locked && mutex.m_owner != myId)
                mutex.m_cond.wait(m_lock);
            mutex.m_locked = true;
            mutex.m_owner = myId;
            ++mutex.m_counter;
            m_lock.unlock();
            m_locked = true;
        }

    private: /* Fields: */

        QueueingReentrantMutex & m_mutex;
        QueueingMutex::Lock m_lock;
        bool m_locked = false;

    }; /* class Lock { */

    class Guard {

    public: /* Methods: */

        inline Guard(QueueingReentrantMutex & mutex) noexcept
            : m_mutex(mutex)
            , m_lock(mutex.m_mutex, QueueingMutex::Lock::nolock)
        {
            const std::thread::id myId = std::this_thread::get_id();
            m_lock.lock();
            while (mutex.m_locked && mutex.m_owner != myId)
                mutex.m_cond.wait(m_lock);
            mutex.m_locked = true;
            mutex.m_owner = myId;
            ++mutex.m_counter;
            m_lock.unlock();
        }

        inline ~Guard() noexcept {
            m_lock.lock();
            assert(m_mutex.m_locked);
            assert(m_mutex.m_owner == std::this_thread::get_id());
            assert(m_mutex.m_counter > 0u);
            if (--m_mutex.m_counter == 0u) {
                m_mutex.m_locked = false;
                m_mutex.m_cond.notify_all();
            }
            m_lock.unlock();
        }

    private: /* Fields: */

        QueueingReentrantMutex & m_mutex;
        QueueingMutex::Lock m_lock;

    };
    friend class Guard;

public: /* Methods: */

    inline QueueingReentrantMutex() noexcept {}

    inline ~QueueingReentrantMutex() noexcept {}

private: /* Fields: */

    QueueingMutex m_mutex;
    std::condition_variable_any m_cond;
    bool m_locked = false;
    std::thread::id m_owner;
    std::size_t m_counter = 0u;

}; /* class QueueingReentrantMutex { */

} /* namespace sharemind { */

#endif /* SHAREMIND_QUEUEINGREENTRANTMUTEX_H */
