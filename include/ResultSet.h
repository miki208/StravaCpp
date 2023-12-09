#pragma once

#include <functional>
#include <string>

#include "boost/json.hpp"

namespace json = boost::json;

namespace Strava
{
	class ResultSet
	{
	public:
		using OnValueCallback = std::function<void(const json::value&)>;

	public:
		ResultSet(const json::value& obj, unsigned int status);
		ResultSet(json::value&& obj, unsigned int status);

		const json::value& Get(const std::string& path) const;
		bool ForEach(const std::string& path, const OnValueCallback& callback) const;

		unsigned int GetStatus() const;

		friend std::ostream& operator<<(std::ostream& os, const ResultSet& rs);

	protected:
		const json::value m_jsonResult;
		unsigned int m_status;
		
		static const json::value s_cNullValue;
	};
}