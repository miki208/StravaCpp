#include "impl/BaseEndpoint.h"

#include "APIInterfaces.h"
#include "impl/NetworkWrapper.h"

#include "boost/beast/http/status.hpp"

namespace Strava
{
	BaseEndpoint::BaseEndpoint(std::shared_ptr<IAPIInternalInterface> pApiInternal, const AuthenticatedAthlete& athlete) :
		m_pApiInternal(pApiInternal),
		m_athlete(athlete)
	{
	}

	ResultSet BaseEndpoint::SendGetRequest(const std::string& endpoint, std::vector<std::pair<http::field, std::string>> header, json::object query, bool authenticatedRequest)
	{
		if (!m_pApiInternal->IsInitialized())
			return ResultSet(boost::json::value(), static_cast<unsigned int>(http::status::failed_dependency));

		if (authenticatedRequest)
			header.push_back({ http::field::authorization, "Bearer " + m_athlete.access_token() });

		unsigned int status;
		json::value response;

		m_pApiInternal->GetNetworkWrapper().SendRequest(
			http::verb::get,
			m_pApiInternal->GetRootEndpoint() + endpoint,
			header,
			query,
			status,
			response
		);

		return ResultSet(std::move(response), status);
	}

	ResultSet BaseEndpoint::SendPostRequest(const std::string& endpoint, std::vector<std::pair<http::field, std::string>> header, json::object body, bool authenticatedRequest)
	{
		if (!m_pApiInternal->IsInitialized())
			return ResultSet(boost::json::value(), static_cast<unsigned int>(http::status::failed_dependency));

		if (authenticatedRequest)
			header.push_back({ http::field::authorization, "Bearer " + m_athlete.access_token() });

		unsigned int status;
		json::value response;

		m_pApiInternal->GetNetworkWrapper().SendRequest(
			http::verb::post,
			m_pApiInternal->GetRootEndpoint() + endpoint,
			header,
			body,
			status,
			response
		);

		return ResultSet(std::move(response), status);
	}

	ResultSet BaseEndpoint::SendPutRequest(const std::string& endpoint, std::vector<std::pair<http::field, std::string>> header, json::object body, bool authenticatedRequest)
	{
		if (!m_pApiInternal->IsInitialized())
			return ResultSet(boost::json::value(), static_cast<unsigned int>(http::status::failed_dependency));

		if (authenticatedRequest)
			header.push_back({ http::field::authorization, "Bearer " + m_athlete.access_token() });

		unsigned int status;
		json::value response;

		m_pApiInternal->GetNetworkWrapper().SendRequest(
			http::verb::put,
			m_pApiInternal->GetRootEndpoint() + endpoint,
			header,
			body,
			status,
			response
		);

		return ResultSet(std::move(response), status);
	}


}