#pragma once

#include "boost/json.hpp"
#include "boost/noncopyable.hpp"

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>

namespace json = boost::json;

namespace Strava
{
	class IAPIInternalInterface;
	
	struct SubscriptionEvent
	{
		enum class EObjectType {
			eActivity,
			eAthlete,
			eUnknown
		};

		enum class EAspectType {
			eCreate,
			eUpdate,
			eDelete,
			eUnknown
		};

		EObjectType object_type{ EObjectType::eUnknown };
		uint64_t object_id{};
		EAspectType aspect_type{ EAspectType::eUnknown };
		json::object updates{};
		uint64_t owner_id{};
		uint32_t subscription_id{};
		uint64_t event_time{};
	};

	class SubscriptionEndpoint : public boost::noncopyable
	{
	public:
		using OnSubscriptionEvent = std::function<void(const SubscriptionEvent&)>;

	public:
		unsigned int Create(uint32_t& subscriptionId);
		unsigned int View(uint32_t& subscriptionId);
		unsigned int Delete(uint32_t subscriptionId);

	protected:
		SubscriptionEndpoint(std::shared_ptr<IAPIInternalInterface> pApiInternal);

		std::string GetHubVerifyToken() const;
		std::string GetSubscriptionEndpoint() const;

		void SetCallbackTarget(const std::string& callback);
		std::string GetCallbackTarget() const;

		void SetCallback(const OnSubscriptionEvent& onSubscriptionEventCb);
		OnSubscriptionEvent GetCallback() const;

		static std::string GenerateHubVerifyToken();

	private:
		std::shared_ptr<IAPIInternalInterface> m_pApiInternal;

		mutable std::mutex m_mtxHubVerifyToken;
		std::string m_hubVerifyToken;

		std::string m_subscriptionCallbackTarget;
		OnSubscriptionEvent m_onSubscriptionEventCb;

		friend class API;
	};
}