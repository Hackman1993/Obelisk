//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_EXAMPLE_COMMON_SERVER_CERTIFICATE_HPP
#define BOOST_BEAST_EXAMPLE_COMMON_SERVER_CERTIFICATE_HPP

#include <boost/asio/buffer.hpp>
#include <boost/asio/ssl/context.hpp>
#include <cstddef>
#include <memory>
#include <iostream>

/*  Load a signed certificate into the ssl context, and configure
    the context for use with a server.

    For this to work with the browser or operating system, it is
    necessary to import the "Beast Test CA" certificate into
    the local certificate store, browser, or operating system
    depending on your environment Please see the documentation
    accompanying the Beast certificate for more details.
*/
inline
void
load_server_certificate(boost::asio::ssl::context& ctx)
{
  /*
      The certificate was generated from bash on Ubuntu (OpenSSL 1.1.1f) using:

      openssl dhparam -out dh.pem 2048
      openssl req -newkey rsa:2048 -nodes -keyout key.pem -x509 -days 10000 -out cert.pem -subj "/C=US/ST=CA/L=Los Angeles/O=Beast/CN=www.example.com"
  */

  std::string const cert =
      "-----BEGIN CERTIFICATE-----\n"
      "MIIF9DCCBNygAwIBAgIQDW11eWWWSwXux1KamWaTjjANBgkqhkiG9w0BAQsFADBu\n"
      "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
      "d3cuZGlnaWNlcnQuY29tMS0wKwYDVQQDEyRFbmNyeXB0aW9uIEV2ZXJ5d2hlcmUg\n"
      "RFYgVExTIENBIC0gRzEwHhcNMjIwOTA4MDAwMDAwWhcNMjMwOTA4MjM1OTU5WjAb\n"
      "MRkwFwYDVQQDExBhc3NldHMuemstdGVjLmNuMIIBIjANBgkqhkiG9w0BAQEFAAOC\n"
      "AQ8AMIIBCgKCAQEAq4ccsLp9jDi9/n4HTIBV9BiYnqkzJn6+jmRuNdm21tRDniZh\n"
      "fV/Bf3ywyXtPoTyXC9MIPtDEE8VA6fTL27PW8Hz77hPzw2eGBNY86S34IWC3YiEJ\n"
      "luIXHYFoJz3pnJlntzNxmACNirN6dhRbh4BO/ui35GWy7D5D39pDoGJeFzHooLGk\n"
      "m2fws3Za8I/W31Fervkhxv73siOEZyRXQcxeC5brTYXcrVfdrn599TCCfX13hfAo\n"
      "EhC/4nNQFolYeN9BB4akBB4v3EL+T1Xn9CnThhLq763LeSBPBhhFDi7uuzw3r3My\n"
      "+XevRbuIE2LTvGYwTN7RR0yPRNit/Om25BOFiwIDAQABo4IC3zCCAtswHwYDVR0j\n"
      "BBgwFoAUVXRPsnJP9WC6UNHX5lFcmgGHGtcwHQYDVR0OBBYEFAFFFoLsscXtZhjg\n"
      "LZ6Ft44rq1bXMBsGA1UdEQQUMBKCEGFzc2V0cy56ay10ZWMuY24wDgYDVR0PAQH/\n"
      "BAQDAgWgMB0GA1UdJQQWMBQGCCsGAQUFBwMBBggrBgEFBQcDAjA+BgNVHSAENzA1\n"
      "MDMGBmeBDAECATApMCcGCCsGAQUFBwIBFhtodHRwOi8vd3d3LmRpZ2ljZXJ0LmNv\n"
      "bS9DUFMwgYAGCCsGAQUFBwEBBHQwcjAkBggrBgEFBQcwAYYYaHR0cDovL29jc3Au\n"
      "ZGlnaWNlcnQuY29tMEoGCCsGAQUFBzAChj5odHRwOi8vY2FjZXJ0cy5kaWdpY2Vy\n"
      "dC5jb20vRW5jcnlwdGlvbkV2ZXJ5d2hlcmVEVlRMU0NBLUcxLmNydDAJBgNVHRME\n"
      "AjAAMIIBfQYKKwYBBAHWeQIEAgSCAW0EggFpAWcAdQDoPtDaPvUGNTLnVyi8iWvJ\n"
      "A9PL0RFr7Otp4Xd9bQa9bgAAAYMcDporAAAEAwBGMEQCIGrbSoS7BZtmEGhpwx5w\n"
      "MgRu6koRznFJRlmAFUTq+gxQAiBs9T+i6l0UaFw89BRnOMi5AJ6+uNXTMW6Sf/fy\n"
      "ZUjzuQB2ADXPGRu/sWxXvw+tTG1Cy7u2JyAmUeo/4SrvqAPDO9ZMAAABgxwOmnUA\n"
      "AAQDAEcwRQIgQoTM9mb651JfSKYKD7CNa489tC+0QvVMP+Z8TLFcbhICIQCW7rhG\n"
      "ND2bPFHHM70p7Ros3SpEW3n5vMA57zO+Bc1E1AB2ALNzdwfhhFD4Y4bWBancEQlK\n"
      "eS2xZwwLh9zwAw55NqWaAAABgxwOmuMAAAQDAEcwRQIhAJf7MJxehhRqO7kGS7+3\n"
      "Osqc5Axapblg4FOl6NkpMSXLAiAIyxguqNkM6pB/gXlq+SOTaEY8hx3j62jmcWyb\n"
      "sFYj+zANBgkqhkiG9w0BAQsFAAOCAQEApron1mvKkszvcDgfBgQQn65gD5XA30Cz\n"
      "ppyLxwYkKfSgvYjxXB+zUB4GfG7pBa0GOmf5uW+FgwwZw/6BxrzSSm1JQLf1iSYV\n"
      "QhwgISmpi0YL/jj/i+kHFb9PrFo8a0HEvcbqrl/CXUq4qC8VCBuaQSQcaIIH55TO\n"
      "oksJPLZvjoKU8keJ4j7IxwMel1JfQcAx1FI0KkzqjK0//SGlcNrOVToDs7Bw8sJa\n"
      "XRbbsqo0CoT1HaF6JbSicM/nnkK7H8jwko+HrWtH8l96OcYNdD2Mwb0IdilPfgVI\n"
      "MpZi3p05vevJJnyjdqOCghN+iTn4YcoPkHETF0pGPdMzD5a6SXErtA==\n"
      "-----END CERTIFICATE-----";

  std::string const key =
      "-----BEGIN RSA PRIVATE KEY-----\n"
      "MIIEpAIBAAKCAQEAq4ccsLp9jDi9/n4HTIBV9BiYnqkzJn6+jmRuNdm21tRDniZh\n"
      "fV/Bf3ywyXtPoTyXC9MIPtDEE8VA6fTL27PW8Hz77hPzw2eGBNY86S34IWC3YiEJ\n"
      "luIXHYFoJz3pnJlntzNxmACNirN6dhRbh4BO/ui35GWy7D5D39pDoGJeFzHooLGk\n"
      "m2fws3Za8I/W31Fervkhxv73siOEZyRXQcxeC5brTYXcrVfdrn599TCCfX13hfAo\n"
      "EhC/4nNQFolYeN9BB4akBB4v3EL+T1Xn9CnThhLq763LeSBPBhhFDi7uuzw3r3My\n"
      "+XevRbuIE2LTvGYwTN7RR0yPRNit/Om25BOFiwIDAQABAoIBABdR4TatxA/iTwJg\n"
      "hAGEmSr8hRNdEZU0OhgF024C0XbXmPHRagOvlxEMFMTvvN3A7RAwerh/jYjakn68\n"
      "R1LxWlGLzHyLveJdGr1hmxpgo5F2Irh2oWUgC+B5DX34TuB5gr5HgjQWLygiSo3v\n"
      "Kpp7eUHyxb2Jjg/MT1IJzJPri7s/VtIsiIuxfUipj2NLv0uJe/7HkHqwJNNpi+IQ\n"
      "8To4jtCS7Rj8YLq0zlf3rSI2qtYSOmQe3gil/jUG55oQo4zxmnCzdTU2x/qb55U5\n"
      "vHYzPp/tJ/Se/h29C+6bCADxb5zrq+/TN5SJplnRj8ofl8v8GHoqcHXs0fWIS0mw\n"
      "pljq9pECgYEA1WEensEJ40Bl9gzoquqjOIj2tDI4wzGQuvaGLETOdz6gwKqUzCKF\n"
      "t8n3WaQ18ZQj2TkjQ3Xv+8nu4wg8FDUyXoMFv4uVXV5kvvlRZf1DZTEv76CjlBgO\n"
      "vrl61Uioi1WisK19w0ArR4WPo479SP6d3g4lI4Dm3nUZJO5R6LF3DCUCgYEAzcn3\n"
      "C9Kb2u9Go2KOrN31tFz1D3LaVM868gUx3pX3Q3TvVnvS9kAS/oII65pdi6k/ZtZy\n"
      "Yi58oCbZ6eVwNyy21/420xj5ElnmOrTaG8vPmCGirvI1hcu+tfKoKi8Z/v9zO2Zt\n"
      "5aVheAcy6hmlrG1WL3jaILEKnUOHqEKZadJDw+8CgYEAlhKJY6RnHOK1E3l+xn7i\n"
      "Lyf9Qz0zOO2KWkR6l3YJNEri6r7OikgZferZQb3Bhtfm4qdSCudO0uryTdED2D6V\n"
      "320NROuaMKHr/3kAa9fVZ71RdpqnkcfdzvxKzOkeSWkAVHygwAtIpTPlk2ZE8+rI\n"
      "A141dqP5PCplddj72Mm/7dUCgYBGreBA2kQXbOkZAqe4Lb/PA8JTXQIp8MbxCuG2\n"
      "7lDvhUESthKd6yHa8fR4XxtT9XxGKM8+c3LwWoerstbYzG28P5664C/joqbVcPoc\n"
      "Y+LYAG1sotnJK1JEEaS6YFUQZNJCC4shPaz9WZOeHdLuJttcEKjyY3weyWvVWZJx\n"
      "avtFVwKBgQCc1c+z6eGzBAUFoggwwrOckyQW15cbg9tMQlcxU2wCUoLNbGt9xZ/N\n"
      "pC3+FDYwgeYqJYnn75Lnod3mM3/KY5X5CWC5NqB1aed/JGaqjGEUNNYh4OBcbUeY\n"
      "lCSB37L7QfzoWEno7+CN7h7PhuTadgURiTBIRqP1xiG9I2VpuzGS1g==\n"
      "-----END RSA PRIVATE KEY-----";

  std::string const dh =
      "-----BEGIN DH PARAMETERS-----\n"
      "MIIBCAKCAQEArzQc5mpm0Fs8yahDeySj31JZlwEphUdZ9StM2D8+Fo7TMduGtSi+\n"
      "/HRWVwHcTFAgrxVdm+dl474mOUqqaz4MpzIb6+6OVfWHbQJmXPepZKyu4LgUPvY/\n"
      "4q3/iDMjIS0fLOu/bLuObwU5ccZmDgfhmz1GanRlTQOiYRty3FiOATWZBRh6uv4u\n"
      "tff4A9Bm3V9tLx9S6djq31w31Gl7OQhryodW28kc16t9TvO1BzcV3HjRPwpe701X\n"
      "oEEZdnZWANkkpR/m/pfgdmGPU66S2sXMHgsliViQWpDCYeehrvFRHEdR9NV+XJfC\n"
      "QMUk26jPTIVTLfXmmwU0u8vUkpR7LQKkwwIBAg==\n"
      "-----END DH PARAMETERS-----\n";

  ctx.set_password_callback(
      [](std::size_t,
         boost::asio::ssl::context_base::password_purpose)
      {
        return "123456";
      });

  ctx.set_options(
      boost::asio::ssl::context::default_workarounds |
      boost::asio::ssl::context::no_sslv3);

  ctx.use_certificate_chain(
      boost::asio::buffer(cert.data(), cert.size()));

  try {
    ctx.use_private_key(
        boost::asio::buffer(key.data(), key.size()),
        boost::asio::ssl::context::file_format::pem);
  } catch (std::exception& e){
    std::cout << e.what() <<std::endl;
  }

//  ctx.use_tmp_dh(
//      boost::asio::buffer(dh.data(), dh.size()));
}

#endif