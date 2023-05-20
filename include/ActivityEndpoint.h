#pragma once

#include "boost/noncopyable.hpp"

namespace Strava
{
	class IAPICore;

	class ActivityEndpoint : public boost::noncopyable
	{
	public:
		bool List();

	private:
		ActivityEndpoint(IAPICore& apiCore);

	private:
		IAPICore& m_apiCore;

	private:
		friend class API;
	};
}