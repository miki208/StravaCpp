#include "APIFactory.h"

#include "API.h"

namespace Strava
{
	std::shared_ptr<IAPIPublicInterface> APIFactory::CreateAPIInstance(int32_t clientId, const std::string& clientSecret)
	{
		return std::shared_ptr<API>(new API(clientId, clientSecret));
	}
}