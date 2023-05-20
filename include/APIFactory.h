#pragma once

#include "APIInterfaces.h"

#include "boost/asio/io_context.hpp"

#include <memory>

namespace net = boost::asio;

namespace Strava
{
	class APIFactory
	{
	public:
		static std::shared_ptr<IEndpointProvider> CreateAPIInstance(net::io_context& ioc, const std::string& accessToken);
	};
}