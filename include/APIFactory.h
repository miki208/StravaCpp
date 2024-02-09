#pragma once

#include "APIInterfaces.h"

#include <cstdint>
#include <memory>
#include <string>

namespace Strava
{
	/*
	* @brief Object factory used to create configured core components.
	*/
	class APIFactory
	{
	public:
		/*
		* @brief Creates an initialized core api object that is used to obtain access to api endpoints. This should be the first thing created when using this library.
		* @param clientId - oauth2 client id that can be obtained after registering app on the strava developer platform.
		* @param clientSecret - oauth2 client secret that should be kept well secured as this is used to authenticate the app with Strava.
		* @param pSrvNetParamBundle - used to configure built-in web server; it's optional, and if it's left empty, web server is not going to start; can be created with APIFactory.
		* @returns a shared resource to the configured core api object.
		*/
		static std::shared_ptr<IAPIPublicInterface> CreateAPIInstance(int32_t clientId, const std::string& clientSecret, std::shared_ptr<Async::IServerNetworkParametersBundleSetter> pSrvNetParamBundle = nullptr);
		
		/*
		* @brief Create a bundle of network parameters for configuring the web server used to receive client authorization codes and webhook subscription updates, as well as to handle other custom requests. This is optional.
		* @param serverHostname - network interface where the web server is bound.
		* @param serverPort - network port where the web server is listening.
		* @param certFile - certification file which contains the server's certificate, and certificate chain, used for TLS.
		* @param pkFile - private key file for the server's certificate, also used for TLS.
		* @param pkPassphrase - password used to encrypt private key (if encrypted, empty string otherwise).
		* @returns a shared resource to the bundle configured with required info; additionally, token exchange and subscription targets can be set, as well as custom handlers.
		*/
		static std::shared_ptr<Async::IServerNetworkParametersBundleSetter> CreateServerNetworkParametersBundle(const std::string& serverHostname, uint16_t serverPort, const std::string& certFile, const std::string& pkFile, const std::string& pkPassphrase);
	};
}