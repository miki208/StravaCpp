#pragma once

#include "APIInterfaces.h"
#include "AuthenticatedAthlete.h"

#include "boost/noncopyable.hpp"

#include <memory>
#include <string>

namespace Strava
{
	class API : public boost::noncopyable, public IAPIPublicInterface, public IAPIInternalInterface, public std::enable_shared_from_this<API>
	{
	public:
		~API();

		bool IsInitialized() const override;

		AuthenticatedAPIAccessor GetApiAccessor(const AuthenticatedAthlete& athlete) override;

		std::string GetApiHostName() const override;
		std::string GetRootEndpoint() const override;

		NetworkWrapper& GetNetworkWrapper() override;

	private:
		API();

		bool Initialize();

	private:
		const std::string cStravaHostName;
		const std::string cRootEndpoint;

		bool m_initialized;

		std::unique_ptr<NetworkWrapper> m_pNetworkWrapper;

	private:
		friend class APIFactory;
	};
}