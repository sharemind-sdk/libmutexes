/*
 * Copyright (C) 2015 Cybernetica
 *
 * Research/Commercial License Usage
 * Licensees holding a valid Research License or Commercial License
 * for the Software may use this file according to the written
 * agreement between you and Cybernetica.
 *
 * GNU General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl-3.0.html.
 *
 * For further information, please contact us at sharemind@cyber.ee.
 */

#ifndef SHAREMIND_QUEUEINGMUTEX_H
#define SHAREMIND_QUEUEINGMUTEX_H

#ifdef SHAREMIND_INSTRUCT_VALGRIND
#include <cassert>
#endif
#include <tbb/queuing_mutex.h>
#ifdef SHAREMIND_INSTRUCT_VALGRIND
#include <valgrind/helgrind.h>
#endif


namespace sharemind {

class QueueingMutex {

public: /* Types: */

    class Lock {

    public: /* Types: */

        enum nolock_t { nolock };

    public: /* Methods: */

        Lock(Lock const &) = delete;
        Lock & operator=(Lock const &) = delete;

        /** \todo Implement move semantic once Intel supports it. For details,
         *        see https://software.intel.com/en-us/forums/topic/542626 */
        Lock(Lock &&) = delete;
        Lock & operator=(Lock &&) = delete;

        inline Lock(QueueingMutex & mutex) noexcept
            : m_mutex(mutex.m_mutex)
            #ifndef SHAREMIND_INSTRUCT_VALGRIND
            , m_lock(mutex.m_mutex)
            #else
            , m_isLocked(true)
            , m_lock((preLock(mutex.m_mutex), mutex.m_mutex))
            #endif
        {
            #ifdef SHAREMIND_INSTRUCT_VALGRIND
            postLock(mutex.m_mutex);
            #endif
        }

        inline Lock(QueueingMutex & mutex, const nolock_t) noexcept
            : m_mutex(mutex.m_mutex)
            #ifdef SHAREMIND_INSTRUCT_VALGRIND
            , m_isLocked(false)
            #endif
        {}

        inline ~Lock() noexcept {
            #ifdef SHAREMIND_INSTRUCT_VALGRIND
            if (m_isLocked)
                unlock();
            #endif
        }

        inline void lock() noexcept {
            #ifdef SHAREMIND_INSTRUCT_VALGRIND
            preLock(m_mutex);
            assert(!m_isLocked);
            #endif
            m_lock.acquire(m_mutex);
            #ifdef SHAREMIND_INSTRUCT_VALGRIND
            m_isLocked = true;
            postLock(m_mutex);
            #endif
        }

        inline bool try_lock() noexcept {
            #ifdef SHAREMIND_INSTRUCT_VALGRIND
            preLock(m_mutex);
            assert(!m_isLocked);
            #endif
            const bool r = m_lock.try_acquire(m_mutex);
            #ifdef SHAREMIND_INSTRUCT_VALGRIND
            if (r) {
                m_isLocked = true;
                postLock(m_mutex);
            }
            #endif
            return r;
        }

        inline void unlock() noexcept {
            #ifdef SHAREMIND_INSTRUCT_VALGRIND
            VALGRIND_HG_MUTEX_UNLOCK_PRE(&m_mutex);
            assert(m_isLocked);
            m_isLocked = false;
            #endif
            m_lock.release();
            #ifdef SHAREMIND_INSTRUCT_VALGRIND
            VALGRIND_HG_MUTEX_UNLOCK_POST(&m_mutex);
            #endif
        }

    private: /* Methods: */

        #ifdef SHAREMIND_INSTRUCT_VALGRIND
        inline static void preLock(tbb::queuing_mutex & mutex) noexcept
        { VALGRIND_HG_MUTEX_LOCK_PRE(&mutex, 0); }

        inline static void postLock(tbb::queuing_mutex & mutex) noexcept
        { VALGRIND_HG_MUTEX_LOCK_POST(&mutex); }
        #endif

    private: /* Fields: */

        tbb::queuing_mutex & m_mutex;
        #ifdef SHAREMIND_INSTRUCT_VALGRIND
        bool m_isLocked;
        #endif
        tbb::queuing_mutex::scoped_lock m_lock;

    }; /* class Lock { */

    class Guard {

    public: /* Methods: */

        inline Guard(QueueingMutex & mutex) noexcept
            #ifndef SHAREMIND_INSTRUCT_VALGRIND
            : m_lock(mutex.m_mutex)
            #else
            : m_lock(mutex)
            #endif
        {}

        inline ~Guard() noexcept {}

    private: /* Fields: */

        #ifndef SHAREMIND_INSTRUCT_VALGRIND
        tbb::queuing_mutex::scoped_lock m_lock;
        #else
        Lock m_lock;
        #endif

    };

public: /* Methods: */

    QueueingMutex(QueueingMutex &&) = delete;
    QueueingMutex(QueueingMutex const &) = delete;
    QueueingMutex & operator=(QueueingMutex &&) = delete;
    QueueingMutex & operator=(QueueingMutex const &) = delete;

    inline QueueingMutex() noexcept {
        #ifdef SHAREMIND_INSTRUCT_VALGRIND
        VALGRIND_HG_MUTEX_INIT_POST(&m_mutex, 0);
        #endif
    }

    inline ~QueueingMutex() noexcept {
        #ifdef SHAREMIND_INSTRUCT_VALGRIND
        VALGRIND_HG_MUTEX_DESTROY_PRE(&m_mutex);
        #endif
    }

private: /* Fields: */

    tbb::queuing_mutex m_mutex;

};

} /* namespace sharemind { */

#endif /* SHAREMIND_QUEUEINGMUTEX_H */
