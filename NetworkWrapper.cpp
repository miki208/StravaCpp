#include "impl/NetworkWrapper.h"

#include "boost/beast/version.hpp"
#include "boost/certify/extensions.hpp"
#include "boost/certify/https_verification.hpp"

namespace Strava
{
	NetworkWrapper::NetworkWrapper(const std::string& host) :
		m_host(host), m_sslContext(ssl::context::tlsv12_client), m_initialized(false)
	{

	}

	NetworkWrapper::~NetworkWrapper()
	{
		DisconnectIfNeeded();
	}

	bool NetworkWrapper::Initialize()
	{
		error_code ec;

		if (m_initialized)
			return true;

		tcp::resolver dnsResolver(m_ioc);
		m_resolvedEndpoints = dnsResolver.resolve(tcp::v4(), m_host, "443", ec);
		if (ec)
		{
			m_resolvedEndpoints.reset();

			return false;
		}

		m_sslContext.set_verify_mode(ssl::verify_peer | ssl::context::verify_fail_if_no_peer_cert, ec);
		if (ec)
			return false;

		m_sslContext.set_default_verify_paths(ec);
		if (ec)
			return false;

		boost::certify::enable_native_https_server_verification(m_sslContext);

		m_initialized = true;

		return m_initialized;
	}

	bool NetworkWrapper::SendRequest(http::verb reqType, const std::string& path, const HeaderVector& header, const json::object& body, unsigned int& status, boost::json::value& response)
	{
		if (!ConnectIfNeeded())
			return false;

		error_code ec;
		http::request<http::string_body> req;

		req.version(11);
		req.method(reqType);

		if (reqType == http::verb::get)
		{
			std::stringstream pathAndQuery;
			pathAndQuery << path;

			bool isFirst = true;
			for (const auto& item : body)
			{
				const auto key = item.key();
				const auto& value = item.value();

				if (value.is_primitive())
				{
					if (isFirst)
					{
						isFirst = false;

						pathAndQuery << '?';
					}
					else
					{
						pathAndQuery << '&';
					}

					pathAndQuery << key << '=' << value;
				}
			}

			req.target(pathAndQuery.str());
		}
		else if (reqType == http::verb::post || reqType == http::verb::put)
		{
			req.target(path);

			req.body() = json::serialize(body);
		}

		BootstrapRequest(req);

		//--- set header
		for (const auto& headerItem : header)
			req.set(headerItem.first, headerItem.second);
		
		http::write(*m_sslStream, req, ec);
		if (ec)
			return false;

		http::response<http::string_body> res;
		http::read(*m_sslStream, m_readingBuffer, res, ec);
		if (ec)
			return false;

		status = res.result_int();

		auto contentType = NetworkWrapper::GetContentType(res);
		if (contentType.size() == 0 || contentType != "application/json")
			return false;

		response = boost::json::parse(res.body(), ec);
		if (ec)
			return false;

		return true;
	}

	bool NetworkWrapper::ConnectIfNeeded()
	{
		if (!m_initialized)
			return false;

		error_code ec;

		if (!m_sslStream.has_value())
			m_sslStream.emplace(m_ioc, m_sslContext);

		if (beast::get_lowest_layer(*m_sslStream).socket().is_open())
			return true;

		if (!SSL_set_tlsext_host_name(m_sslStream->native_handle(), m_host.c_str()))
		{
			m_sslStream.reset();

			return false;
		}

		beast::get_lowest_layer(*m_sslStream).connect(*m_resolvedEndpoints, ec);
		if (ec)
		{
			m_sslStream.reset();

			return false;
		}

		m_sslStream->handshake(ssl::stream_base::client, ec);
		if (ec)
		{
			m_sslStream.reset();

			return false;
		}

		return true;
	}

	bool NetworkWrapper::DisconnectIfNeeded()
	{
		if (!m_initialized)
			return false;

		error_code ec;

		if (!m_sslStream.has_value())
			return true;

		if (beast::get_lowest_layer(*m_sslStream).socket().is_open())
		{
			m_sslStream->shutdown(ec);

			if (ec && ec != net::error::eof)
			{
				m_sslStream.reset();

				return false;
			}

			beast::get_lowest_layer(*m_sslStream).socket().close(ec);
			if (ec)
			{
				m_sslStream.reset();

				return false;
			}
		}

		m_sslStream.reset();

		return true;
	}

	void NetworkWrapper::BootstrapRequest(http::request<http::string_body>& request)
	{
		request.set(http::field::host, m_host);
		request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
	}

	boost::beast::string_view NetworkWrapper::GetContentType(const http::response<http::string_body>& response)
	{
		auto contentTypeIter = response.find(http::field::content_type);

		if (contentTypeIter == response.cend())
			return boost::beast::string_view();

		auto delimiterPos = contentTypeIter->value().find(';');
		if (delimiterPos == boost::beast::string_view::npos)
			return contentTypeIter->value();

		const auto contentType = contentTypeIter->value();

		return contentType.substr(0, delimiterPos);
	}
}
