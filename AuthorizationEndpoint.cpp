#include "AuthorizationEndpoint.h"

namespace Strava
{
	AuthorizationEndpoint::AuthorizationEndpoint(std::shared_ptr<IAPIInternalInterface> pApiInternal) : m_pApiInternal(pApiInternal)
	{
	}

	void AuthorizationEndpoint::SetCallbackTarget(const std::string& target)
	{
		m_target = target;
	}

	std::string AuthorizationEndpoint::GetCallbackTarget() const
	{
		return m_target;
	}

	void AuthorizationEndpoint::SetOnAthleteAuthenticatedCallback(const OnAthleteAuthenticated& cb)
	{
		m_onAthleteAuthenticatedCb = cb;
	}

	AuthorizationEndpoint::OnAthleteAuthenticated AuthorizationEndpoint::GetOnAthleteAuthenticatedCallback() const
	{
		return m_onAthleteAuthenticatedCb;
	}

	void AuthorizationEndpoint::SetOnAuthorizationGrantCallback(const OnAuthorizationGrant& cb)
	{
		m_onAuthorizationGrantCb = cb;
	}

	AuthorizationEndpoint::OnAuthorizationGrant AuthorizationEndpoint::GetOnAuthorizationGrantCallback() const
	{
		return m_onAuthorizationGrantCb;
	}

	void AuthorizationEndpoint::SetOnAccessDeniedCallback(const OnAccessDenied& cb)
	{
		m_onAccessDeniedCb = cb;
	}

	AuthorizationEndpoint::OnAccessDenied AuthorizationEndpoint::GetOnAccessDeniedCallback() const
	{
		return m_onAccessDeniedCb;
	}

	std::string AuthorizationEndpoint::GetAuthorizationEndpoint()
	{
		return "/oauth/authorize";
	}

	std::string AuthorizationEndpoint::GetTokenExchangeEndpoint()
	{
		return "oauth/token";
	}
}