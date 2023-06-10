#pragma once

#include "APIInterfaces.h"

#include <memory>

namespace Strava
{
	class APIFactory
	{
	public:
		std::shared_ptr<IAPIPublicInterface> CreateAPIInstance();
	};
}