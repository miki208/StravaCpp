#pragma once

#include "AuthenticatedAPIAccessor.h"
#include "AuthenticatedAthlete.h"

#include "boost/json.hpp"
#include "boost/beast/http/status.hpp"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;

namespace Strava
{
	/*
	* @brief Public interface available to developer through api core object.
	*/
	class IAPIPublicInterface
	{
	public:
		virtual bool Initialize() = 0;
		virtual bool IsInitialized() const = 0;

		virtual AuthenticatedAPIAccessor GetApiAccessor(const AuthenticatedAthlete& athlete) = 0;
	};

	class ClientNetworkWrapper;

	class IAPIInternalInterface
	{
	public:
		virtual bool _IsInitialized() const = 0;

		virtual std::string _GetApiHostName() const = 0;
		virtual std::string _GetRootEndpoint() const = 0;

		virtual int32_t _GetClientId() const = 0;
		virtual std::string _GetClientSecret() const = 0;

		virtual ClientNetworkWrapper& _GetClientNetworkWrapper() = 0;
	};

	namespace Async
	{
		using RequestFilter = std::function<bool(const std::vector<std::string>& target)>;
		using RequestHandler = std::function<bool(const std::vector<std::string>& target, const json::object& query, const json::value& reqBody, http::status& respStatus, json::value& respBody)>;

		class IServerNetworkParametersBundleGetter
		{
		public:
			virtual std::string GetServerInterface() const = 0;
			virtual uint16_t GetServerPort() const = 0;
			
			virtual std::string GetCertFile() const = 0;
			virtual std::string GetPkFile() const = 0;
			virtual std::string GetPkPassphrase() const = 0;

			virtual std::string GetTokenExchangeTarget() const = 0;
			virtual std::string GetSubscriptionTarget() const = 0;

			virtual std::vector<std::pair<RequestFilter, RequestHandler>>& GetRequestHandlers() = 0;
		};

		class IServerNetworkParametersBundleSetter
		{
		public:
			virtual void SetTokenExchangeTarget(const std::string& tokenExchangeTarget) = 0;
			virtual void SetSubscriptionTarget(const std::string& subscriptionTarget) = 0;

			virtual void AddCustomRequestHandler(const RequestFilter& reqFilter, const RequestHandler& reqHandler) = 0;

			virtual std::shared_ptr<IServerNetworkParametersBundleGetter> GetInternalInterface() = 0;
		};
	}
	
}