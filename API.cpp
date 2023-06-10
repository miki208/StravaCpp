#include "API.h"

#include "impl/NetworkWrapper.h"

namespace Strava
{
	API::API() : cStravaHostName("www.strava.com"), cRootEndpoint("/api/v3"), m_pNetworkWrapper(std::make_unique<NetworkWrapper>(cStravaHostName)), m_initialized(false)
	{

	}

	API::~API()
	{

	}

	bool API::Initialize()
	{
		if (m_initialized)
			return true;

		m_initialized = m_pNetworkWrapper->Initialize();

		return m_initialized;
	}

	bool API::IsInitialized() const
	{
		return m_initialized;
	}
	
	AuthenticatedAPIAccessor API::GetApiAccessor(const AuthenticatedAthlete& athlete)
	{
		if(!IsInitialized())
			Initialize();

		return AuthenticatedAPIAccessor(shared_from_this(), athlete);
	}

	std::string API::GetApiHostName() const
	{
		return cStravaHostName;
	}

	std::string API::GetRootEndpoint() const
	{
		return cRootEndpoint;
	}

	NetworkWrapper& API::GetNetworkWrapper()
	{
		return *m_pNetworkWrapper;
	}
}