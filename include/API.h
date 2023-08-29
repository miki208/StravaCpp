#pragma once

#include "APIInterfaces.h"
#include "AuthenticatedAthlete.h"

#include "boost/noncopyable.hpp"

#include <memory>
#include <string>

namespace Strava
{
	namespace Async
	{
		class ServerNetworkWrapper;
	}

	class API : public boost::noncopyable, public IAPIPublicInterface, public IAPIInternalInterface, public std::enable_shared_from_this<API>
	{
	public:
		~API();

		bool IsInitialized() const override;

		AuthenticatedAPIAccessor GetApiAccessor(const AuthenticatedAthlete& athlete) override;

		std::string GetApiHostName() const override;
		std::string GetRootEndpoint() const override;

		int GetClientId() const override;
		std::string GetClientSecret() const override;

		ClientNetworkWrapper& GetClientNetworkWrapper() override;

	private:
		API(int clientId, const std::string& clientSecret, std::string serverInterface = "", uint16_t serverPort = 0, std::string certFile = "", std::string pkFile = "", std::string pkPasshphrase = "");

		bool Initialize();

	private:
		const std::string cStravaHostName;
		const std::string cRootEndpoint;

		const int m_clientId;
		const std::string m_clientSecret;

		bool m_initialized;

		std::unique_ptr<ClientNetworkWrapper> m_pClientNetworkWrapper;
		std::unique_ptr<Async::ServerNetworkWrapper> m_pServerNetworkWrapper;

		std::string m_serverInterface;
		uint16_t m_serverPort;
		std::string m_certFile;
		std::string m_pkFile;
		std::string m_pkPassphrase;

	private:
		friend class APIFactory;
	};
}