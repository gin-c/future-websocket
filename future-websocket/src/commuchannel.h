#ifndef __COMMU_CHANNEL_37A9A597688F432096285A9772BB8243_H__
#define __COMMU_CHANNEL_37A9A597688F432096285A9772BB8243_H__

#include <queue>
#include <stdint.h>

#ifndef VS2015_VERSION
#define VS2015_VERSION 1900
#endif
#if (defined _MSC_VER && _MSC_VER < VS2015_VERSION)
#include "condition/condition.h"
#else
#include <future>
#include <mutex>
#include <condition_variable>
#endif

namespace commuchannel
{

	template<typename T>
	class CChannel
	{
	public:
		explicit CChannel(int size = 1)
			: m_size(size)
			, m_signalQuit(false)
			, m_queue()
			, m_mutex()
#if (defined _MSC_VER && _MSC_VER < VS2015_VERSION)
			, m_cv(m_mutex)
#else
			, m_cv()
#endif
		{
		}
		virtual ~CChannel()
		{
		}

	public:
		CChannel &operator=(const CChannel &ch) {
			return *this;
		}

	public:
		bool write(const T &data)
		{
#if (defined _MSC_VER && _MSC_VER < VS2015_VERSION)
			while (m_queue.size() > m_size || m_signalQuit == true) {
				if (m_signalQuit) {
					m_mutex.unlock();
					return false;
				}
				m_cv.wait();
			}
			m_mutex.unlock();
			m_mutex.lock();
			m_queue.push(data);
			m_mutex.unlock();
			m_cv.notify();
			return true;
#else
			if (m_size > 0) {
				std::unique_lock<std::mutex> lk(m_mutex);
				while (m_queue.size() > m_size || m_signalQuit == true) {
					if (m_signalQuit) {
						lk.unlock();
						return false;
					}
					m_cv.wait(lk);
				}
				lk.unlock();
			}
			m_mutex.lock();
			m_queue.push(data);
			m_mutex.unlock();
			m_cv.notify_all();
			return true;
#endif
		}
		bool read(T &data)
		{
#if (defined _MSC_VER && _MSC_VER < VS2015_VERSION)
			while (m_queue.empty() || m_signalQuit == true) {
				if (m_signalQuit) {
					m_mutex.unlock();
					return;
				}
				m_cv.wait();
			}
			m_mutex.unlock();
			m_mutex.lock();
			if (!m_queue.empty()) {
				data = m_queue.front();
				m_queue.pop();
			}
			m_mutex.unlock();
			m_cv.notify();
			return true;
#else
			std::unique_lock<std::mutex> lk(m_mutex);
			while (m_queue.empty() || m_signalQuit == true) {
				if (m_signalQuit) {
					lk.unlock();
					return false;
				}
				m_cv.wait(lk);
			}
			lk.unlock();
			m_mutex.lock();
			if (!m_queue.empty()) {
				data = m_queue.front();
				m_queue.pop();
			}
			m_mutex.unlock();
			m_cv.notify_all();
			return true;
#endif
		}
		bool read(T &data, const uint32_t &timeoutMS)
		{
#if (defined _MSC_VER && _MSC_VER < VS2015_VERSION)
			while (m_queue.empty() || m_signalQuit == true) {
				if (m_signalQuit) {
					m_mutex.unlock();
					return false;
				}
				bool r = m_cv.wait(timeoutMS);
				if (!r || m_signalQuit == true) {
					m_mutex.unlock();
					return false;
				}
			}
			m_mutex.unlock();
			m_mutex.lock();
			if (!m_queue.empty()) {
				data = m_queue.front();
				m_queue.pop();
			}
			m_mutex.unlock();
			m_cv.notify();
			return true;
#else
			std::unique_lock<std::mutex> lk(m_mutex);
			while (m_queue.empty() || m_signalQuit == true) {
				if (m_signalQuit) {
					lk.unlock();
					return false;
				}
				const std::cv_status &status = m_cv.wait_for(lk, std::chrono::milliseconds(timeoutMS));
				if (status == std::cv_status::timeout) {
					lk.unlock();
					return false;
				}
			}
			lk.unlock();
			m_mutex.lock();
			if (!m_queue.empty()) {
				data = m_queue.front();
				m_queue.pop();
			}
			m_mutex.unlock();
			m_cv.notify_all();
			return true;
#endif
		}

		void close()
		{
#if (defined _MSC_VER && _MSC_VER < VS2015_VERSION)
			m_mutex.unlock();
			m_mutex.lock();
			m_signalQuit = true;
			m_mutex.unlock();
			m_cv.notify();
#else
			m_signalQuit = true;
			m_cv.notify_all();
#endif
		}
		bool isClose()
		{
			return m_signalQuit;
		}
	private:
		bool m_signalQuit;
		int m_size;
		std::queue<T> m_queue;
#if (defined _MSC_VER && _MSC_VER < VS2015_VERSION)
		mutex::CMutex m_mutex;
		condition::CCondition<mutex::CMutex> m_cv;
#else
		std::mutex m_mutex;
		std::condition_variable m_cv;
#endif
	};

}

#endif // __COMMU_CHANNEL_37A9A597688F432096285A9772BB8243_H__
