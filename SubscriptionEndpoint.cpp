#include "SubscriptionEndpoint.h"

#include "APIInterfaces.h"
#include "impl/ClientNetworkWrapper.h"

#include <random>

namespace Strava
{
	SubscriptionEndpoint::SubscriptionEndpoint(std::shared_ptr<IAPIInternalInterface> pApiInternal) : m_pApiInternal(pApiInternal)
	{
		
	}

	std::string SubscriptionEndpoint::GetHubVerifyToken() const
	{
		std::lock_guard<std::mutex> lock(m_mtxHubVerifyToken);

		return m_hubVerifyToken;
	}

	std::string SubscriptionEndpoint::GetSubscriptionEndpoint() const
	{
		return "/push_subscriptions";
	}

	void SubscriptionEndpoint::SetCallbackTarget(const std::string& callback)
	{
		m_subscriptionCallbackTarget = callback;
	}

	std::string SubscriptionEndpoint::GetCallbackTarget() const
	{
		return m_subscriptionCallbackTarget;
	}

	void SubscriptionEndpoint::SetCallback(const OnSubscriptionEvent& onSubscriptionEventCb)
	{
		m_onSubscriptionEventCb = onSubscriptionEventCb;
	}

	SubscriptionEndpoint::OnSubscriptionEvent SubscriptionEndpoint::GetCallback() const
	{
		return m_onSubscriptionEventCb;
	}

	std::string SubscriptionEndpoint::GenerateHubVerifyToken()
	{
		std::random_device rd;

		std::mt19937 gen(rd());

		std::uniform_int_distribution<> distribution(11111111, 99999999);

		return std::to_string(distribution(gen));
	}

	unsigned int SubscriptionEndpoint::Create(uint32_t& subscriptionId)
	{
		{
			std::lock_guard<std::mutex> lock(m_mtxHubVerifyToken);

			m_hubVerifyToken = GenerateHubVerifyToken();
		}

		std::string serverHostname = m_pApiInternal->_GetServerHostname();
		if (serverHostname.empty())
			return static_cast<unsigned int>(http::status::failed_dependency);

		json::object requestBody{
			{"client_id", m_pApiInternal->_GetClientId()},
			{"client_secret", m_pApiInternal->_GetClientSecret()},
			{"callback_url", "https://" + serverHostname + GetCallbackTarget()},
			{"verify_token", m_hubVerifyToken}
		};

		unsigned int status;
		json::value responseBody;

		bool success = m_pApiInternal->_GetClientNetworkWrapper().SendRequest(http::verb::post, m_pApiInternal->_GetRootEndpoint() + GetSubscriptionEndpoint(), {}, requestBody, status, responseBody);

		if (success != true)
			return static_cast<unsigned int>(http::status::service_unavailable);

		if (static_cast<http::status>(status) == http::status::ok)
		{
			if (!responseBody.is_object())
				return static_cast<unsigned int>(http::status::expectation_failed);

			json::object& responseBodyObj = responseBody.as_object();

			json::value* pId = responseBodyObj.if_contains("id");
			if(pId == nullptr || !pId->is_number())
				return static_cast<unsigned int>(http::status::expectation_failed);

			subscriptionId = pId->to_number<uint32_t>();
		}

		return status;
	}

	unsigned int SubscriptionEndpoint::View(uint32_t& subscriptionId)
	{
		unsigned int status;
		json::value responseBody;

		bool success = m_pApiInternal->_GetClientNetworkWrapper().SendRequest(http::verb::get, m_pApiInternal->_GetRootEndpoint() + GetSubscriptionEndpoint(), {}, {}, status, responseBody);
		
		if (success != true)
			return static_cast<unsigned int>(http::status::service_unavailable);

		if (static_cast<http::status>(status) == http::status::ok)
		{
			if (!responseBody.is_object())
				return static_cast<unsigned int>(http::status::expectation_failed);

			json::object& responseBodyObj = responseBody.as_object();

			json::value* pId = responseBodyObj.if_contains("id");
			if (pId == nullptr || !pId->is_number())
				return static_cast<unsigned int>(http::status::expectation_failed);

			subscriptionId = pId->to_number<uint32_t>();
		}

		return status;
	}

	unsigned int SubscriptionEndpoint::Delete(uint32_t subscriptionId)
	{
		json::object requestBody;

		unsigned int status;
		json::value responseBody;

		bool success = m_pApiInternal->_GetClientNetworkWrapper().SendRequest(http::verb::get, m_pApiInternal->_GetRootEndpoint() + GetSubscriptionEndpoint() + "/" + std::to_string(subscriptionId), {}, requestBody, status, responseBody);

		if (success != true)
			return static_cast<unsigned int>(http::status::service_unavailable);

		return status;
	}
}