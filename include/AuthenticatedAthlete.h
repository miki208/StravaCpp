#pragma once

#include <cstdint>
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
		std::string m_accessToken;
		uint64_t m_expiresAt;

		std::string m_refreshToken;
	};
}