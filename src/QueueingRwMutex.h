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

#ifndef SHAREMIND_QUEUEINGRWMUTEX_H
#define SHAREMIND_QUEUEINGRWMUTEX_H

#ifdef SHAREMIND_INSTRUCT_VALGRIND
#include <cassert>
#endif
#include <tbb/queuing_rw_mutex.h>
#ifdef SHAREMIND_INSTRUCT_VALGRIND
#include <valgrind/helgrind.h>
#endif


namespace sharemind {

class QueueingRwMutex {

public: /* Types: */

    template <bool UNIQUE__>
    class LockBase {

    public: /* Types: */

        enum nolock_t { nolock };

    public: /* Methods: */

        inline LockBase(QueueingRwMutex & mutex) noexcept
            : m_mutex(mutex.m_mutex)
            #ifdef SHAREMIND_INSTRUCT_VALGRIND
            , m_isUnique(UNIQUE__)
            , m_isLocked(true)
            #endif
            , m_lock(mutex.m_mutex, UNIQUE__)
        {
            #ifdef SHAREMIND_INSTRUCT_VALGRIND
            ANNOTATE_RWLOCK_ACQUIRED(&m_mutex, UNIQUE__);
            #endif
        }

        inline LockBase(QueueingRwMutex & mutex, const nolock_t) noexcept
            : m_mutex(mutex.m_mutex)
            #ifdef SHAREMIND_INSTRUCT_VALGRIND
            , m_isUnique(UNIQUE__)
            , m_isLocked(false)
            #endif
        {}

        inline ~LockBase() noexcept {
            #ifdef SHAREMIND_INSTRUCT_VALGRIND
            if (m_isLocked)
                unlock();
            #endif
        }

        inline void lock() noexcept {
            m_lock.acquire(m_mutex, UNIQUE__);
            #ifdef SHAREMIND_INSTRUCT_VALGRIND
            assert(m_isUnique == UNIQUE__);
            assert(!m_isLocked);
            m_isLocked = true;
            ANNOTATE_RWLOCK_ACQUIRED(&m_mutex, UNIQUE__);
            #endif
        }

        inline bool try_lock() noexcept {
            const bool r = m_lock.try_acquire(m_mutex, UNIQUE__);
            #ifdef SHAREMIND_INSTRUCT_VALGRIND
            if (r) {
                assert(m_isUnique == UNIQUE__);
                assert(!m_isLocked);
                m_isLocked = true;
                ANNOTATE_RWLOCK_ACQUIRED(&m_mutex, UNIQUE__);
            }
            #endif
            return r;
        }

        inline void unlock() noexcept {
            #ifdef SHAREMIND_INSTRUCT_VALGRIND
            ANNOTATE_RWLOCK_RELEASED(&m_mutex, m_isUnique);
            m_isUnique = UNIQUE__;
            m_isLocked = false;
            #endif
            m_lock.release();
        }

        inline bool upgrade_to_writer() noexcept {
            #ifdef SHAREMIND_INSTRUCT_VALGRIND
            assert(m_isLocked);
            assert(!m_isUnique);
            ANNOTATE_RWLOCK_RELEASED(&m_mutex, false);
            #endif
            const bool r = m_lock.upgrade_to_writer();
            #ifdef SHAREMIND_INSTRUCT_VALGRIND
            m_isUnique = true;
            ANNOTATE_RWLOCK_ACQUIRED(&m_mutex, true);
            #endif
            return r;
        }

        inline bool downgrade_to_reader() noexcept {
            #ifdef SHAREMIND_INSTRUCT_VALGRIND
            assert(m_isLocked);
            assert(m_isUnique);
            ANNOTATE_RWLOCK_RELEASED(&m_mutex, true);
            #endif
            const bool r = m_lock.downgrade_to_reader();
            #ifdef SHAREMIND_INSTRUCT_VALGRIND
            m_isUnique = false;
            ANNOTATE_RWLOCK_ACQUIRED(&m_mutex, false);
            #endif
            return r;
        }

    private: /* Fields: */

        tbb::queuing_rw_mutex & m_mutex;
        #ifdef SHAREMIND_INSTRUCT_VALGRIND
        bool m_isUnique;
        bool m_isLocked;
        #endif
        tbb::queuing_rw_mutex::scoped_lock m_lock;

    }; /* template <bool UNIQUE__> class LockBase { */

    typedef LockBase<true> UniqueLock;
    typedef LockBase<false> SharedLock;

    template <bool UNIQUE__>
    class GuardBase {

    public: /* Methods: */

        inline GuardBase(QueueingRwMutex & mutex) noexcept
            #ifndef SHAREMIND_INSTRUCT_VALGRIND
            : m_lock(mutex.m_mutex, UNIQUE__)
            #else
            : m_lock(mutex)
            #endif
        {}

        inline ~GuardBase() noexcept {}

        inline bool upgrade_to_writer() noexcept
        { return m_lock.upgrade_to_writer(); }

        inline bool downgrade_to_reader() noexcept
        { return m_lock.downgrade_to_reader(); }

    private: /* Fields: */

        #ifndef SHAREMIND_INSTRUCT_VALGRIND
        tbb::queuing_rw_mutex::scoped_lock m_lock;
        #else
        LockBase<UNIQUE__> m_lock;
        #endif

    }; /* template <bool UNIQUE__> class GuardBase { */

    typedef GuardBase<true> UniqueGuard;
    typedef GuardBase<false> SharedGuard;

public: /* Methods: */

    QueueingRwMutex(QueueingRwMutex &&) = delete;
    QueueingRwMutex(QueueingRwMutex const &) = delete;
    QueueingRwMutex & operator=(QueueingRwMutex &&) = delete;
    QueueingRwMutex & operator=(QueueingRwMutex const &) = delete;

    #ifdef SHAREMIND_INSTRUCT_VALGRIND
    inline QueueingRwMutex() noexcept { ANNOTATE_RWLOCK_CREATE(&m_mutex); }
    #endif

    inline ~QueueingRwMutex() noexcept {
        #ifdef SHAREMIND_INSTRUCT_VALGRIND
        ANNOTATE_RWLOCK_DESTROY(&m_mutex);
        #endif
    }

private: /* Fields: */

    tbb::queuing_rw_mutex m_mutex;

};

} /* namespace sharemind { */

#endif /* SHAREMIND_QUEUEINGRWMUTEX_H */
