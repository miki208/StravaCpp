#include "API.h"

#include "impl/ClientNetworkWrapper.h"
#include "impl/ServerNetworkWrapper.h"

#include "boost/algorithm/string.hpp"

namespace Strava
{
	API::API(int clientId, const std::string& clientSecret, std::string serverInterface, uint16_t serverPort, std::string certFile, std::string pkFile, std::string pkPasshphrase) :
		cStravaHostName("www.strava.com"), cRootEndpoint("/api/v3"),
		m_pClientNetworkWrapper(std::make_unique<ClientNetworkWrapper>(cStravaHostName)), m_initialized(false),
		m_clientId(clientId), m_clientSecret(clientSecret), m_serverInterface(serverInterface), m_serverPort(serverPort),
		m_certFile(certFile), m_pkFile(pkFile), m_pkPassphrase(pkPasshphrase)
	{
		if (!serverInterface.empty() && serverPort != 0)
			m_pServerNetworkWrapper = std::make_unique<Async::ServerNetworkWrapper>();
	}

	API::~API()
	{

	}

	bool API::Initialize()
	{
		if (m_initialized)
			return true;

		m_initialized = m_pClientNetworkWrapper->Initialize();

		if (m_pServerNetworkWrapper)
		{
			m_initialized = m_initialized && !m_pServerNetworkWrapper->Initialize(m_certFile, m_pkFile, m_pkPassphrase,
				[](error_code ec, const std::string& msg)
				{
					//--- server closure events
				},
				[](const ssl::stream<tcp::socket>&) -> bool
				{
					//--- connection events (can control whether the connection should be accepted)

					return true;
				},
				[](const ssl::stream<tcp::socket>&, const error_code&)
				{
					//--- disconnection events (with error code if it's due to an error)
				},
				[](const http::request<http::string_body>& req, http::response<http::string_body>& resp) -> bool
				{
					std::vector<std::string> pathElements;
					boost::split(pathElements, req.target(), boost::is_any_of("/"));

					//--- Strava requests
					//------ token exchange
					//------ callback validation for webhooks
					//------ subscription events from webhooks

					//--- User requests
					//------ deauth requests
					//------ login requests
					//------ other requests related to the app

					return false;
				}
			);
		}

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

	int API::GetClientId() const
	{
		return m_clientId;
	}

	std::string API::GetClientSecret() const
	{
		return m_clientSecret;
	}

	ClientNetworkWrapper& API::GetClientNetworkWrapper()
	{
		return *m_pClientNetworkWrapper;
	}
}