#include "CCache.h"

std::shared_ptr<CCache::cacheInstance> CCache::cachePtrGet(const std::string& path, bool& updating) const
{
	std::shared_lock<std::shared_mutex> lock(cacheListMutex);
	auto it = cacheList.find(path);

	if (it != cacheList.end())
	{
		auto& cacheInstance = (*it).second;

		std::shared_ptr<CCache::cacheInstance> cachePtr;

		if (cacheInstance.bupdating)
		{
			cachePtr = cacheInstance.oldptr;
		}
		else
		{
			cachePtr = cacheInstance.newptr;
		}

		updating = cacheInstance.bupdating;

		return cachePtr;
	}

	return std::shared_ptr<CCache::cacheInstance>();
}

CCache& CCache::cache()
{
	static CCache ch;
	return ch;
}

CCache::cacheResult CCache::requestCacheData(const char* path, const char* htmlRequestHeaderData, size_t htmlRequestHeaderSize)
{
	cacheResult result;

	/*CRequestManager::URL processedUrl = CRequestManager::request().processUrl(path);

	bool updating = false;

	std::shared_ptr<CCache::cacheInstance> cachePtr = cachePtrGet(processedUrl.path, updating);

	while (cachePtr.get() == nullptr && updating)
	{
		std::this_thread::yield();
		cachePtr = cachePtrGet(processedUrl.path, updating);
	}

	if (cachePtr.get() == nullptr)
	{
		cachePtr = cachePtrGet(processedUrl.path, updating);
	}

	if (cachePtr.get() != nullptr)
	{
		result.cInst = cachePtr;

		if (cachePtr.get()->dataBytes.size() > 0)
		{
			//std::lock_guard<std::mutex> lock(cacheInstance.mut);
			result.cacheDataSize = cachePtr.get()->dataBytes.size();
			result.cacheData = &(cachePtr.get()->dataBytes[0]);

			return result;
		}
		else
		{
			//if (cachePtr.get()->header.getStatusCode() != 404)
			{
				//std::cout << "executeURL 0 " << processedUrl.path << std::endl;
				CRequestManager::request().executeURL(processedUrl, nullptr, result);
			}
		}
	}
	else
	{
		//std::cout << "executeURL 1" << std::endl;
		CRequestManager::request().executeURL(processedUrl, nullptr, result);
	}*/

	return result;
}

CCache::cache_mgr_ptr& CCache::updateCache(const std::string& namePath, const cacheResult& cr)
{
	std::unique_lock<std::shared_mutex> lock(cacheListMutex);
	auto& newCache = cacheList[namePath];
	return newCache;
}

CCache::CCache()
{
	//cached404Response.r404();


}
