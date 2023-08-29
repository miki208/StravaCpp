#include "ActivityEndpoint.h"

#include "APIInterfaces.h"

namespace Strava
{
	ActivityEndpoint::ActivityEndpoint(std::shared_ptr<IAPIInternalInterface> pApiInternal, const AuthenticatedAthlete& athlete, const std::function<void(const AuthenticatedAthlete&)>& onAuthenticatedAthleteUpdatedCb) :
		BaseEndpoint(pApiInternal, athlete, onAuthenticatedAthleteUpdatedCb)
	{

	}

	ResultSet ActivityEndpoint::List(uint32_t page, uint32_t per_page, const std::optional<uint64_t>& before, const std::optional<uint64_t>& after)
	{
		json::object query;

		query["page"] = page;
		query["per_page"] = per_page;

		if (before.has_value())
			query["before"] = *before;

		if (after.has_value())
			query["after"] = *after;

		return SendGetRequest("/athlete/activities", {}, std::move(query), true);
	}

	ResultSet ActivityEndpoint::Create(const std::string& name, const std::string& sport_type, const std::string& start_date_local, uint64_t elapsed_time, const std::optional<std::string>& type, const std::optional<std::string>& description, const std::optional<double>& distance, const std::optional<bool>& trainer, const std::optional<bool>& commute)
	{
		json::object query;

		query["name"] = name;
		query["sport_type"] = sport_type;
		query["start_date_local"] = start_date_local;
		query["elapsed_time"] = elapsed_time;

		if (type.has_value())
			query["type"] = *type;

		if (description.has_value())
			query["description"] = *description;

		if (distance.has_value())
			query["distance"] = *distance;

		if (trainer.has_value())
			query["trainer"] = (*trainer) ? 1 : 0;

		if (commute.has_value())
			query["commute"] = (*commute) ? 1 : 0;

		return SendPostRequest("/activities", {}, query, true);
	}

	ResultSet ActivityEndpoint::Get(uint64_t id, const std::optional<bool>& include_all_efforts)
	{
		json::object query;

		if (include_all_efforts.has_value())
			query["include_all_efforts"] = *include_all_efforts;

		return SendGetRequest("/activities/" + std::to_string(id), {}, query, true);
	}

	ResultSet ActivityEndpoint::Update(uint64_t id, const UpdatableActivity& updatableActivity)
	{
		return SendPutRequest("/activities/" + std::to_string(id), {}, updatableActivity.ToJson(), true);
	}

	ResultSet ActivityEndpoint::Comments(uint64_t id, uint32_t page_size, const std::optional<std::string>& after_cursor)
	{
		json::object query;

		query["page_size"] = page_size;

		if (after_cursor.has_value())
			query["after_cursor"] = *after_cursor;

		return SendGetRequest("/activities/" + std::to_string(id) + "/comments", {}, query, true);
	}

	ResultSet ActivityEndpoint::Kudoers(uint64_t id, uint32_t page, uint32_t per_page)
	{
		json::object query;

		query["page"] = page;
		query["per_page"] = per_page;

		return SendGetRequest("/activities/" + std::to_string(id) + "/kudos", {}, query, true);
	}

	ResultSet ActivityEndpoint::Laps(uint64_t id)
	{
		return SendGetRequest("/activities/" + std::to_string(id) + "/laps", {}, {}, true);
	}

	ResultSet ActivityEndpoint::Zones(uint64_t id)
	{
		return SendGetRequest("/activities/" + std::to_string(id) + "/zones", {}, {}, true);
	}
}