#include "CRoute.h"

CRoute &CRoute::routes()
{
	static CRoute routesSingleton;
	return routesSingleton;
}

size_t CRoute::registerRoute(const std::string &url, int script)
{
	Route newRoute;
	newRoute.scriptController = script;
	newRoute.isScript = true;
	routeList.push_back(newRoute);

	//std::cout << "script route " << newRoute.scriptController << std::endl;

	routeList.back().parseUrlToRules(url);

	return routeList.size() - 1;
}

size_t CRoute::getRoute(const std::string &urlRule)
{
	for (size_t i = 0, size = routeList.size(); i < size; i++)
	{
		if (routeList[i].routeRuleStr == urlRule)
		{
			return i;
		}
	}

	return ~(0uLL);
}

routeResponse CRoute::testRoute(const std::string &url, const std::string &method)
{
	routeResponse result;

	for (int i = 0, size = routeList.size(); i < size; i++)
	{
		auto &route = routeList[i];

		if (route.compare(url, result.r))
		{
			result.route = &route;
			result.routeId = i;

			if (!route.acceptPost && method == "POST")
			{
				result.responseNow = true;
				result.httpCode = 404;
			}

			return result;
		}

		result.r.clear();
	}

	return result;
}

CRoute::CRoute()
{


}

void Route::parseUrlToRules(const std::string &url)
{
	routeRuleStr = url;
	rules.clear();

	std::string tempstr;

	bool commandOpen = false;

	urlRule newRule;

	for (size_t i = 0, size = url.size(); i < size; ++i)
	{
		if (strncmp(&(url.c_str()[i]), "{{", sizeof("{{") - 1) == 0)
		{
			if (newRule.text.size() > 0)
			{
				rules.push_back(newRule);
			}

			newRule = urlRule();

			commandOpen = true;
			newRule.isParameter = true;

			i += sizeof("{{") - 2;
			continue;
		}

		if (strncmp(&(url.c_str()[i]), "??", sizeof("??") - 1) == 0 && commandOpen)
		{
			newRule.optional = true;

			i += sizeof("??") - 2;
			continue;
		}

		if (strncmp(&(url.c_str()[i]), "}}", sizeof("}}") - 1) == 0 && commandOpen)
		{
			if (newRule.text.size() > 0)
			{
				rules.push_back(newRule);
			}

			newRule = urlRule();

			commandOpen = false;

			i += sizeof("}}") - 2;
			continue;
		}

		newRule.text += url[i];
	}

	if (newRule.text.size() > 0)
	{
		rules.push_back(newRule);
	}
}

bool Route::compare(const std::string &url, std::vector<std::string> &urlParts)
{
	bool result = true;

	if (rules.size() == 0)
		return false;

	size_t ruleID = 0, rulesSize = rules.size();

	std::string temp;

	int rulesOk = 0;

	for (size_t i = 0, size = url.size(); i < size; ++i)
	{
		if (ruleID >= rulesSize)
			return false;

		auto &nRule = rules[ruleID];

		if (ruleID < (rulesSize - 1))
		{
			auto &pRule = rules[ruleID + 1];

			if (nRule.isParameter && pRule.text.size() > 0)
			{
				if (nRule.optional)
				{
					if (url[i] == '/')
					{
						++rulesOk;
						break;
					}
				}

				if (!pRule.isParameter)
				{
					if (strncmp(&(url.c_str()[i]), pRule.text.c_str(), pRule.text.size()) == 0)
					{
						if (!nRule.optional && temp.size() == 0)
							return false;

						//temp.insert(temp.end(), url[i]);

						urlParts.push_back(temp);
						temp.clear();

						++ruleID;

						--i;
						++rulesOk;
						continue;
					}
				}
				else
				{
					if (url[i] == '/')
					{
						urlParts.push_back(temp);
						temp.clear();

						++ruleID;
						++rulesOk;
						continue;
					}
				}
			}
		}

		if (!nRule.isParameter)
		{
			if (strncmp(&(url.c_str()[i]), nRule.text.c_str(), nRule.text.size()) == 0)
			{
				urlParts.push_back(nRule.text);
				temp.clear();
				i += nRule.text.size() - 1;
				++ruleID;
				++rulesOk;
			}
			else
			{
				return false;
			}
		}
		else
		{
			temp.insert(temp.end(), url[i]);
		}
	}

	if (temp.size() > 0)
	{
		urlParts.push_back(temp);
		temp.clear();
	}

	/*for (auto &part : urlParts)
	{
		std::cout << part << std::endl;
	}*/

	return result;
}

Route::Route()
{
	isCacheable = true;
	isScript = false;
	isCached = false;
	acceptPost = false;
}
