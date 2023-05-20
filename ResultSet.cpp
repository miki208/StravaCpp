#include "ResultSet.h"

namespace Strava
{
	ResultSet::ResultSet(const boost::json::object& obj) : m_jsonResult(obj)
	{

	}

	ResultSet::ResultSet(boost::json::object&& obj) : m_jsonResult(std::move(obj))
	{

	}

	const boost::json::value& ResultSet::Get(const std::string& path) const
	{
		boost::json::error_code ec;

		const boost::json::value* result = m_jsonResult.find_pointer(path, ec);

		if (ec)
			return m_nullValue;
		else
			return *result;
	}

	bool ResultSet::ForEach(const std::string& path, const std::function<void(const boost::json::value&)>& callback) const
	{
		const auto& result = Get(path);

		if (result.is_array())
		{
			const auto& arr = result.get_array();

			for (const auto& elem : arr)
				callback(elem);

			return true;
		}

		return false;
	}
}