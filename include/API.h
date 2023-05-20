#pragma once

#include "ActivityEndpoint.h"
#include "APIInterfaces.h"
#include "NetworkWrapper.h"

namespace Strava
{
	class API : public boost::noncopyable, public IEndpointProvider, public IAPICore
	{
	public:
		bool IsInitialized() const override;

		ActivityEndpoint& Activities() override;

		std::string GetApiHostName() const override;
		std::string GetRootEndpoint() const override;
		std::string GetAccessToken() const override;

		NetworkWrapper& GetNetworkWrapper() override;

	private:
		API(net::io_context& ioc, const std::string& accessToken);

		bool Initialize();

	private:
		ActivityEndpoint m_activityEndpoint;

		const std::string cStravaHostName;
		const std::string cRootEndpoint;

		std::string m_accessToken;

		bool m_initialized;

		NetworkWrapper m_apiContext;

	private:
		friend class APIFactory;
	};
}