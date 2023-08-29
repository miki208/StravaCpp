#pragma once

#include "boost/noncopyable.hpp"

#include "boost/asio/ip/tcp.hpp"
#include "boost/beast/core.hpp"
#include "boost/beast/http.hpp"
#include "boost/beast/ssl.hpp"

#include <optional>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;

using tcp = net::ip::tcp;
using error_code = boost::system::error_code;

namespace Strava
{
	namespace Async
	{
		class HttpsSession : public std::enable_shared_from_this<HttpsSession>
		{
		public:
			using OnSessionStart = std::function<bool(const ssl::stream<tcp::socket>&)>;
			using OnSessionEnd = std::function<void(const ssl::stream<tcp::socket>&, const error_code&)>;
			using OnRequest = std::function<bool(const http::request<http::string_body>&, http::response<http::string_body>&)>;

		public:
			HttpsSession(tcp::socket&& socket, ssl::context& context, const OnSessionStart& onSessionStartCb, const OnSessionEnd& onSessionEndCb, const OnRequest& onRequestCb);

			void start();

		private:
			void doRead();
			void processRequest();

		private:
			ssl::stream<tcp::socket> m_sslStream;

			http::request<http::string_body> m_request;

			beast::flat_buffer m_buffer;
			http::response<http::string_body> m_response;

			OnSessionStart m_onSessionStartCb;
			OnSessionEnd m_onSessionEndCb;
			OnRequest m_onRequestCb;
		};

		class ServerNetworkWrapper : public boost::noncopyable
		{
		public:
			using OnStoppedListening = std::function<void(error_code ec, const std::string& msg)>;

		public:
			ServerNetworkWrapper();
			~ServerNetworkWrapper();

			error_code Initialize(const std::string& certificateFile, const std::string& pkFile, const std::string& pkPassphrase, const OnStoppedListening& onStoppedListeningCb, const HttpsSession::OnSessionStart& onSessionStartCb, const HttpsSession::OnSessionEnd& onSessionEndCb, const HttpsSession::OnRequest& onRequestCb);

			std::pair<error_code, std::string> Start(const std::string& interface, uint16_t port);
			bool Stop();

		private:
			void OnAccept(error_code ec, tcp::socket socket);

		private:
			std::atomic_bool m_started;
			std::optional<std::thread> m_ioThread;

			bool m_initialized;

			net::io_context m_ioc;
			ssl::context m_sslContext;

			std::optional<tcp::acceptor> m_tcpAcceptor;

			OnStoppedListening m_onStoppedListeningCb;
			HttpsSession::OnSessionStart m_onSessionStartCb;
			HttpsSession::OnSessionEnd m_onSessionEndCb;
			HttpsSession::OnRequest m_onRequestCb;
		};
	}
}