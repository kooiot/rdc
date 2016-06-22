#ifndef koo_lock_defs_h__
#define koo_lock_defs_h__

#define USE_SPIN_LOCK
#include <mutex>

#define k_lock_v0(l, m) std::lock_guard<std::mutex> (l)((m))
#define k_lock_r_v0(l, m) std::lock_guard<std::recursive_mutex> (l)((m))

#define k_lock(l, m) std::lock_guard<decltype((m))> (l)((m))
#define k_lock_r(l, m) std::lock_guard<decltype((m))> (l)((m))

#define k_unique_lock_v0(l, m) std::unique_lock<std::mutex&> (l)((m))
#define k_unique_lock(l, m) std::unique_lock<decltype(m)> l(m)
#define k_ulock(l, m, t) std::unique_lock<decltype(m)> l(m, t)

#ifndef USE_SPIN_LOCK
#define koo_smart_locker std::mutex
#define k_smart_lock(l, s) k_lock(l, m)
#else
#include <iostream>
#include <atomic>
class koo_spin_lock
{
	typedef enum { kSpinLockLocked, kSpinLockUnlocked } k_spin_lock_state;
	std::atomic<k_spin_lock_state> m_state = ATOMIC_VAR_INIT(kSpinLockUnlocked);
public:
	koo_spin_lock()
		: m_state(kSpinLockUnlocked)
	{

	}
	void lock()
	{
		while (m_state.exchange(kSpinLockLocked, std::memory_order_acquire) == kSpinLockLocked);
	}

	void unlock()
	{
		m_state.store(kSpinLockUnlocked, std::memory_order_release);
	}
};

class koo_spin_lock_auto
{
	koo_spin_lock &m_lock;
public:
	explicit koo_spin_lock_auto(koo_spin_lock &lock)
		: m_lock(lock)
	{
		m_lock.lock();
	}
	~koo_spin_lock_auto()
	{
		m_lock.unlock();
	}
};
#define k_spin_lock(l, m) koo_spin_lock_auto l(m)
#define koo_smart_locker koo_spin_lock
#define k_smart_lock(l, s) koo_spin_lock_auto l(s)
#endif

#endif // koo_lock_defs_h__
