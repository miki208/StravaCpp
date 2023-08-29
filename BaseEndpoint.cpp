#include "impl/BaseEndpoint.h"

#include "APIInterfaces.h"
#include "impl/ClientNetworkWrapper.h"

#include "boost/beast/http/status.hpp"

namespace Strava
{
	BaseEndpoint::BaseEndpoint(std::shared_ptr<IAPIInternalInterface> pApiInternal, const AuthenticatedAthlete& athlete, const std::function<void(const AuthenticatedAthlete&)>& m_onAuthenticatedAthleteUpdatedCb) :
		m_pApiInternal(pApiInternal),
		m_athlete(athlete),
		m_onAuthenticatedAthleteUpdatedCb(m_onAuthenticatedAthleteUpdatedCb)
	{
	}

	ResultSet BaseEndpoint::SendGetRequest(const std::string& endpoint, std::vector<std::pair<http::field, std::string>> header, json::object query, bool authenticatedRequest)
	{
		return SendCommon(http::verb::get, endpoint, header, query, authenticatedRequest);
	}

	ResultSet BaseEndpoint::SendPostRequest(const std::string& endpoint, std::vector<std::pair<http::field, std::string>> header, json::object body, bool authenticatedRequest)
	{
		return SendCommon(http::verb::post, endpoint, header, body, authenticatedRequest);
	}

	ResultSet BaseEndpoint::SendPutRequest(const std::string& endpoint, std::vector<std::pair<http::field, std::string>> header, json::object body, bool authenticatedRequest)
	{
		return SendCommon(http::verb::put, endpoint, header, body, authenticatedRequest);
	}

	ResultSet BaseEndpoint::SendCommon(http::verb verb, const std::string& endpoint, std::vector<std::pair<http::field, std::string>> header, json::object params, bool authenticatedRequest)
	{
		if (!m_pApiInternal->IsInitialized())
			return ResultSet(boost::json::value(), static_cast<unsigned int>(http::status::failed_dependency));

		if (authenticatedRequest)
		{
			int64_t nowTs = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

			if (m_athlete.expires_at() < nowTs || m_athlete.expires_at() - nowTs < 30LL * 60)
			{
				//--- refresh access token

				json::object refreshRequestParams;

				refreshRequestParams["client_id"] = m_pApiInternal->GetClientId();
				refreshRequestParams["client_secret"] = m_pApiInternal->GetClientSecret();
				refreshRequestParams["grant_type"] = "refresh_token";
				refreshRequestParams["refresh_token"] = m_athlete.refresh_token();

				auto refreshResponse = SendPostRequest("/oauth/token", {}, refreshRequestParams, false);

				if (static_cast<http::status>(refreshResponse.GetStatus()) == http::status::ok)
				{
					const auto& access_token = refreshResponse.Get("/access_token");
					const auto& expires_at = refreshResponse.Get("/expires_at");
					const auto& refresh_token = refreshResponse.Get("/refresh_token");

					if(!access_token.is_string() || !expires_at.is_number() || !refresh_token.is_string())
						return ResultSet(boost::json::value(), static_cast<unsigned int>(http::status::unauthorized));

					m_athlete = AuthenticatedAthlete(std::string(access_token.get_string()), expires_at.to_number<int>(), std::string(refresh_token.get_string()));

					if (m_onAuthenticatedAthleteUpdatedCb)
						m_onAuthenticatedAthleteUpdatedCb(m_athlete);
				}
				else
				{
					return ResultSet(boost::json::value(), static_cast<unsigned int>(http::status::unauthorized));
				}
			}

			header.push_back({ http::field::authorization, "Bearer " + m_athlete.access_token() });
		}

		unsigned int status;
		json::value response;

		m_pApiInternal->GetClientNetworkWrapper().SendRequest(
			verb,
			m_pApiInternal->GetRootEndpoint() + endpoint,
			header,
			params,
			status,
			response
		);

		return ResultSet(std::move(response), status);
	}
}