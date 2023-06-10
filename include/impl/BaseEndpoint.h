#pragma once

#include "AuthenticatedAthlete.h"
#include "ResultSet.h"

#include "boost/beast/http/field.hpp"
#include "boost/json.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;

namespace Strava
{
	class IAPIInternalInterface;

	class BaseEndpoint
	{
	protected:
		BaseEndpoint(std::shared_ptr<IAPIInternalInterface> pApiInternal, const AuthenticatedAthlete& athlete);

		ResultSet SendGetRequest(const std::string& endpoint, std::vector<std::pair<http::field, std::string>> header, json::object query, bool authenticatedRequest = true);
		ResultSet SendPostRequest(const std::string& endpoint, std::vector<std::pair<http::field, std::string>> header, json::object body, bool authenticatedRequest = true);
		ResultSet SendPutRequest(const std::string& endpoint, std::vector<std::pair<http::field, std::string>> header, json::object body, bool authenticatedRequest = true);

	protected:
		std::shared_ptr<IAPIInternalInterface> m_pApiInternal;

		AuthenticatedAthlete m_athlete;
	};
}