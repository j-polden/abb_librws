/***********************************************************************************************************************
 *
 * Copyright (c) 2015, ABB Schweiz AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with
 * or without modification, are permitted provided that
 * the following conditions are met:
 *
 *    * Redistributions of source code must retain the
 *      above copyright notice, this list of conditions
 *      and the following disclaimer.
 *    * Redistributions in binary form must reproduce the
 *      above copyright notice, this list of conditions
 *      and the following disclaimer in the documentation
 *      and/or other materials provided with the
 *      distribution.
 *    * Neither the name of ABB nor the names of its
 *      contributors may be used to endorse or promote
 *      products derived from this software without
 *      specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ***********************************************************************************************************************
 */

#ifndef RWS_POCO_CLIENT_H
#define RWS_POCO_CLIENT_H

#include "Poco/Mutex.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/SharedPtr.h"

namespace abb
{
namespace rws
{
/**
 * \brief A class for a simple client based on POCO.
 */
class POCOClient
{
public:
  /**
   * \brief A struct for containing the result of a communication.
   */
  struct POCOResult
  {
    /**
     * \brief An enum for specifying a general status.
     */
    enum GeneralStatus
    {
      UNKNOWN,                 ///< Unknown status.
      OK,                      ///< Ok status.
      WEBSOCKET_NOT_ALLOCATED, ///< The WebSocket has not been allocated.
      EXCEPTION_POCO_TIMEOUT,  ///< POCO timeout exception.
      EXCEPTION_POCO_NET,      ///< POCO net exception.
      EXCEPTION_POCO_WEBSOCKET ///< POCO WebSocket exception.
    };
    
    /**
     * \brief A struct for containing POCO info.
     */
    struct POCOInfo
    {
      /**
       * \brief A struct for containing HTTP info.
       */
      struct HTTPInfo
      {
        /**
         * \brief A struct for containing HTTP request info.
         */
        struct RequestInfo
        {
          /**
           * \brief Method used for the request.
           */
          std::string method;

          /**
           * \brief URI used for the request.
           */
          std::string uri;
        
          /**
           * \brief Content used for the request.
           */
          std::string content;
        };

        /**
         * \brief A struct for containing HTTP response info.
         */
        struct ResponseInfo
        {
          /**
           * \brief Response status.
           */
          Poco::Net::HTTPResponse::HTTPStatus status;

          /**
           * \brief Response header info.
           */
          std::string header_info;

          /**
           * \brief Response content.
           */
          std::string content;
          
          /**
           * \brief A default constructor.
           */
          ResponseInfo() : status(Poco::Net::HTTPResponse::HTTP_OK) {}
        };
       
        /**
         * \brief Info about a HTTP request.
         */
        RequestInfo request;

         /**
          * \brief Info about a HTTP response.
          */
        ResponseInfo response;
      };
      
      /**
       * \brief A struct for containing WebSocket info.
       */
      struct WebSocketInfo
      {
        /**
         * \brief Flags from a recieved WebSocket frame.
         */
        int flags;

        /**
         * \brief Content from a recieved WebSocket frame.
         */
        std::string frame_content;

        /**
         * \brief A default constructor.
         */
        WebSocketInfo() : flags(0) {};
      };

      /**
       * \brief Container for HTTP info.
       */
      HTTPInfo http;
      
      /**
       * \brief Container for WebSocket info.
       */
      WebSocketInfo websocket;
    };
    
    /**
     * \brief Container for a general status.
     */
    GeneralStatus status;

    /**
    * \brief Container for an exception message (if one occured).
    */
    std::string exception_message;

    /**
     * \brief Container for POCO info.
     */
    POCOInfo poco_info;
    
    /**
     * \brief A default constructor.
     */
    POCOResult() : status(UNKNOWN) {};
   
    /**
     * \brief A method for adding info from a HTTP request.
     *
     * \param request for the HTTP request.
     * \param request_content for the HTTP request's content.
     */
    void addHTTPRequestInfo(const Poco::Net::HTTPRequest& request, const std::string request_content = "");
    
    /**
     * \brief A method for adding info from a HTTP response.
     *
     * \param response for the HTTP response.
     * \param response_content for the HTTP response's content.
     */
    void addHTTPResponseInfo(const Poco::Net::HTTPResponse& response, const std::string response_content = "");
    
    /**
     * \brief A method for adding info from a received WebSocket frame.
     *
     * \param flags for the received WebSocket frame's flags.
     * \param frame_content for the received WebSocket frame's content.
     */
    void addWebSocketFrameInfo(const int flags, const std::string frame_content);

    /**
     * \brief A method to map the general status to a std::string.
     *
     * \return std::string containing the mapped general status.
     */
    std::string mapGeneralStatus() const;
    
    /**
     * \brief A method to map the opcode of a received WebSocket frame.
     *
     * \return std::string containing the mapped opcode.
     */
    std::string mapWebSocketOpcode() const;

    /**
     * \brief A method to construct a text representation of the result.
     *
     * \param verbose indicating if the log text should be verbose or not.
     * \param indent for indentation.
     *
     * \return std::string containing the text representation.
     */
    std::string toString(const bool verbose = false, const size_t indent = 0) const;
  };

  /**
   * \brief A constructor.
   *
   * \param ip_address for the remote server's IP address.
   * \param port for the remote server's port.
   * \param user for the remote server's authentication (assumed to be Digest).
   * \param password for the remote server's authentication (assumed to be Digest).
   */
  POCOClient(const std::string ip_address,
             const Poco::UInt16 port,
             const std::string user,
             const std::string password)
  :
  client_session_(ip_address, port),
  digest_credentials_(user, password)
  {
    client_session_.setKeepAlive(true);
    client_session_.setTimeout(Poco::Timespan(DEFAULT_TIMEOUT));
  }

  /**
   * \brief A destructor.
   */
  ~POCOClient() {}
  
  /**
   * \brief A method for sending a HTTP GET request.
   *
   * \param uri for the URI (path and query).
   *
   * \return POCOResult containing the result.
   */
  POCOResult httpGet(const std::string uri);
  
  /**
   * \brief A method for sending a HTTP POST request.
   *
   * \param uri for the URI (path and query).
   * \param content for the request's content.
   *
   * \return POCOResult containing the result.
   */
  POCOResult httpPost(const std::string uri, const std::string content = "");
  
  /**
   * \brief A method for sending a HTTP PUT request.
   *
   * \param uri for the URI (path and query).
   * \param content for the request's content.
   *
   * \return POCOResult containing the result.
   */
  POCOResult httpPut(const std::string uri, const std::string content = "");

  /**
   * \brief A method for sending a HTTP DELETE request.
   *
   * \param uri for the URI (path and query).
   *
   * \return POCOResult containing the result.
   */
  POCOResult httpDelete(const std::string uri);
  
  /**
   * \brief A method for resetting the timeout to the default value.
   */
  void resetTimeout() { client_session_.setTimeout(Poco::Timespan(DEFAULT_TIMEOUT)); }
  
  /**
   * \brief A method for setting the timeout to a long value.
   */
  void setLongTimeout() { client_session_.setTimeout(Poco::Timespan(LONG_TIMEOUT)); }

  /**
   * \brief A method for checking if the WebSocket exist.
   *
   * \return bool flag indicating if the WebSocket exist or not.
   */
  bool webSocketExist() { return !p_websocket_.isNull(); }

  /**
   * \brief A method for connecting a WebSocket.
   *
   * \param uri for the URI (path and query).
   * \param uri for the WebSocket protocol.
   *
   * \return POCOResult containing the result.
   */
  POCOResult webSocketConnect(const std::string uri, const std::string protocol);
  
  /**
   * \brief A method for receiving a WebSocket frame.
   *
   * \return POCOResult containing the result.
   */
  POCOResult webSocketRecieveFrame();

  /**
   * \brief A method for retriving a substring in a string.
   *
   * \param whole_string for the string containing the substring.
   * \param substring_start start of the substring.
   * \param substring_end end of the substring.
   *
   * \return string containing the substring.
   */
  std::string findSubstringContent(const std::string whole_string,
                                   const std::string substring_start,
                                   const std::string substring_end);

private:
  /**
   * \brief A method for making a HTTP request.
   *
   * \param method for the request's method.
   * \param uri for the URI (path and query).
   * \param content for the request's content.
   *
   * \return POCOResult containing the result.
   */
  POCOResult makeHTTPRequest(const std::string method,
                             const std::string uri = "/",
                             const std::string content = "");
 
  /**
   * \brief A method for sending and receiving HTTP messages.
   *
   * \param result for the result.
   * \param request for the HTTP request.
   * \param response for the HTTP response.
   * \param request_content for the request's content.
   */
  void sendAndReceive(POCOResult& result,
                      Poco::Net::HTTPRequest& request,
                      Poco::Net::HTTPResponse& response,
                      const std::string request_content);
  
  /**
   * \brief A method for performing authentication.
   *
   * \param result for the result.
   * \param request for the HTTP request.
   * \param response for the HTTP response.
   * \param request_content for the request's content.
   */
  void authenticate(POCOResult& result,
                    Poco::Net::HTTPRequest& request,
                    Poco::Net::HTTPResponse& response,
                    const std::string request_content);
  
  /**
   * \brief A method for extracting and storing information from a cookie string.
   *
   * \param cookie_string for the cookie string.
   */
  void extractAndStoreCookie(const std::string cookie_string);

  /**
   * \brief Static constant for the default timeout for HTTP requests, in microseconds.
   */
  static const Poco::Int64 DEFAULT_TIMEOUT = 400000;
  
  /**
   * \brief Static constant for a long timeout for HTTP requests, in microseconds.
   */
  static const Poco::Int64 LONG_TIMEOUT = 10000000;

  /**
   * \brief Static constant for the socket's buffer size.
   */
  static const size_t BUFFER_SIZE = 1024;

  /**
   * \brief A mutex for protecting the clients's HTTP resources.
   */
  Poco::Mutex http_mutex_;

  /**
   * \brief A mutex for protecting the client's WebSocket resources.
   */
  Poco::Mutex websocket_mutex_;

  /**
   * \brief Digest authentication credentials.
   */
  Poco::Net::HTTPDigestCredentials digest_credentials_;
  
  /**
   * \brief A container for cookies received from a server.
   */
  Poco::Net::NameValueCollection cookies_;

  /**
   * \brief A HTTP client session.
   */
  Poco::Net::HTTPClientSession client_session_;
  
  /**
   * \brief A buffer for a WebSocket.
   */
  char websocket_buffer_[BUFFER_SIZE];
  
  /**
   * \brief A pointer to a WebSocket client.
   */
  Poco::SharedPtr<Poco::Net::WebSocket> p_websocket_;
};

} // end namespace rws
} // end namespace abb

#endif