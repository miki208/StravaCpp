#include "APIFactory.h"

#include "API.h"

namespace Strava
{
	std::shared_ptr<IAPIPublicInterface> APIFactory::CreateAPIInstance()
	{
		return std::shared_ptr<API>(new API());
	}
}