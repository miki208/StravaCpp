#include "impl/ServerNetworkParametersBundle.h"

#include "impl/Helpers.h"

namespace Strava
{
	namespace Async
	{
		ServerNetworkParametersBundle::ServerNetworkParametersBundle(const std::string& serverHostname, uint16_t serverPort, const std::string& certFile, const std::string& pkFile, const std::string& pkPassphrase) :
			m_serverHostname(serverHostname), m_serverPort(serverPort), m_certFile(certFile), m_pkFile(pkFile), m_pkPassphrase(pkPassphrase)
		{
		}

		std::string ServerNetworkParametersBundle::GetServerHostname() const
		{
			return m_serverHostname;
		}

		uint16_t ServerNetworkParametersBundle::GetServerPort() const
		{
			return m_serverPort;
		}

		std::string ServerNetworkParametersBundle::GetCertFile() const
		{
			return m_certFile;
		}

		std::string ServerNetworkParametersBundle::GetPkFile() const
		{
			return m_pkFile;
		}

		std::string ServerNetworkParametersBundle::GetPkPassphrase() const
		{
			return m_pkPassphrase;
		}

		void ServerNetworkParametersBundle::SetTokenExchangeTarget(const std::string& tokenExchangeTarget, const SubscriptionEndpoint::OnSubscriptionEvent& onSubscriptionEventCb)
		{
			m_tokenExchangeTarget = tokenExchangeTarget;
		}

		std::string ServerNetworkParametersBundle::GetTokenExchangeTarget() const
		{
			return m_tokenExchangeTarget;
		}

		void ServerNetworkParametersBundle::SetSubscriptionTarget(const std::string& subscriptionTarget)
		{
			m_subscriptionTarget = subscriptionTarget;
		}

		std::string ServerNetworkParametersBundle::GetSubscriptionTarget() const
		{
			return m_subscriptionTarget;
		}

		SubscriptionEndpoint::OnSubscriptionEvent ServerNetworkParametersBundle::GetSubscriptionCallback() const
		{
			return m_onSubscriptionEventCb;
		}

		void ServerNetworkParametersBundle::AddCustomRequestHandler(const RequestFilter& reqFilter, const RequestHandler& reqHandler)
		{
			m_requestHandlers.push_back({ reqFilter, reqHandler });
		}

		std::vector<std::pair<RequestFilter, RequestHandler>>& ServerNetworkParametersBundle::GetRequestHandlers()
		{
			return m_requestHandlers;
		}

		std::shared_ptr<IServerNetworkParametersBundleGetter> ServerNetworkParametersBundle::GetInternalInterface()
		{
			return shared_from_this();
		}
	}
}
