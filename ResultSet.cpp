#include "ResultSet.h"

namespace Strava
{
	const json::value ResultSet::s_cNullValue(nullptr);

	ResultSet::ResultSet(const json::value& obj, unsigned int status) :
		m_jsonResult(obj),
		m_status(status)
	{

	}

	ResultSet::ResultSet(json::value&& obj, unsigned int status) :
		m_jsonResult(std::move(obj)),
		m_status(status)
	{

	}

	const json::value& ResultSet::Get(const std::string& path) const
	{
		json::error_code ec;

		const json::value* result = m_jsonResult.find_pointer(path, ec);

		if (ec)
			return s_cNullValue;
		else
			return *result;
	}

	bool ResultSet::ForEach(const std::string& path, const std::function<void(const json::value&)>& callback) const
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

	unsigned int ResultSet::GetStatus() const
	{
		return m_status;
	}

	std::ostream& operator<<(std::ostream& os, const ResultSet& rs)
	{
		os << "Status: " << rs.m_status << std::endl;

		if (rs.m_jsonResult != ResultSet::s_cNullValue)
			os << "Response: " << std::endl << rs.m_jsonResult;

		return os;
	}
}