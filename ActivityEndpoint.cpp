#include "ActivityEndpoint.h"

#include "APIInterfaces.h"
#include "ResultSet.h"

namespace Strava
{
	ActivityEndpoint::ActivityEndpoint(IAPICore& apiCore) : m_apiCore(apiCore)
	{

	}

	bool ActivityEndpoint::List()
	{
		if (!m_apiCore.IsInitialized())
			return false;

		unsigned int status;
		json::value response;

		m_apiCore.GetNetworkWrapper().SendRequest(
			http::verb::get,
			m_apiCore.GetRootEndpoint() + "/activities",
			{
				{http::field::authorization, "Bearer " + m_apiCore.GetAccessToken()}
			},
			status,
			response
		);

		return true;
	}
}