#include "API.h"

#include "impl/ClientNetworkWrapper.h"
#include "impl/Helpers.h"
#include "impl/ServerNetworkWrapper.h"

#include <functional>

namespace Strava
{
	API::API(int32_t clientId, const std::string& clientSecret, std::shared_ptr<Async::IServerNetworkParametersBundleSetter> pSrvNetParamBundle) :
		cStravaHostName("www.strava.com"), cRootEndpoint("/api/v3"),
		m_pClientNetworkWrapper(std::make_unique<ClientNetworkWrapper>(cStravaHostName)), m_initialized(false),
		m_clientId(clientId), m_clientSecret(clientSecret), m_subscriptionEndpoint(shared_from_this()), m_authorizationEndpoint(shared_from_this())
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
			if (!m_pSrvNetParamBundle->GetSubscriptionTarget().empty())
			{
				m_subscriptionEndpoint.SetCallbackTarget(m_pSrvNetParamBundle->GetSubscriptionTarget());

				auto& registeredHandlers = m_pSrvNetParamBundle->GetRequestHandlers();

				registeredHandlers.insert(registeredHandlers.begin(), std::make_pair(std::bind_front(&API::SubscriptionRequestFilter, this), std::bind_front(&API::SubscriptionRequestHandler, this)));
			}

			if (!m_pSrvNetParamBundle->GetTokenExchangeTarget().empty())
			{
				auto& registeredHandlers = m_pSrvNetParamBundle->GetRequestHandlers();

				registeredHandlers.insert(registeredHandlers.begin(), std::make_pair(std::bind_front(&API::TokenExchangeRequestFilter, this), std::bind_front(&API::SubscriptionRequestHandler, this)));
			}

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
						if (customRequestHandler.first(req.method(), pathTokens))
						{
							boost::json::value reqBody;
							if (!HttpHelper::GetJsonFromBody(req, reqBody))
							{
								resp.result(http::status::bad_request);

								return false;
							}

							http::status respStatus;
							json::value respBody;

							bool shouldKeepAlive = customRequestHandler.second(req.method(), pathTokens, query, reqBody, respStatus, respBody);

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
			{
				m_pServerNetworkWrapper->Start(m_pSrvNetParamBundle->GetServerHostname(), m_pSrvNetParamBundle->GetServerPort());
			}
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

	std::string API::_GetServerHostname() const
	{
		if (m_pSrvNetParamBundle == nullptr)
			return "";

		return m_pSrvNetParamBundle->GetServerHostname();
	}

	ClientNetworkWrapper& API::_GetClientNetworkWrapper()
	{
		return *m_pClientNetworkWrapper;
	}

	SubscriptionEndpoint& API::GetSubscriptionEndpoint()
	{
		if (!IsInitialized())
			Initialize();

		return m_subscriptionEndpoint;
	}

	AuthorizationEndpoint& API::GetAuthorizationEndpoint()
	{
		if (!IsInitialized())
			Initialize();

		return m_authorizationEndpoint;
	}

	bool API::SubscriptionRequestFilter(http::verb method, const std::vector<std::string>& target)
	{
		return HttpHelper::JoinTarget(target) == m_subscriptionEndpoint.GetCallbackTarget() && (method == http::verb::get || method == http::verb::post);
	}

	bool API::SubscriptionRequestHandler(http::verb method, const std::vector<std::string>& target, const json::object& query, const json::value& reqBody, http::status& respStatus, json::value& respBody)
	{
		if (method == http::verb::post)
		{
			//--- subscription event

			if (!reqBody.is_object())
			{
				respStatus = http::status::bad_request;
				respBody = {};

				return false;
			}

			const json::object& reqBodyObj = reqBody.as_object();
			SubscriptionEvent ev;

			const json::value* pObjectType = reqBodyObj.if_contains("object_type");
			if (pObjectType != nullptr && pObjectType->is_string())
			{
				const auto& objectType = pObjectType->as_string();

				if (objectType == "activity")
					ev.object_type = SubscriptionEvent::EObjectType::eActivity;
				else if (objectType == "athlete")
					ev.object_type = SubscriptionEvent::EObjectType::eAthlete;
			}

			const json::value* pObjectId = reqBodyObj.if_contains("object_id");
			if (pObjectId != nullptr && pObjectId->is_number())
				ev.object_id = pObjectId->to_number<decltype(ev.object_id)>();

			const json::value* pAspectType = reqBodyObj.if_contains("aspect_type");
			if (pAspectType != nullptr && pAspectType->is_string())
			{
				const auto& aspectType = pAspectType->as_string();

				if (aspectType == "create")
					ev.aspect_type = SubscriptionEvent::EAspectType::eCreate;
				else if (aspectType == "update")
					ev.aspect_type = SubscriptionEvent::EAspectType::eUpdate;
				else if (aspectType == "delete")
					ev.aspect_type = SubscriptionEvent::EAspectType::eDelete;
			}

			const json::value* pUpdates = reqBodyObj.if_contains("updates");
			if (pUpdates != nullptr && pUpdates->is_object())
				ev.updates = pUpdates->as_object();

			const json::value* pOwnerId = reqBodyObj.if_contains("owner_id");
			if (pOwnerId != nullptr && pOwnerId->is_number())
				ev.owner_id = pOwnerId->to_number<decltype(ev.owner_id)>();

			const json::value* pSubscriptionId = reqBodyObj.if_contains("subscription_id");
			if (pSubscriptionId != nullptr && pSubscriptionId->is_number())
				ev.subscription_id = pSubscriptionId->to_number<decltype(ev.subscription_id)>();

			const json::value* pEventTime = reqBodyObj.if_contains("event_time");
			if (pEventTime != nullptr && pEventTime->is_number())
				ev.event_time = pEventTime->to_number<decltype(ev.event_time)>();

			auto cb = m_subscriptionEndpoint.GetCallback();

			if (cb != nullptr)
				cb(ev);

			respStatus = http::status::ok;
			respBody = {};

			return false;
		}
		else if (method == http::verb::get)
		{
			//--- subscription registration

			const json::value* pHubMode = query.if_contains("hub.mode");

			if (pHubMode == nullptr || !pHubMode->is_string() || pHubMode->as_string() != "subscribe")
			{
				respStatus = http::status::bad_request;
				respBody = {};

				return false;
			}

			const json::value* pHubChallenge = query.if_contains("hub.challenge");
			if (pHubChallenge == nullptr || !pHubMode->is_string())
			{
				respStatus = http::status::bad_request;
				respBody = {};

				return false;
			}

			const json::value* pHubVerifyToken = query.if_contains("hub.verify_token");
			if (pHubVerifyToken == nullptr || !pHubVerifyToken->is_string() || pHubVerifyToken->as_string() != m_subscriptionEndpoint.GetHubVerifyToken())
			{
				respStatus = http::status::bad_request;
				respBody = {};

				return false;
			}

			respStatus = http::status::ok;
			respBody = { {"hub.challenge", pHubChallenge->as_string()} };

			return false;
		}

		respStatus = http::status::bad_request;
		respBody = {};

		return false;
	}

	bool API::TokenExchangeRequestFilter(http::verb method, const std::vector<std::string>& target)
	{
		return HttpHelper::JoinTarget(target) == m_authorizationEndpoint.GetCallbackTarget() && method == http::verb::get;
	}

	bool API::TokenExchangeRequestHandler(http::verb method, const std::vector<std::string>& target, const json::object& query, const json::value& reqBody, http::status& respStatus, json::value& respBody)
	{
		std::string state;
		const json::value* stateField = query.if_contains("state");
		if (stateField->is_string())
			state = stateField->as_string().c_str();

		const json::value* errorField = query.if_contains("error");

		if (errorField != nullptr)
		{
			respStatus = http::status::unauthorized;
			respBody = {};

			auto errorCb = m_authorizationEndpoint.GetOnAccessDeniedCallback();

			if (errorCb != nullptr)
			{
				std::string errMsg = errorField->is_string() ? std::string(errorField->as_string()) : "error_missing_msg_error";

				errorCb(state, errMsg, respStatus, respBody);
			}

			return false;
		}

		const json::value* codeField = query.if_contains("code");
		const json::value* scopeField = query.if_contains("scope");
		if (codeField == nullptr || !codeField->is_string() || scopeField == nullptr || !scopeField->is_string())
		{
			respStatus = http::status::unauthorized;
			respBody = {};

			auto errorCb = m_authorizationEndpoint.GetOnAccessDeniedCallback();

			if (errorCb != nullptr)
				errorCb(state, "error_missing_code_or_scope", respStatus, respBody);

			return false;
		}

		std::string code = codeField->as_string().c_str();
		std::string scope = scopeField->as_string().c_str();

		auto grantCb = m_authorizationEndpoint.GetOnAuthorizationGrantCallback();
		if (grantCb != nullptr && !grantCb(state, scope))
		{
			respStatus = http::status::unauthorized;
			respBody = {};

			auto errorCb = m_authorizationEndpoint.GetOnAccessDeniedCallback();

			if (errorCb != nullptr)
				errorCb(state, "error_authorization_rejected_by_app", respStatus, respBody);

			return false;
		}

		json::object requestBody{
			{"client_id", _GetClientId()},
			{"client_secret", _GetClientSecret()},
			{"code", code},
			{"grant_type", "authorization_code"}
		};

		unsigned int status;
		json::value responseBody;

		bool success = m_pClientNetworkWrapper->SendRequest(http::verb::post, _GetRootEndpoint() + AuthorizationEndpoint::GetTokenExchangeEndpoint(), {}, requestBody, status, responseBody);

		if (!success)
		{
			respStatus = http::status::unauthorized;
			respBody = {};

			auto errorCb = m_authorizationEndpoint.GetOnAccessDeniedCallback();

			if (errorCb != nullptr)
				errorCb(state, "token_exchange_error_" + std::to_string(status), respStatus, respBody);

			return false;
		}

		respStatus = http::status::ok;
		respBody = {};

		return false;
	}
}