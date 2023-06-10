#include "AuthenticatedAthlete.h"

namespace Strava
{
	AuthenticatedAthlete::AuthenticatedAthlete(const std::string& accessToken, int64_t expiresAt, const std::string& refreshToken) :
		m_accessToken(accessToken), m_expiresAt(expiresAt), m_refreshToken(refreshToken)
	{

	}

	std::string AuthenticatedAthlete::access_token() const
	{
		return m_accessToken;
	}

	int64_t AuthenticatedAthlete::expires_at() const
	{
		return m_expiresAt;
	}

	std::string AuthenticatedAthlete::refresh_token() const
	{
		return m_refreshToken;
	}
}