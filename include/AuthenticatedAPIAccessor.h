#pragma once

#include "ActivityEndpoint.h"
#include "AuthenticatedAthlete.h"

#include <memory>

namespace Strava
{
	class IAPIInternalInterface;

	class AuthenticatedAPIAccessor
	{
	public:
		AuthenticatedAPIAccessor(std::shared_ptr<IAPIInternalInterface> pApiInternal, const AuthenticatedAthlete& athlete);

		void SetOnAuthenticatedAthleteUpdatedCallback(const BaseEndpoint::OnAuthenticatedAthleteUpdated& cb);

		ActivityEndpoint Activity();

	private:
		std::shared_ptr<IAPIInternalInterface> m_pApiInternal;

		AuthenticatedAthlete m_athlete;
		BaseEndpoint::OnAuthenticatedAthleteUpdated m_onAuthenticatedAthleteUpdatedCb;
	};
}