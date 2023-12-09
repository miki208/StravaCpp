#pragma once

#include "APIInterfaces.h"

#include "boost/noncopyable.hpp"

#include <cstdint>
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

		bool Initialize() override;
		bool IsInitialized() const override;

		AuthenticatedAPIAccessor GetApiAccessor(const AuthenticatedAthlete& athlete) override;

		bool _IsInitialized() const override;

		std::string _GetApiHostName() const override;
		std::string _GetRootEndpoint() const override;

		int32_t _GetClientId() const override;
		std::string _GetClientSecret() const override;

		ClientNetworkWrapper& _GetClientNetworkWrapper() override;

	private:
		API(int32_t clientId, const std::string& clientSecret, std::shared_ptr<Async::IServerNetworkParametersBundleSetter> pSrvNetParamBundle = nullptr);

	private:
		const std::string cStravaHostName;
		const std::string cRootEndpoint;

		const int32_t m_clientId;
		const std::string m_clientSecret;

		bool m_initialized;

		std::unique_ptr<ClientNetworkWrapper> m_pClientNetworkWrapper;

		std::unique_ptr<Async::ServerNetworkWrapper> m_pServerNetworkWrapper;
		std::shared_ptr<Async::IServerNetworkParametersBundleGetter> m_pSrvNetParamBundle;

	private:
		friend class APIFactory;
	};
}