#pragma once
#ifndef _CFastThreadSignal_h_
#define _CFastThreadSignal_h_
#include <Windows.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>

class CFastThreadSignal
{
	std::atomic<int> status;
	std::chrono::steady_clock::time_point t0;

public:
	void pauseThread(std::thread &th)
	{
		status |= 2;
		t0 = std::chrono::high_resolution_clock::now();
		if ((status & 1) == 0)
		{
			SuspendThread(th.native_handle());
		}
		status &= ~2;
	}

	void resumeThread(std::thread &th)
	{
		status |= 1;

		if ((status & 2) != 0)
		{
			auto t1 = std::chrono::high_resolution_clock::now();
			auto difference = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();

			if (difference <= 100)
			{
				std::this_thread::yield();
			}

			ResumeThread(th.native_handle());
		}

		status &= ~1;
	}

	CFastThreadSignal()
	{
		status = 0;
	}
};


#endif
