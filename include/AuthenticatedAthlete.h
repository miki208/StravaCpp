#pragma once

#include <string>

namespace Strava
{
	class AuthenticatedAthlete
	{
	public:
		AuthenticatedAthlete(const std::string& accessToken, int64_t expiresAt, const std::string& refreshToken);

		std::string access_token() const;
		int64_t expires_at() const;

		std::string refresh_token() const;

	private:
		const std::string m_accessToken;
		const uint64_t m_expiresAt;

		const std::string m_refreshToken;
	};
}