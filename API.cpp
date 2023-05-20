#include "API.h"

namespace Strava
{
	API::API(net::io_context& ioc, const std::string& accessToken) : cStravaHostName("www.strava.com"), cRootEndpoint("/api/v3"), m_apiContext(cStravaHostName, ioc), m_initialized(false),
		m_activityEndpoint(*this), m_accessToken(accessToken)
	{

	}

	bool API::Initialize()
	{
		if (m_initialized)
			return true;

		m_initialized = m_apiContext.Initialize();

		return m_initialized;
	}

	bool API::IsInitialized() const
	{
		return m_initialized;
	}
	
	ActivityEndpoint& API::Activities()
	{
		if(!IsInitialized())
			Initialize();

		return m_activityEndpoint;
	}

	std::string API::GetApiHostName() const
	{
		return cStravaHostName;
	}

	std::string API::GetRootEndpoint() const
	{
		return cRootEndpoint;
	}

	std::string API::GetAccessToken() const
	{
		return m_accessToken;
	}

	NetworkWrapper& API::GetNetworkWrapper()
	{
		return m_apiContext;
	}
}