#pragma once

#include "APIInterfaces.h"

#include <functional>
#include <string>
#include <vector>
#include <utility>

namespace Strava
{
	namespace Async
	{
		class ServerNetworkParametersBundle : public IServerNetworkParametersBundleGetter, public IServerNetworkParametersBundleSetter, public std::enable_shared_from_this<ServerNetworkParametersBundle>
		{
		public:
			std::string GetServerHostname() const override;
			uint16_t GetServerPort() const override;

			std::string GetCertFile() const override;
			std::string GetPkFile() const override;
			std::string GetPkPassphrase() const override;

			void SetTokenExchangeTarget(const std::string& tokenExchangeTarget, const SubscriptionEndpoint::OnSubscriptionEvent& onSubscriptionEventCb) override;
			std::string GetTokenExchangeTarget() const override;

			void SetSubscriptionTarget(const std::string& subscriptionTarget) override;
			std::string GetSubscriptionTarget() const override;
			SubscriptionEndpoint::OnSubscriptionEvent  GetSubscriptionCallback() const override;

			void AddCustomRequestHandler(const RequestFilter& reqFilter, const RequestHandler& reqHandler) override;
			std::vector<std::pair<RequestFilter, RequestHandler>>& GetRequestHandlers() override;

			std::shared_ptr<IServerNetworkParametersBundleGetter> GetInternalInterface() override;

		private:
			ServerNetworkParametersBundle(const std::string& serverHostname, uint16_t serverPort, const std::string& certFile, const std::string& pkFile, const std::string& pkPassphrase);

		private:
			std::string m_serverHostname;
			uint16_t m_serverPort{};

			std::string m_certFile;
			std::string m_pkFile;
			std::string m_pkPassphrase;

			std::string m_tokenExchangeTarget;

			std::string m_subscriptionTarget;
			SubscriptionEndpoint::OnSubscriptionEvent  m_onSubscriptionEventCb;

			std::vector<std::pair<RequestFilter, RequestHandler>> m_requestHandlers;

			friend class APIFactory;
		};
	}
}
