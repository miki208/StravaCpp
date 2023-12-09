#include "AuthenticatedAPIAccessor.h"

#include "APIInterfaces.h"

namespace Strava
{
	AuthenticatedAPIAccessor::AuthenticatedAPIAccessor(std::shared_ptr<IAPIInternalInterface> pApiInternal, const AuthenticatedAthlete& athlete) :
		m_pApiInternal(pApiInternal),
		m_athlete(athlete)
	{

	}

	void AuthenticatedAPIAccessor::SetOnAuthenticatedAthleteUpdatedCallback(const BaseEndpoint::OnAuthenticatedAthleteUpdated& cb)
	{
		m_onAuthenticatedAthleteUpdatedCb = cb;
	}

	ActivityEndpoint AuthenticatedAPIAccessor::Activity()
	{
		return ActivityEndpoint(m_pApiInternal, m_athlete, m_onAuthenticatedAthleteUpdatedCb);
	}
}