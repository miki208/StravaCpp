#pragma once

#include <functional>

#include "boost/json.hpp"

namespace Strava
{
	class ResultSet
	{
	public:
		ResultSet(const boost::json::object& obj);
		ResultSet(boost::json::object&& obj);

		const boost::json::value& Get(const std::string& path) const;
		bool ForEach(const std::string& path, const std::function<void(const boost::json::value&)>& callback) const;

	protected:
		boost::json::value m_jsonResult;
		boost::json::value m_nullValue;
	};
}