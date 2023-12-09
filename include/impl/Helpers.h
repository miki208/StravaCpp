#pragma once

#include "boost/algorithm/string.hpp"
#include "boost/beast/core.hpp"
#include "boost/beast/http.hpp"
#include "boost/json.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;

using error_code = boost::system::error_code;

namespace Strava
{
	class HttpHelper
	{
	public:
		template<bool isRequest>
		static beast::string_view GetContentType(const http::message<isRequest, http::string_body>& msg)
		{
			auto contentTypeIter = msg.find(http::field::content_type);

			if (contentTypeIter == msg.cend())
				return boost::beast::string_view();

			auto delimiterPos = contentTypeIter->value().find(';');
			if (delimiterPos == boost::beast::string_view::npos)
				return contentTypeIter->value();

			const auto contentType = contentTypeIter->value();

			return contentType.substr(0, delimiterPos);
		}

		template<bool isRequest>
		static bool GetJsonFromBody(const http::message<isRequest, http::string_body>& msg, json::value& result)
		{
			error_code ec;

			auto contentType = HttpHelper::GetContentType(msg);
			if (contentType.size() == 0 || contentType != "application/json")
				return false;

			if (msg.body().empty())
			{
				result = json::value();

				return true;
			}

			result = json::parse(msg.body(), ec);

			if (ec)
				return false;

			return true;
		}

		template<bool isRequest>
		static bool SetBodyFromJson(const json::value& jsonObj, http::message<isRequest, http::string_body>& result)
		{
			result.set(http::field::content_type, "application/json");
			result.body() = json::serialize(jsonObj);

			result.prepare_payload();

			return true;
		}

		static bool ExtractTokensFromTarget(const http::request<http::string_body>& req, std::vector<std::string>& pathTokens, json::object& query)
		{
			std::vector<std::string> targetTokens;
			boost::split(targetTokens, req.target(), boost::is_any_of("?"));

			if (targetTokens.size() < 1 || targetTokens.size() > 2)
				return false;

			boost::split(pathTokens, targetTokens[0], boost::is_any_of("/"));

			if (targetTokens.size() == 2)
			{
				std::vector<std::string> queryTokens;
				boost::split(queryTokens, targetTokens[1], boost::is_any_of("&"));

				for (const auto& queryToken : queryTokens)
				{
					std::vector<std::string> parameterAndValue;
					boost::split(parameterAndValue, queryToken, boost::is_any_of("="));

					if (parameterAndValue.size() != 2)
						continue;

					query.emplace(parameterAndValue[0], parameterAndValue[1]);
				}
			}

			return true;
		}
	};
}