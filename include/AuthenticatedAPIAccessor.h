#pragma once

#include "ActivityEndpoint.h"
#include "AuthenticatedAthlete.h"

#include <functional>

namespace Strava
{
	class IAPIInternalInterface;

	class AuthenticatedAPIAccessor
	{
	public:
		AuthenticatedAPIAccessor(std::shared_ptr<IAPIInternalInterface> pApiInternal, const AuthenticatedAthlete& athlete);

		void SetOnAuthenticatedAthleteUpdatedCallback(const std::function<void(const AuthenticatedAthlete&)>& cb);

		ActivityEndpoint Activity();

	private:
		std::shared_ptr<IAPIInternalInterface> m_pApiInternal;

		AuthenticatedAthlete m_athlete;
		std::function<void(const AuthenticatedAthlete&)> m_onAuthenticatedAthleteUpdatedCb;
	};
}