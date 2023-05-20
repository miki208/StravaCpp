#include "APIFactory.h"

#include "API.h"

namespace Strava
{
	std::shared_ptr<IEndpointProvider> APIFactory::CreateAPIInstance(net::io_context& ioc, const std::string& accessToken)
	{
		return std::shared_ptr<API>(new API(ioc, accessToken));
	}
}