#pragma once

#include "AuthenticatedAPIAccessor.h"
#include "AuthenticatedAthlete.h"

#include <string>

namespace Strava
{
	class NetworkWrapper;

	class IAPIPublicInterface
	{
	public:
		virtual AuthenticatedAPIAccessor GetApiAccessor(const AuthenticatedAthlete& athlete) = 0;
	};

	class IAPIInternalInterface
	{
	public:
		virtual bool IsInitialized() const = 0;

		virtual std::string GetApiHostName() const = 0;
		virtual std::string GetRootEndpoint() const = 0;

		virtual NetworkWrapper& GetNetworkWrapper() = 0;
	};
}