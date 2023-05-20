#pragma once

#include <string>

#include "ActivityEndpoint.h"
#include "NetworkWrapper.h"

namespace Strava
{
	class IEndpointProvider
	{
	public:
		virtual ActivityEndpoint& Activities() = 0;
	};

	class IAPICore
	{
	public:
		virtual bool IsInitialized() const = 0;

		virtual std::string GetApiHostName() const = 0;
		virtual std::string GetRootEndpoint() const = 0;
		virtual std::string GetAccessToken() const = 0;

		virtual NetworkWrapper& GetNetworkWrapper() = 0;
	};
}