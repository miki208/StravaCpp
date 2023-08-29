#pragma once

#include "AuthenticatedAthlete.h"
#include "impl/BaseEndpoint.h"
#include "ResultSet.h"

#include <memory>
#include <optional>

namespace Strava
{
	class IAPIInternalInterface;

	struct UpdatableActivity
	{
		std::optional<bool> commute;
		std::optional<bool> trainer;
		std::optional<bool> hide_from_home;
		std::optional<std::string> description;
		std::optional<std::string> name;
		std::optional<std::string> sport_type;
		std::optional<std::string> gear_id;

		json::object ToJson() const
		{
			json::object result;

			if (commute.has_value())
				result["commute"] = *commute;

			if (trainer.has_value())
				result["trainer"] = *trainer;

			if (hide_from_home.has_value())
				result["hide_from_home"] = *hide_from_home;

			if (description.has_value())
				result["description"] = *description;

			if (name.has_value())
				result["name"] = *name;

			if (sport_type.has_value())
				result["sport_type"] = *sport_type;

			if (gear_id.has_value())
				result["gear_id"] = *gear_id;

			return result;
		}
	};

	class ActivityEndpoint : public BaseEndpoint
	{
	public:
		ResultSet List(uint32_t page = 1, uint32_t per_page = 30, const std::optional<uint64_t>& before = std::nullopt, const std::optional<uint64_t>& after = std::nullopt);
		ResultSet Create(const std::string& name, const std::string& sport_type, const std::string& start_date_local, uint64_t elapsed_time, const std::optional<std::string>& type = std::nullopt, const std::optional<std::string>& description = std::nullopt, const std::optional<double>& distance = std::nullopt, const std::optional<bool>& trainer = std::nullopt, const std::optional<bool>& commute = std::nullopt);
		ResultSet Get(uint64_t id, const std::optional<bool>& include_all_efforts = std::nullopt);
		ResultSet Update(uint64_t id, const UpdatableActivity& updatableActivity);

		ResultSet Comments(uint64_t id, uint32_t page_size = 30, const std::optional<std::string>& after_cursor = std::nullopt);
		ResultSet Kudoers(uint64_t id, uint32_t page = 1, uint32_t per_page = 30);
		ResultSet Laps(uint64_t id);
		ResultSet Zones(uint64_t id);

	private:
		ActivityEndpoint(std::shared_ptr<IAPIInternalInterface> pApiInternal, const AuthenticatedAthlete& athlete, const std::function<void(const AuthenticatedAthlete&)>& onAuthenticatedAthleteUpdatedCb);

	private:
		friend class AuthenticatedAPIAccessor;
	};
}