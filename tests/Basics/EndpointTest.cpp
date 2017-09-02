////////////////////////////////////////////////////////////////////////////////
/// @brief test suite for Endpoint classes
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2004-2012 triagens GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is triAGENS GmbH, Cologne, Germany
///
/// @author Jan Steemann
/// @author Copyright 2012, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#include "Basics/Common.h"

#include "catch.hpp"

#include "Endpoint/Endpoint.h"
#include "Endpoint/EndpointIp.h"
#include "Endpoint/EndpointIpV4.h"
#include "Endpoint/EndpointIpV6.h"
#include "Endpoint/EndpointUnixDomain.h"

using namespace avocadodb;
using namespace avocadodb::basics;
using namespace std;

// -----------------------------------------------------------------------------
// --SECTION--                                                            macros
// -----------------------------------------------------------------------------

#define FACTORY_NAME(name) name ## Factory  

#define FACTORY(name, specification) avocadodb::Endpoint::FACTORY_NAME(name)(specification)

#define CHECK_ENDPOINT_FEATURE(type, specification, feature, expected) \
  e = FACTORY(type, specification); \
  CHECK((expected) == (e->feature())); \
  delete e;

#define CHECK_ENDPOINT_SERVER_FEATURE(type, specification, feature, expected) \
  e = avocadodb::Endpoint::serverFactory(specification, 1, true); \
  CHECK((expected) == (e->feature())); \
  delete e;

TEST_CASE("EndpointTest", "[endpoints]") {

////////////////////////////////////////////////////////////////////////////////
/// @brief test invalid
////////////////////////////////////////////////////////////////////////////////

SECTION("EndpointInvalid") {
  Endpoint* e = nullptr;

  CHECK(e == avocadodb::Endpoint::clientFactory(""));
  CHECK(e == avocadodb::Endpoint::clientFactory("@"));

  CHECK(e == avocadodb::Endpoint::clientFactory("http://"));
  CHECK(e == avocadodb::Endpoint::clientFactory("ssl://"));
  CHECK(e == avocadodb::Endpoint::clientFactory("unix://"));

  CHECK(e == avocadodb::Endpoint::clientFactory("fish://127.0.0.1:8529"));
  CHECK(e == avocadodb::Endpoint::clientFactory("http://127.0.0.1:8529"));
  CHECK(e == avocadodb::Endpoint::clientFactory("https://127.0.0.1:8529"));
  
  CHECK(e == avocadodb::Endpoint::clientFactory("tcp//127.0.0.1:8529"));
  CHECK(e == avocadodb::Endpoint::clientFactory("tcp:127.0.0.1:8529"));
  CHECK(e == avocadodb::Endpoint::clientFactory("ssl:localhost"));
  CHECK(e == avocadodb::Endpoint::clientFactory("ssl//:localhost"));
  CHECK(e == avocadodb::Endpoint::clientFactory("unix///tmp/socket"));
  CHECK(e == avocadodb::Endpoint::clientFactory("unix:tmp/socket"));
  
  CHECK(e == avocadodb::Endpoint::clientFactory("fish@tcp://127.0.0.1:8529"));
  CHECK(e == avocadodb::Endpoint::clientFactory("ssl@tcp://127.0.0.1:8529"));
  CHECK(e == avocadodb::Endpoint::clientFactory("https@tcp://127.0.0.1:8529"));
  CHECK(e == avocadodb::Endpoint::clientFactory("https@tcp://127.0.0.1:"));
  
  CHECK(e == avocadodb::Endpoint::clientFactory("tcp://127.0.0.1:65536"));
  CHECK(e == avocadodb::Endpoint::clientFactory("tcp://127.0.0.1:65537"));
  CHECK(e == avocadodb::Endpoint::clientFactory("tcp://127.0.0.1:-1"));
  CHECK(e == avocadodb::Endpoint::clientFactory("tcp://127.0.0.1:6555555555"));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief test specification
////////////////////////////////////////////////////////////////////////////////

SECTION("EndpointSpecification") {
  Endpoint* e;

  CHECK_ENDPOINT_FEATURE(client, "tcp://127.0.0.1", specification, "http+tcp://127.0.0.1:8529");
  CHECK_ENDPOINT_FEATURE(client, "tcp://localhost", specification, "http+tcp://127.0.0.1:8529");
  CHECK_ENDPOINT_FEATURE(client, "SSL://127.0.0.5", specification, "http+ssl://127.0.0.5:8529");
  CHECK_ENDPOINT_FEATURE(client, "httP@ssl://localhost:4635", specification, "http+ssl://127.0.0.1:4635");

#ifndef _WIN32
  CHECK_ENDPOINT_SERVER_FEATURE(server, "unix:///path/to/socket", specification, "http+unix:///path/to/socket");
  CHECK_ENDPOINT_SERVER_FEATURE(server, "htTp@UNIx:///a/b/c/d/e/f.s", specification, "http+unix:///a/b/c/d/e/f.s");
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// @brief test types
////////////////////////////////////////////////////////////////////////////////

SECTION("EndpointTypes") {
  Endpoint* e;

  CHECK_ENDPOINT_FEATURE(client, "tcp://127.0.0.1", type, avocadodb::Endpoint::EndpointType::CLIENT);
  CHECK_ENDPOINT_FEATURE(client, "tcp://localhost", type, avocadodb::Endpoint::EndpointType::CLIENT);
  CHECK_ENDPOINT_FEATURE(client, "ssl://127.0.0.1", type, avocadodb::Endpoint::EndpointType::CLIENT);
  CHECK_ENDPOINT_FEATURE(client, "ssl://localhost", type, avocadodb::Endpoint::EndpointType::CLIENT);
#ifndef _WIN32
  CHECK_ENDPOINT_FEATURE(client, "unix:///path/to/socket", type, avocadodb::Endpoint::EndpointType::CLIENT);
#endif

  CHECK_ENDPOINT_SERVER_FEATURE(server, "tcp://127.0.0.1", type, avocadodb::Endpoint::EndpointType::SERVER);
  CHECK_ENDPOINT_SERVER_FEATURE(server, "tcp://localhost", type, avocadodb::Endpoint::EndpointType::SERVER);
  CHECK_ENDPOINT_SERVER_FEATURE(server, "ssl://127.0.0.1", type, avocadodb::Endpoint::EndpointType::SERVER);
  CHECK_ENDPOINT_SERVER_FEATURE(server, "ssl://localhost", type, avocadodb::Endpoint::EndpointType::SERVER);
#ifndef _WIN32
  CHECK_ENDPOINT_SERVER_FEATURE(server, "unix:///path/to/socket", type, avocadodb::Endpoint::EndpointType::SERVER);
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// @brief test domains
////////////////////////////////////////////////////////////////////////////////

SECTION("EndpointDomains") {
  Endpoint* e;

  CHECK_ENDPOINT_FEATURE(client, "tcp://127.0.0.1", domain, AF_INET);
  CHECK_ENDPOINT_FEATURE(client, "tcp://192.168.173.13", domain, AF_INET);
  CHECK_ENDPOINT_FEATURE(client, "tcp://localhost", domain, AF_INET);
  CHECK_ENDPOINT_FEATURE(client, "tcp://www.avocadodb.org", domain, AF_INET);
  CHECK_ENDPOINT_FEATURE(client, "tcp://127.0.0.1:8529", domain, AF_INET);
  CHECK_ENDPOINT_FEATURE(client, "tcp://localhost:8529", domain, AF_INET);
  CHECK_ENDPOINT_FEATURE(client, "tcp://www.avocadodb.org:8529", domain, AF_INET);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[127.0.0.1]", domain, AF_INET6);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[::]", domain, AF_INET6);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[127.0.0.1]:8529", domain, AF_INET6);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[::]:8529", domain, AF_INET6);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[2001:0db8:0000:0000:0000:ff00:0042:8329]:8529", domain, AF_INET6);
  CHECK_ENDPOINT_FEATURE(client, "http@tcp://[2001:0db8:0000:0000:0000:ff00:0042:8329]:8529", domain, AF_INET6);
  CHECK_ENDPOINT_FEATURE(client, "ssl://127.0.0.1", domain, AF_INET);
  CHECK_ENDPOINT_FEATURE(client, "ssl://192.168.173.13", domain, AF_INET);
  CHECK_ENDPOINT_FEATURE(client, "ssl://localhost", domain, AF_INET);
  CHECK_ENDPOINT_FEATURE(client, "ssl://www.avocadodb.org", domain, AF_INET);
  CHECK_ENDPOINT_FEATURE(client, "ssl://127.0.0.1:8529", domain, AF_INET);
  CHECK_ENDPOINT_FEATURE(client, "ssl://localhost:8529", domain, AF_INET);
  CHECK_ENDPOINT_FEATURE(client, "ssl://www.avocadodb.org:8529", domain, AF_INET);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[127.0.0.1]", domain, AF_INET6);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[::]", domain, AF_INET6);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[127.0.0.1]:8529", domain, AF_INET6);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[::]:8529", domain, AF_INET6);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[2001:0db8:0000:0000:0000:ff00:0042:8329]:8529", domain, AF_INET6);
  CHECK_ENDPOINT_FEATURE(client, "http@ssl://[2001:0db8:0000:0000:0000:ff00:0042:8329]:8529", domain, AF_INET6);
  
#ifndef _WIN32
  CHECK_ENDPOINT_FEATURE(client, "unix:///tmp/socket", domain, AF_UNIX);
  CHECK_ENDPOINT_FEATURE(client, "unix:///tmp/socket/avocado.sock", domain, AF_UNIX);
  CHECK_ENDPOINT_FEATURE(client, "http@unix:///tmp/socket/avocado.sock", domain, AF_UNIX);
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// @brief test domain types
////////////////////////////////////////////////////////////////////////////////

SECTION("EndpointDomainTypes") {
  Endpoint* e;

  CHECK_ENDPOINT_FEATURE(client, "tcp://127.0.0.1", domainType, avocadodb::Endpoint::DomainType::IPV4);
  CHECK_ENDPOINT_FEATURE(client, "tcp://localhost", domainType, avocadodb::Endpoint::DomainType::IPV4);
  CHECK_ENDPOINT_FEATURE(client, "tcp://www.avocadodb.org", domainType, avocadodb::Endpoint::DomainType::IPV4);
  CHECK_ENDPOINT_FEATURE(client, "tcp://127.0.0.1:8529", domainType, avocadodb::Endpoint::DomainType::IPV4);
  CHECK_ENDPOINT_FEATURE(client, "tcp://localhost:8529", domainType, avocadodb::Endpoint::DomainType::IPV4);
  CHECK_ENDPOINT_FEATURE(client, "tcp://www.avocadodb.org:8529", domainType, avocadodb::Endpoint::DomainType::IPV4);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[127.0.0.1]", domainType, avocadodb::Endpoint::DomainType::IPV6);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[::]", domainType, avocadodb::Endpoint::DomainType::IPV6);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[127.0.0.1]:8529", domainType, avocadodb::Endpoint::DomainType::IPV6);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[::]:8529", domainType, avocadodb::Endpoint::DomainType::IPV6);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[2001:0db8:0000:0000:0000:ff00:0042:8329]:8529", domainType, avocadodb::Endpoint::DomainType::IPV6);
  CHECK_ENDPOINT_FEATURE(client, "TCP://127.0.0.1", domainType, avocadodb::Endpoint::DomainType::IPV4);
  CHECK_ENDPOINT_FEATURE(client, "Tcp://127.0.0.1", domainType, avocadodb::Endpoint::DomainType::IPV4);
  CHECK_ENDPOINT_FEATURE(client, "tCP://127.0.0.1", domainType, avocadodb::Endpoint::DomainType::IPV4);
  CHECK_ENDPOINT_FEATURE(client, "http@tcp://127.0.0.1", domainType, avocadodb::Endpoint::DomainType::IPV4);
  
  CHECK_ENDPOINT_FEATURE(client, "ssl://127.0.0.1", domainType, avocadodb::Endpoint::DomainType::IPV4);
  CHECK_ENDPOINT_FEATURE(client, "ssl://localhost", domainType, avocadodb::Endpoint::DomainType::IPV4);
  CHECK_ENDPOINT_FEATURE(client, "ssl://www.avocadodb.org", domainType, avocadodb::Endpoint::DomainType::IPV4);
  CHECK_ENDPOINT_FEATURE(client, "ssl://127.0.0.1:8529", domainType, avocadodb::Endpoint::DomainType::IPV4);
  CHECK_ENDPOINT_FEATURE(client, "ssl://localhost:8529", domainType, avocadodb::Endpoint::DomainType::IPV4);
  CHECK_ENDPOINT_FEATURE(client, "ssl://www.avocadodb.org:8529", domainType, avocadodb::Endpoint::DomainType::IPV4);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[127.0.0.1]", domainType, avocadodb::Endpoint::DomainType::IPV6);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[::]", domainType, avocadodb::Endpoint::DomainType::IPV6);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[127.0.0.1]:8529", domainType, avocadodb::Endpoint::DomainType::IPV6);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[::]:8529", domainType, avocadodb::Endpoint::DomainType::IPV6);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[2001:0db8:0000:0000:0000:ff00:0042:8329]:8529", domainType, avocadodb::Endpoint::DomainType::IPV6);
  CHECK_ENDPOINT_FEATURE(client, "SSL://127.0.0.1", domainType, avocadodb::Endpoint::DomainType::IPV4);
  CHECK_ENDPOINT_FEATURE(client, "Ssl://127.0.0.1", domainType, avocadodb::Endpoint::DomainType::IPV4);
  CHECK_ENDPOINT_FEATURE(client, "sSL://127.0.0.1", domainType, avocadodb::Endpoint::DomainType::IPV4);
  CHECK_ENDPOINT_FEATURE(client, "http@ssl://127.0.0.1", domainType, avocadodb::Endpoint::DomainType::IPV4);
  
#ifndef _WIN32
  CHECK_ENDPOINT_FEATURE(client, "unix:///tmp/socket", domainType, avocadodb::Endpoint::DomainType::UNIX);
  CHECK_ENDPOINT_FEATURE(client, "unix:///tmp/socket/avocado.sock", domainType, avocadodb::Endpoint::DomainType::UNIX);
  CHECK_ENDPOINT_FEATURE(client, "UNIX:///tmp/socket", domainType, avocadodb::Endpoint::DomainType::UNIX);
  CHECK_ENDPOINT_FEATURE(client, "Unix:///tmp/socket", domainType, avocadodb::Endpoint::DomainType::UNIX);
  CHECK_ENDPOINT_FEATURE(client, "uNIX:///tmp/socket", domainType, avocadodb::Endpoint::DomainType::UNIX);
  CHECK_ENDPOINT_FEATURE(client, "http@unix:///tmp/socket", domainType, avocadodb::Endpoint::DomainType::UNIX);
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// @brief test ports
////////////////////////////////////////////////////////////////////////////////

SECTION("EndpointPorts") {
  Endpoint* e;

  CHECK_ENDPOINT_FEATURE(client, "tcp://127.0.0.1", port, EndpointIp::_defaultPortHttp);
  CHECK_ENDPOINT_FEATURE(client, "tcp://localhost", port, EndpointIp::_defaultPortHttp);
  CHECK_ENDPOINT_FEATURE(client, "tcp://www.avocadodb.org", port, EndpointIp::_defaultPortHttp); 
  CHECK_ENDPOINT_FEATURE(client, "tcp://127.0.0.1:8529", port, 8529);
  CHECK_ENDPOINT_FEATURE(client, "tcp://127.0.0.1:8532", port, 8532);
  CHECK_ENDPOINT_FEATURE(client, "tcp://127.0.0.1:80", port, 80);
  CHECK_ENDPOINT_FEATURE(client, "tcp://127.0.0.1:443", port, 443);
  CHECK_ENDPOINT_FEATURE(client, "tcp://127.0.0.1:65535", port, 65535);
  CHECK_ENDPOINT_FEATURE(client, "tcp://localhost:8529", port, 8529);
  CHECK_ENDPOINT_FEATURE(client, "tcp://localhost:8532", port, 8532);
  CHECK_ENDPOINT_FEATURE(client, "tcp://localhost:80", port, 80);
  CHECK_ENDPOINT_FEATURE(client, "tcp://localhost:443", port, 443);
  CHECK_ENDPOINT_FEATURE(client, "tcp://localhost:65535", port, 65535);
  CHECK_ENDPOINT_FEATURE(client, "tcp://www.avocadodb.org:8529", port, 8529);
  CHECK_ENDPOINT_FEATURE(client, "http@tcp://www.avocadodb.org:8529", port, 8529);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[127.0.0.1]", port, EndpointIp::_defaultPortHttp);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[::]", port, EndpointIp::_defaultPortHttp);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[127.0.0.1]:8529", port, 8529);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[127.0.0.1]:8532", port, 8532);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[127.0.0.1]:80", port, 80);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[127.0.0.1]:443", port, 443);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[127.0.0.1]:65535", port, 65535);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[::]:8529", port, 8529);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[::]:8532", port, 8532);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[::]:80", port, 80);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[::]:443", port, 443);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[::]:65535", port, 65535);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[2001:0db8:0000:0000:0000:ff00:0042:8329]:666", port, 666);
  CHECK_ENDPOINT_FEATURE(client, "http@tcp://[2001:0db8:0000:0000:0000:ff00:0042:8329]:666", port, 666);
  
  CHECK_ENDPOINT_FEATURE(client, "ssl://127.0.0.1", port, EndpointIp::_defaultPortHttp);
  CHECK_ENDPOINT_FEATURE(client, "ssl://localhost", port, EndpointIp::_defaultPortHttp);
  CHECK_ENDPOINT_FEATURE(client, "ssl://www.avocadodb.org", port, EndpointIp::_defaultPortHttp);
  CHECK_ENDPOINT_FEATURE(client, "ssl://127.0.0.1:8529", port, 8529);
  CHECK_ENDPOINT_FEATURE(client, "ssl://127.0.0.1:8532", port, 8532);
  CHECK_ENDPOINT_FEATURE(client, "ssl://127.0.0.1:80", port, 80);
  CHECK_ENDPOINT_FEATURE(client, "ssl://127.0.0.1:443", port, 443);
  CHECK_ENDPOINT_FEATURE(client, "ssl://127.0.0.1:65535", port, 65535);
  CHECK_ENDPOINT_FEATURE(client, "ssl://localhost:8529", port, 8529);
  CHECK_ENDPOINT_FEATURE(client, "ssl://localhost:8532", port, 8532);
  CHECK_ENDPOINT_FEATURE(client, "ssl://localhost:80", port, 80);
  CHECK_ENDPOINT_FEATURE(client, "ssl://localhost:443", port, 443);
  CHECK_ENDPOINT_FEATURE(client, "ssl://localhost:65535", port, 65535);
  CHECK_ENDPOINT_FEATURE(client, "ssl://www.avocadodb.org:8529", port, 8529);
  CHECK_ENDPOINT_FEATURE(client, "http@ssl://www.avocadodb.org:8529", port, 8529);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[127.0.0.1]", port, EndpointIp::_defaultPortHttp);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[::]", port, EndpointIp::_defaultPortHttp);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[127.0.0.1]:8529", port, 8529);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[127.0.0.1]:8532", port, 8532);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[127.0.0.1]:80", port, 80);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[127.0.0.1]:443", port, 443);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[127.0.0.1]:65535", port, 65535);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[::]:8529", port, 8529);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[::]:8532", port, 8532);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[::]:80", port, 80);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[::]:443", port, 443);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[::]:65535", port, 65535);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[2001:0db8:0000:0000:0000:ff00:0042:8329]:666", port, 666);
  CHECK_ENDPOINT_FEATURE(client, "http@ssl://[2001:0db8:0000:0000:0000:ff00:0042:8329]:666", port, 666);
  
#ifndef _WIN32
  CHECK_ENDPOINT_FEATURE(client, "unix:///tmp/socket", port, 0);
  CHECK_ENDPOINT_FEATURE(client, "unix:///tmp/socket/avocado.sock", port, 0);
  CHECK_ENDPOINT_FEATURE(client, "http@unix:///tmp/socket/avocado.sock", port, 0);
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// @brief test encryption
////////////////////////////////////////////////////////////////////////////////

SECTION("EndpointEncryption") {
  Endpoint* e;

  CHECK_ENDPOINT_FEATURE(client, "tcp://127.0.0.1", encryption, avocadodb::Endpoint::EncryptionType::NONE);
  CHECK_ENDPOINT_FEATURE(client, "tcp://localhost", encryption, avocadodb::Endpoint::EncryptionType::NONE);
  CHECK_ENDPOINT_FEATURE(client, "tcp://www.avocadodb.org", encryption, avocadodb::Endpoint::EncryptionType::NONE);
  CHECK_ENDPOINT_FEATURE(client, "tcp://127.0.0.1:8529", encryption, avocadodb::Endpoint::EncryptionType::NONE);
  CHECK_ENDPOINT_FEATURE(client, "tcp://localhost:8529", encryption, avocadodb::Endpoint::EncryptionType::NONE);
  CHECK_ENDPOINT_FEATURE(client, "tcp://www.avocadodb.org:8529", encryption, avocadodb::Endpoint::EncryptionType::NONE);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[127.0.0.1]", encryption, avocadodb::Endpoint::EncryptionType::NONE);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[::]", encryption, avocadodb::Endpoint::EncryptionType::NONE);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[127.0.0.1]:8529", encryption, avocadodb::Endpoint::EncryptionType::NONE);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[::]:8529", encryption, avocadodb::Endpoint::EncryptionType::NONE);
  CHECK_ENDPOINT_FEATURE(client, "tcp://[2001:0db8:0000:0000:0000:ff00:0042:8329]:666", encryption, avocadodb::Endpoint::EncryptionType::NONE);
  CHECK_ENDPOINT_FEATURE(client, "http@tcp://[2001:0db8:0000:0000:0000:ff00:0042:8329]:666", encryption, avocadodb::Endpoint::EncryptionType::NONE);
  
  CHECK_ENDPOINT_FEATURE(client, "ssl://127.0.0.1", encryption, avocadodb::Endpoint::EncryptionType::SSL);
  CHECK_ENDPOINT_FEATURE(client, "ssl://localhost", encryption, avocadodb::Endpoint::EncryptionType::SSL);
  CHECK_ENDPOINT_FEATURE(client, "ssl://www.avocadodb.org", encryption, avocadodb::Endpoint::EncryptionType::SSL);
  CHECK_ENDPOINT_FEATURE(client, "ssl://127.0.0.1:8529", encryption, avocadodb::Endpoint::EncryptionType::SSL);
  CHECK_ENDPOINT_FEATURE(client, "ssl://localhost:8529", encryption, avocadodb::Endpoint::EncryptionType::SSL);
  CHECK_ENDPOINT_FEATURE(client, "ssl://www.avocadodb.org:8529", encryption, avocadodb::Endpoint::EncryptionType::SSL);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[127.0.0.1]", encryption, avocadodb::Endpoint::EncryptionType::SSL);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[::]", encryption, avocadodb::Endpoint::EncryptionType::SSL);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[127.0.0.1]:8529", encryption, avocadodb::Endpoint::EncryptionType::SSL);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[2001:0db8:0000:0000:0000:ff00:0042:8329]:666", encryption, avocadodb::Endpoint::EncryptionType::SSL);
  CHECK_ENDPOINT_FEATURE(client, "ssl://[::]:8529", encryption, avocadodb::Endpoint::EncryptionType::SSL);
  CHECK_ENDPOINT_FEATURE(client, "SSL://[::]:8529", encryption, avocadodb::Endpoint::EncryptionType::SSL);
  CHECK_ENDPOINT_FEATURE(client, "Ssl://[::]:8529", encryption, avocadodb::Endpoint::EncryptionType::SSL);
  CHECK_ENDPOINT_FEATURE(client, "sSL://[::]:8529", encryption, avocadodb::Endpoint::EncryptionType::SSL);
  CHECK_ENDPOINT_FEATURE(client, "http@ssl://[::]:8529", encryption, avocadodb::Endpoint::EncryptionType::SSL);
  
#ifndef _WIN32
  CHECK_ENDPOINT_FEATURE(client, "unix:///tmp/socket", encryption, avocadodb::Endpoint::EncryptionType::NONE);
  CHECK_ENDPOINT_FEATURE(client, "unix:///tmp/socket/avocado.sock", encryption, avocadodb::Endpoint::EncryptionType::NONE);
  CHECK_ENDPOINT_FEATURE(client, "UNIX:///tmp/socket/avocado.sock", encryption, avocadodb::Endpoint::EncryptionType::NONE);
  CHECK_ENDPOINT_FEATURE(client, "Unix:///tmp/socket/avocado.sock", encryption, avocadodb::Endpoint::EncryptionType::NONE);
  CHECK_ENDPOINT_FEATURE(client, "uNIX:///tmp/socket/avocado.sock", encryption, avocadodb::Endpoint::EncryptionType::NONE);
  CHECK_ENDPOINT_FEATURE(client, "http@unix:///tmp/socket/avocado.sock", encryption, avocadodb::Endpoint::EncryptionType::NONE);
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// @brief test host
////////////////////////////////////////////////////////////////////////////////

SECTION("EndpointHost") {
  Endpoint* e;

  CHECK_ENDPOINT_FEATURE(client, "tcp://127.0.0.1", host, "127.0.0.1");
  CHECK_ENDPOINT_FEATURE(client, "tcp://localhost", host, "127.0.0.1");
  CHECK_ENDPOINT_FEATURE(client, "tcp://www.avocadodb.org", host, "www.avocadodb.org");
  CHECK_ENDPOINT_FEATURE(client, "tcp://avocadodb.org", host, "avocadodb.org");
  CHECK_ENDPOINT_FEATURE(client, "tcp://DE.triagens.AvocadoDB.org", host, "de.triagens.avocadodb.org");
  CHECK_ENDPOINT_FEATURE(client, "tcp://192.168.173.13:8529", host, "192.168.173.13");
  CHECK_ENDPOINT_FEATURE(client, "tcp://127.0.0.1:8529", host, "127.0.0.1");
  CHECK_ENDPOINT_FEATURE(client, "tcp://localhost:8529", host, "127.0.0.1");
  CHECK_ENDPOINT_FEATURE(client, "tcp://www.avocadodb.org:8529", host, "www.avocadodb.org");
  CHECK_ENDPOINT_FEATURE(client, "tcp://avocadodb.org:8529", host, "avocadodb.org");
  CHECK_ENDPOINT_FEATURE(client, "tcp://[127.0.0.1]", host, "127.0.0.1");
  CHECK_ENDPOINT_FEATURE(client, "tcp://[::]", host, "::");
  CHECK_ENDPOINT_FEATURE(client, "tcp://[127.0.0.1]:8529", host, "127.0.0.1");
  CHECK_ENDPOINT_FEATURE(client, "tcp://[::]:8529", host, "::");
  CHECK_ENDPOINT_FEATURE(client, "tcp://[2001:0db8:0000:0000:0000:ff00:0042:8329]", host, "2001:0db8:0000:0000:0000:ff00:0042:8329");
  CHECK_ENDPOINT_FEATURE(client, "tcp://[2001:0db8:0000:0000:0000:ff00:0042:8329]:8529", host, "2001:0db8:0000:0000:0000:ff00:0042:8329");
  CHECK_ENDPOINT_FEATURE(client, "http@tcp://[::]:8529", host, "::");
  
  CHECK_ENDPOINT_FEATURE(client, "ssl://127.0.0.1", host, "127.0.0.1");
  CHECK_ENDPOINT_FEATURE(client, "ssl://localhost", host, "127.0.0.1");
  CHECK_ENDPOINT_FEATURE(client, "ssl://www.avocadodb.org", host, "www.avocadodb.org");
  CHECK_ENDPOINT_FEATURE(client, "ssl://avocadodb.org", host, "avocadodb.org");
  CHECK_ENDPOINT_FEATURE(client, "ssl://DE.triagens.AvocadoDB.org", host, "de.triagens.avocadodb.org");
  CHECK_ENDPOINT_FEATURE(client, "ssl://192.168.173.13:8529", host, "192.168.173.13");
  CHECK_ENDPOINT_FEATURE(client, "ssl://localhost:8529", host, "127.0.0.1");
  CHECK_ENDPOINT_FEATURE(client, "ssl://www.avocadodb.org:8529", host, "www.avocadodb.org");
  CHECK_ENDPOINT_FEATURE(client, "ssl://[127.0.0.1]", host, "127.0.0.1");
  CHECK_ENDPOINT_FEATURE(client, "ssl://[::]", host, "::");
  CHECK_ENDPOINT_FEATURE(client, "ssl://[127.0.0.1]:8529", host, "127.0.0.1");
  CHECK_ENDPOINT_FEATURE(client, "ssl://[::]:8529", host, "::");
  CHECK_ENDPOINT_FEATURE(client, "ssl://[2001:0db8:0000:0000:0000:ff00:0042:8329]", host, "2001:0db8:0000:0000:0000:ff00:0042:8329");
  CHECK_ENDPOINT_FEATURE(client, "ssl://[2001:0db8:0000:0000:0000:ff00:0042:8329]:8529", host, "2001:0db8:0000:0000:0000:ff00:0042:8329");
  CHECK_ENDPOINT_FEATURE(client, "http@ssl://[::]:8529", host, "::");
  
#ifndef _WIN32
  CHECK_ENDPOINT_FEATURE(client, "unix:///tmp/socket", host, "localhost");
  CHECK_ENDPOINT_FEATURE(client, "unix:///tmp/socket/avocado.sock", host, "localhost");
  CHECK_ENDPOINT_FEATURE(client, "http@unix:///tmp/socket/avocado.sock", host, "localhost");
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// @brief test hoststring
////////////////////////////////////////////////////////////////////////////////

SECTION("EndpointHostString") {
  Endpoint* e;

  CHECK_ENDPOINT_FEATURE(client, "tcp://127.0.0.1", hostAndPort, "127.0.0.1:8529");
  CHECK_ENDPOINT_FEATURE(client, "tcp://localhost", hostAndPort, "127.0.0.1:8529");
  CHECK_ENDPOINT_FEATURE(client, "tcp://www.avocadodb.org", hostAndPort, "www.avocadodb.org:8529");
  CHECK_ENDPOINT_FEATURE(client, "tcp://avocadodb.org", hostAndPort, "avocadodb.org:8529");
  CHECK_ENDPOINT_FEATURE(client, "tcp://DE.triagens.AvocadoDB.org", hostAndPort, "de.triagens.avocadodb.org:8529");
  CHECK_ENDPOINT_FEATURE(client, "tcp://192.168.173.13:8529", hostAndPort, "192.168.173.13:8529");
  CHECK_ENDPOINT_FEATURE(client, "tcp://192.168.173.13:678", hostAndPort, "192.168.173.13:678");
  CHECK_ENDPOINT_FEATURE(client, "tcp://127.0.0.1:8529", hostAndPort, "127.0.0.1:8529");
  CHECK_ENDPOINT_FEATURE(client, "tcp://127.0.0.1:44", hostAndPort, "127.0.0.1:44");
  CHECK_ENDPOINT_FEATURE(client, "tcp://localhost:8529", hostAndPort, "127.0.0.1:8529");
  CHECK_ENDPOINT_FEATURE(client, "tcp://localhost:65535", hostAndPort, "127.0.0.1:65535");
  CHECK_ENDPOINT_FEATURE(client, "tcp://www.avocadodb.org:8529", hostAndPort, "www.avocadodb.org:8529");
  CHECK_ENDPOINT_FEATURE(client, "tcp://avocadodb.org:8529", hostAndPort, "avocadodb.org:8529");
  CHECK_ENDPOINT_FEATURE(client, "tcp://[127.0.0.1]", hostAndPort, "[127.0.0.1]:8529");
  CHECK_ENDPOINT_FEATURE(client, "tcp://[::]", hostAndPort, "[::]:8529");
  CHECK_ENDPOINT_FEATURE(client, "tcp://[127.0.0.1]:8529", hostAndPort, "[127.0.0.1]:8529");
  CHECK_ENDPOINT_FEATURE(client, "tcp://[127.0.0.1]:80", hostAndPort, "[127.0.0.1]:80");
  CHECK_ENDPOINT_FEATURE(client, "tcp://[127.0.0.1]:555", hostAndPort, "[127.0.0.1]:555");
  CHECK_ENDPOINT_FEATURE(client, "tcp://[127.0.0.1]:65535", hostAndPort, "[127.0.0.1]:65535");
  CHECK_ENDPOINT_FEATURE(client, "tcp://[::]:8529", hostAndPort, "[::]:8529");
  CHECK_ENDPOINT_FEATURE(client, "tcp://[::]:80", hostAndPort, "[::]:80");
  CHECK_ENDPOINT_FEATURE(client, "tcp://[::]:8080", hostAndPort, "[::]:8080");
  CHECK_ENDPOINT_FEATURE(client, "tcp://[2001:0db8:0000:0000:0000:ff00:0042:8329]", hostAndPort, "[2001:0db8:0000:0000:0000:ff00:0042:8329]:8529");
  CHECK_ENDPOINT_FEATURE(client, "tcp://[2001:0db8:0000:0000:0000:ff00:0042:8329]:8529", hostAndPort, "[2001:0db8:0000:0000:0000:ff00:0042:8329]:8529");
  CHECK_ENDPOINT_FEATURE(client, "tcp://[2001:0db8:0000:0000:0000:ff00:0042:8329]:777", hostAndPort, "[2001:0db8:0000:0000:0000:ff00:0042:8329]:777");
  CHECK_ENDPOINT_FEATURE(client, "http@tcp://[2001:0db8:0000:0000:0000:ff00:0042:8329]:777", hostAndPort, "[2001:0db8:0000:0000:0000:ff00:0042:8329]:777");
  
  CHECK_ENDPOINT_FEATURE(client, "ssl://127.0.0.1", hostAndPort, "127.0.0.1:8529");
  CHECK_ENDPOINT_FEATURE(client, "ssl://localhost", hostAndPort, "127.0.0.1:8529");
  CHECK_ENDPOINT_FEATURE(client, "ssl://www.avocadodb.org", hostAndPort, "www.avocadodb.org:8529");
  CHECK_ENDPOINT_FEATURE(client, "ssl://avocadodb.org", hostAndPort, "avocadodb.org:8529");
  CHECK_ENDPOINT_FEATURE(client, "ssl://DE.triagens.AvocadoDB.org", hostAndPort, "de.triagens.avocadodb.org:8529");
  CHECK_ENDPOINT_FEATURE(client, "ssl://192.168.173.13:8529", hostAndPort, "192.168.173.13:8529");
  CHECK_ENDPOINT_FEATURE(client, "ssl://192.168.173.13:1234", hostAndPort, "192.168.173.13:1234");
  CHECK_ENDPOINT_FEATURE(client, "ssl://localhost:8529", hostAndPort, "127.0.0.1:8529");
  CHECK_ENDPOINT_FEATURE(client, "ssl://localhost:5", hostAndPort, "127.0.0.1:5");
  CHECK_ENDPOINT_FEATURE(client, "ssl://www.avocadodb.org:8529", hostAndPort, "www.avocadodb.org:8529");
  CHECK_ENDPOINT_FEATURE(client, "ssl://www.avocadodb.org:12345", hostAndPort, "www.avocadodb.org:12345");
  CHECK_ENDPOINT_FEATURE(client, "ssl://[127.0.0.1]", hostAndPort, "[127.0.0.1]:8529");
  CHECK_ENDPOINT_FEATURE(client, "ssl://[::]", hostAndPort, "[::]:8529");
  CHECK_ENDPOINT_FEATURE(client, "ssl://[127.0.0.1]:8529", hostAndPort, "[127.0.0.1]:8529");
  CHECK_ENDPOINT_FEATURE(client, "ssl://[127.0.0.1]:32768", hostAndPort, "[127.0.0.1]:32768");
  CHECK_ENDPOINT_FEATURE(client, "ssl://[::]:8529", hostAndPort, "[::]:8529");
  CHECK_ENDPOINT_FEATURE(client, "ssl://[2001:0db8:0000:0000:0000:ff00:0042:8329]", hostAndPort, "[2001:0db8:0000:0000:0000:ff00:0042:8329]:8529");
  CHECK_ENDPOINT_FEATURE(client, "ssl://[2001:0db8:0000:0000:0000:ff00:0042:8329]:8529", hostAndPort, "[2001:0db8:0000:0000:0000:ff00:0042:8329]:8529");
  CHECK_ENDPOINT_FEATURE(client, "ssl://[2001:0db8:0000:0000:0000:ff00:0042:8329]:994", hostAndPort, "[2001:0db8:0000:0000:0000:ff00:0042:8329]:994");
  CHECK_ENDPOINT_FEATURE(client, "http@ssl://[2001:0db8:0000:0000:0000:ff00:0042:8329]:994", hostAndPort, "[2001:0db8:0000:0000:0000:ff00:0042:8329]:994");
  
#ifndef _WIN32
  CHECK_ENDPOINT_FEATURE(client, "unix:///tmp/socket", hostAndPort, "localhost");
  CHECK_ENDPOINT_FEATURE(client, "unix:///tmp/socket/avocado.sock", hostAndPort, "localhost");
  CHECK_ENDPOINT_FEATURE(client, "http@unix:///tmp/socket/avocado.sock", hostAndPort, "localhost");
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// @brief test isconnected
////////////////////////////////////////////////////////////////////////////////

SECTION("EndpointIsConnectedServer1") {
  Endpoint* e;

  e = avocadodb::Endpoint::serverFactory("tcp://127.0.0.1", 1, true);
  CHECK(false == e->isConnected());
  delete e;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief test isconnected
////////////////////////////////////////////////////////////////////////////////

SECTION("EndpointIsConnectedServer2") {
  Endpoint* e;

  e = avocadodb::Endpoint::serverFactory("ssl://127.0.0.1", 1, true);
  CHECK(false == e->isConnected());
  delete e;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief test isconnected
////////////////////////////////////////////////////////////////////////////////

#ifndef _WIN32
SECTION("EndpointIsConnectedServer3") {
  Endpoint* e;

  e = avocadodb::Endpoint::serverFactory("unix:///tmp/socket", 1, true);
  CHECK(false == e->isConnected());
  delete e;
}
#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief test isconnected
////////////////////////////////////////////////////////////////////////////////

SECTION("EndpointIsConnectedClient1") {
  Endpoint* e;

  e = avocadodb::Endpoint::clientFactory("tcp://127.0.0.1");
  CHECK(false == e->isConnected());
  delete e;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief test isconnected
////////////////////////////////////////////////////////////////////////////////

SECTION("EndpointIsConnectedClient2") {
  Endpoint* e;

  e = avocadodb::Endpoint::clientFactory("ssl://127.0.0.1");
  CHECK(false == e->isConnected());
  delete e;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief test isconnected
////////////////////////////////////////////////////////////////////////////////

#ifndef _WIN32
SECTION("EndpointIsConnectedClient3") {
  Endpoint* e;

  e = avocadodb::Endpoint::clientFactory("unix:///tmp/socket");
  CHECK(false == e->isConnected());
  delete e;
}
#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief test server endpoint
////////////////////////////////////////////////////////////////////////////////

SECTION("EndpointServerTcpIpv4WithPort") {
  Endpoint* e;

  e = avocadodb::Endpoint::serverFactory("tcp://127.0.0.1:667", 1, true);
  CHECK("http+tcp://127.0.0.1:667" == e->specification());
  CHECK(avocadodb::Endpoint::EndpointType::SERVER == e->type());
  CHECK(avocadodb::Endpoint::DomainType::IPV4 == e->domainType());
  CHECK(avocadodb::Endpoint::EncryptionType::NONE == e->encryption());
  CHECK(AF_INET == e->domain());
  CHECK("127.0.0.1" == e->host());
  CHECK(667 == e->port());
  CHECK("127.0.0.1:667" == e->hostAndPort());
  CHECK(false == e->isConnected());
  delete e;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief test server endpoint
////////////////////////////////////////////////////////////////////////////////

#ifndef _WIN32
SECTION("EndpointServerUnix") {
  Endpoint* e;

  e = avocadodb::Endpoint::serverFactory("unix:///path/to/avocado.sock", 1, true);
  CHECK("http+unix:///path/to/avocado.sock" == e->specification());
  CHECK(avocadodb::Endpoint::EndpointType::SERVER == e->type());
  CHECK(avocadodb::Endpoint::DomainType::UNIX == e->domainType());
  CHECK(avocadodb::Endpoint::EncryptionType::NONE == e->encryption());
  CHECK(AF_UNIX == e->domain());
  CHECK("localhost" == e->host());
  CHECK(0 == e->port());
  CHECK("localhost" == e->hostAndPort());
  CHECK(false == e->isConnected());
  delete e;
}
#endif

////////////////////////////////////////////////////////////////////////////////
/// @brief test client endpoint
////////////////////////////////////////////////////////////////////////////////

SECTION("EndpointClientSslIpV6WithPortHttp") {
  Endpoint* e;

  e = avocadodb::Endpoint::clientFactory("http+ssl://[0001:0002:0003:0004:0005:0006:0007:0008]:43425");
  CHECK("http+ssl://[0001:0002:0003:0004:0005:0006:0007:0008]:43425" == e->specification());
  CHECK(avocadodb::Endpoint::EndpointType::CLIENT == e->type());
  CHECK(avocadodb::Endpoint::DomainType::IPV6 == e->domainType());
  CHECK(avocadodb::Endpoint::EncryptionType::SSL == e->encryption());
  CHECK(AF_INET6 == e->domain());
  CHECK("0001:0002:0003:0004:0005:0006:0007:0008" == e->host());
  CHECK(43425 == e->port());
  CHECK("[0001:0002:0003:0004:0005:0006:0007:0008]:43425" == e->hostAndPort());
  CHECK(false == e->isConnected());
  delete e;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief test client endpoint
////////////////////////////////////////////////////////////////////////////////

SECTION("EndpointClientTcpIpv6WithoutPort") {
  Endpoint* e;

  e = avocadodb::Endpoint::clientFactory("tcp://[::]");
  CHECK("http+tcp://[::]:8529" == e->specification());
  CHECK(avocadodb::Endpoint::EndpointType::CLIENT == e->type());
  CHECK(avocadodb::Endpoint::DomainType::IPV6 == e->domainType());
  CHECK(avocadodb::Endpoint::EncryptionType::NONE == e->encryption());
  CHECK(AF_INET6 == e->domain());
  CHECK("::" == e->host());
  CHECK(8529 == e->port());
  CHECK("[::]:8529" == e->hostAndPort());
  CHECK(false == e->isConnected());
  delete e;
}
}
// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|// --SECTION--\\|/// @\\}\\)"
// End:
