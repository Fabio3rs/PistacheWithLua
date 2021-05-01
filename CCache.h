#pragma once
#ifndef _CCache_h_
#define _CCache_h_
#include <unordered_map>
#include <string>
#include <cmath>
#include <vector>
#include <memory>
#include <thread>
#include <algorithm>
#include <functional>
#include <chrono>
#include <shared_mutex>
#include <memory>
#include <atomic>

class CCache
{
public:
	class cacheInstance
	{
		friend CCache;

	public:
		bool expires;
		std::chrono::system_clock::time_point expiresAt;
		//CServerHTTP::HTMLHeader header;
		std::vector<char> dataBytes;
		std::mutex mut;

		inline cacheInstance()
		{
			bool expires = false;

		}
	};

private:
	mutable std::shared_mutex cacheListMutex;

	struct cache_mgr_ptr
	{
		std::shared_ptr<cacheInstance> newptr;
		std::shared_ptr<cacheInstance> oldptr;
		std::atomic<bool> bupdating;

		std::mutex changeMutex;
	};

	std::unordered_map<std::string, cache_mgr_ptr> cacheList;

	std::shared_ptr<CCache::cacheInstance> cachePtrGet(const std::string& path, bool& updating) const;

public:
	struct cacheResult
	{
		const char* cacheHeader;
		const char* cacheData;

		std::shared_ptr<cacheInstance> cInst;

		size_t cacheHeaderSize, cacheDataSize;

		inline cacheResult(const char* cacheHeader, const char* cacheData, size_t cacheHeaderSize, size_t cacheDataSize)
		{
			this->cacheHeader = cacheHeader;
			this->cacheData = cacheData;
			this->cacheHeaderSize = cacheHeaderSize;
			this->cacheDataSize = cacheDataSize;
			cInst = nullptr;
		}

		inline cacheResult()
		{
			cacheHeader = cacheData = nullptr;
			cInst = nullptr;
		}
	};

	static CCache& cache();

	cacheResult requestCacheData(const char* path, const char* htmlRequestHeaderData, size_t htmlRequestHeaderSize);

	cache_mgr_ptr& updateCache(const std::string& namePath, const cacheResult& cr);

private:
	CCache();
	CCache(CCache&) = delete;
};

#endif
