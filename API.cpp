#include "API.h"

#include "impl/ClientNetworkWrapper.h"
#include "impl/Helpers.h"
#include "impl/ServerNetworkWrapper.h"

namespace Strava
{
	API::API(int32_t clientId, const std::string& clientSecret, std::shared_ptr<Async::IServerNetworkParametersBundleSetter> pSrvNetParamBundle) :
		cStravaHostName("www.strava.com"), cRootEndpoint("/api/v3"),
		m_pClientNetworkWrapper(std::make_unique<ClientNetworkWrapper>(cStravaHostName)), m_initialized(false),
		m_clientId(clientId), m_clientSecret(clientSecret)
	{
		if (pSrvNetParamBundle)
		{
			m_pServerNetworkWrapper = std::make_unique<Async::ServerNetworkWrapper>();

			m_pSrvNetParamBundle = pSrvNetParamBundle->GetInternalInterface();
		}
	}

	API::~API()
	{
		if (m_pServerNetworkWrapper)
			m_pServerNetworkWrapper->Stop();
	}

	bool API::Initialize()
	{
		if (m_initialized)
			return true;

		m_initialized = m_pClientNetworkWrapper->Initialize();

		if (m_pServerNetworkWrapper)
		{
			m_initialized = m_initialized && !m_pServerNetworkWrapper->Initialize(m_pSrvNetParamBundle->GetCertFile(), m_pSrvNetParamBundle->GetPkFile(), m_pSrvNetParamBundle->GetPkPassphrase(),
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
				[this](const http::request<http::string_body>& req, http::response<http::string_body>& resp) -> bool
				{
					std::vector<std::string> pathTokens;
					json::object query;

					if (!HttpHelper::ExtractTokensFromTarget(req, pathTokens, query))
					{
						resp.result(http::status::bad_request);

						return false;
					}

					auto& customRequestHandlers = m_pSrvNetParamBundle->GetRequestHandlers();
					for (auto& customRequestHandler : customRequestHandlers)
					{
						if (customRequestHandler.first(pathTokens))
						{
							boost::json::value reqBody;
							if (!HttpHelper::GetJsonFromBody(req, reqBody))
							{
								resp.result(http::status::bad_request);

								return false;
							}

							http::status respStatus;
							json::value respBody;

							bool shouldKeepAlive = customRequestHandler.second(pathTokens, query, reqBody, respStatus, respBody);

							if (!HttpHelper::SetBodyFromJson(respBody, resp))
							{
								resp.result(http::status::internal_server_error);

								return false;
							}

							resp.result(respStatus);

							return shouldKeepAlive;
						}
					}

					resp.result(http::status::not_found);

					return false;
				}
			);

			if (m_initialized)
				m_pServerNetworkWrapper->Start(m_pSrvNetParamBundle->GetServerInterface(), m_pSrvNetParamBundle->GetServerPort());
		}

		return m_initialized;
	}

	bool API::IsInitialized() const
	{
		return _IsInitialized();
	}
	
	AuthenticatedAPIAccessor API::GetApiAccessor(const AuthenticatedAthlete& athlete)
	{
		if(!IsInitialized())
			Initialize();

		return AuthenticatedAPIAccessor(shared_from_this(), athlete);
	}

	bool API::_IsInitialized() const
	{
		return m_initialized;
	}

	std::string API::_GetApiHostName() const
	{
		return cStravaHostName;
	}

	std::string API::_GetRootEndpoint() const
	{
		return cRootEndpoint;
	}

	int32_t API::_GetClientId() const
	{
		return m_clientId;
	}

	std::string API::_GetClientSecret() const
	{
		return m_clientSecret;
	}

	ClientNetworkWrapper& API::_GetClientNetworkWrapper()
	{
		return *m_pClientNetworkWrapper;
	}
}