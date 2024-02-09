#pragma once

#include "AuthenticatedAthlete.h"

#include "boost/json.hpp"
#include "boost/noncopyable.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <utility>

namespace json = boost::json;

namespace Strava
{
	class IAPIInternalInterface;

	class AuthorizationEndpoint : public boost::noncopyable
	{
	public:
		using OnAthleteAuthenticated = std::function<void(const AuthenticatedAthlete&)>;
		using OnAuthorizationGrant = std::function<bool(const std::string& /*state*/, const std::string& /*scope*/)>;
		using OnAccessDenied = std::function<void(const std::string& /*state*/, const std::string& /*errMsg*/, http::status& /*respStatus*/, json::value& /*respBody*/)>;

		void SetCallbackTarget(const std::string& target);
		std::string GetCallbackTarget() const;

		void SetOnAthleteAuthenticatedCallback(const OnAthleteAuthenticated& cb);
		OnAthleteAuthenticated GetOnAthleteAuthenticatedCallback() const;

		void SetOnAuthorizationGrantCallback(const OnAuthorizationGrant& cb);
		OnAuthorizationGrant GetOnAuthorizationGrantCallback() const;

		void SetOnAccessDeniedCallback(const OnAccessDenied& cb);
		OnAccessDenied GetOnAccessDeniedCallback() const;

		static std::string GetAuthorizationEndpoint();
		static std::string GetTokenExchangeEndpoint();

	protected:
		AuthorizationEndpoint(std::shared_ptr<IAPIInternalInterface> pApiInternal);

	private:
		std::shared_ptr<IAPIInternalInterface> m_pApiInternal;

		std::string m_target;

		OnAthleteAuthenticated m_onAthleteAuthenticatedCb;
		OnAuthorizationGrant m_onAuthorizationGrantCb;
		OnAccessDenied m_onAccessDeniedCb;

		friend class API;
	};
}