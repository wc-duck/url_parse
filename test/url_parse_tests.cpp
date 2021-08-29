/*
    Simple parser for URL:s as specified by RFC1738 ( http://www.ietf.org/rfc/rfc1738.txt )

    version 1.1, July, 2021

	Copyright (C) 2021- Fredrik Kihlander

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.

	Fredrik Kihlander
*/

#define URL_PARSER_IMPLEMENTATION

#include "greatest.h"
#include "../url.h"

TEST full_url_parse()
{
	parsed_url* parsed = parse_url( "http://user:pass@testurl.com:8080/sub/resource.file?query#fragment", 0x0, 0 );
	if( parsed == 0x0 )
		FAILm( "failed to parse url" );

	ASSERT_STR_EQ(               "http", parsed->scheme );
	ASSERT_STR_EQ(        "testurl.com", parsed->host );
	ASSERT_STR_EQ( "/sub/resource.file", parsed->path );
	ASSERT_STR_EQ(               "user", parsed->user );
	ASSERT_STR_EQ(               "pass", parsed->pass );
	ASSERT_STR_EQ(              "query", parsed->query );
	ASSERT_STR_EQ(           "fragment", parsed->fragment );

	ASSERT_EQ( 8080, parsed->port );

	free( parsed );

	return GREATEST_TEST_RES_PASS;
}

TEST full_url_parse_win_style_path()
{
	parsed_url* parsed = parse_url( "http://user:pass@testurl.com:8080/e:/whoppa?query#fragment", 0x0, 0 );
	if( parsed == 0x0 )
		FAILm( "failed to parse url" );

	ASSERT_STR_EQ(        "http", parsed->scheme );
	ASSERT_STR_EQ( "testurl.com", parsed->host );
	ASSERT_STR_EQ(  "/e:/whoppa", parsed->path );
	ASSERT_STR_EQ(        "user", parsed->user );
	ASSERT_STR_EQ(        "pass", parsed->pass );
	ASSERT_STR_EQ(       "query", parsed->query );
	ASSERT_STR_EQ(    "fragment", parsed->fragment );

	ASSERT_EQ( 8080, parsed->port );

	free( parsed );

	return GREATEST_TEST_RES_PASS;
}

TEST url_scheme_is_lower_cased()
{
	{
		parsed_url* parsed = parse_url( "HTTP://test.com", 0x0, 0 );
		if( parsed == 0x0 )
			FAILm( "failed to parse url" );

		ASSERT_STR_EQ( "http", parsed->scheme );

		free( parsed );
	}

	{
		parsed_url* parsed = parse_url( "hTTp://test.com", 0x0, 0 );
		if( parsed == 0x0 )
			FAILm( "failed to parse url" );

		ASSERT_STR_EQ( "http", parsed->scheme );

		free( parsed );
	}

	return GREATEST_TEST_RES_PASS;
}

TEST url_no_scheme_with_port()
{
	parsed_url* parsed = parse_url( "testurl.com:8080", 0x0, 0 );
	if( parsed == 0x0 )
		FAILm( "failed to parse url" );

	ASSERT_STR_EQ( "testurl.com", parsed->host );
	ASSERT_STR_EQ( "/", parsed->path );

	ASSERT_EQ( 8080, parsed->port );
	ASSERT_EQ( 0x0,  parsed->scheme );
	ASSERT_EQ( 0x0,  parsed->user );
	ASSERT_EQ( 0x0,  parsed->pass );
	ASSERT_EQ( 0x0,  parsed->query );
	ASSERT_EQ( 0x0,  parsed->fragment );

	free( parsed );

	return GREATEST_TEST_RES_PASS;
}

TEST url_casing()
{
	parsed_url* parsed = parse_url( "WhoPPa://UsEr:Pa$$Word@teStUrL.cOm/whIhoWeeHa?qUEry#frAGment", 0x0, 0 );
	if( parsed == 0x0 )
		FAILm( "failed to parse url" );

	// scheme and host is case-insensitive according to spec, lowercasing it to make usage simpler.
	ASSERT_STR_EQ( "whoppa",      parsed->scheme );
	ASSERT_STR_EQ( "testurl.com", parsed->host );
	// ... user and pass is ofc ourse not!
	ASSERT_STR_EQ( "UsEr",        parsed->user );
	ASSERT_STR_EQ( "Pa$$Word",    parsed->pass );
	// ... same with path, query and fragment ...
	ASSERT_STR_EQ( "/whIhoWeeHa", parsed->path );
	ASSERT_STR_EQ( "qUEry",       parsed->query );
	ASSERT_STR_EQ( "frAGment",    parsed->fragment );

	ASSERT_EQ( 0,  parsed->port );

	free( parsed );

	return GREATEST_TEST_RES_PASS;
}

TEST url_no_host()
{
	parsed_url* parsed = parse_url( "file:///sub/resource.file", 0x0, 0 );
	if( parsed == 0x0 )
		FAILm( "failed to parse url" );

	ASSERT_STR_EQ(          "localhost", parsed->host );
	ASSERT_STR_EQ(               "file", parsed->scheme );
	ASSERT_STR_EQ( "/sub/resource.file", parsed->path );

	ASSERT_EQ( 0x0, parsed->user );
	ASSERT_EQ( 0x0, parsed->pass );
	ASSERT_EQ(   0, parsed->port );
	ASSERT_EQ( 0x0, parsed->query );
	ASSERT_EQ( 0x0, parsed->fragment );

	free( parsed );

	return GREATEST_TEST_RES_PASS;
}

TEST url_win_style_abs_path()
{
	parsed_url* parsed = parse_url( "file:///e:/sub/resource.file", 0x0, 0 );
	if( parsed == 0x0 )
		FAILm( "failed to parse url" );

	ASSERT_STR_EQ(            "localhost",  parsed->host );
	ASSERT_STR_EQ(                 "file",  parsed->scheme );
	ASSERT_STR_EQ( "/e:/sub/resource.file", parsed->path );

	ASSERT_EQ( 0x0, parsed->user );
	ASSERT_EQ( 0x0, parsed->pass );
	ASSERT_EQ(   0, parsed->port );
	ASSERT_EQ( 0x0, parsed->query );
	ASSERT_EQ( 0x0, parsed->fragment );

	free( parsed );

	return GREATEST_TEST_RES_PASS;
}

TEST url_win_style_abs_path_with_host()
{
	parsed_url* parsed = parse_url( "file://some_host/e:/sub/resource.file", 0x0, 0 );
	if( parsed == 0x0 )
		FAILm( "failed to parse url" );

	ASSERT_STR_EQ(             "some_host",  parsed->host );
	ASSERT_STR_EQ(                  "file",  parsed->scheme );
	ASSERT_STR_EQ( "/e:/sub/resource.file", parsed->path );

	ASSERT_EQ( 0x0, parsed->user );
	ASSERT_EQ( 0x0, parsed->pass );
	ASSERT_EQ(   0, parsed->port );
	ASSERT_EQ( 0x0, parsed->query );
	ASSERT_EQ( 0x0, parsed->fragment );

	free( parsed );

	return GREATEST_TEST_RES_PASS;
}

TEST url_win_style_abs_path_with_host_and_port()
{
	parsed_url* parsed = parse_url( "file://some_host:1337/e:/sub/resource.file", 0x0, 0 );
	if( parsed == 0x0 )
		FAILm( "failed to parse url" );

	ASSERT_STR_EQ(            "some_host",  parsed->host );
	ASSERT_STR_EQ(                 "file",  parsed->scheme );
	ASSERT_STR_EQ( "/e:/sub/resource.file", parsed->path );

	ASSERT_EQ( 0x0,  parsed->user );
	ASSERT_EQ( 0x0,  parsed->pass );
	ASSERT_EQ( 1337, parsed->port );
	ASSERT_EQ( 0x0, parsed->query );
	ASSERT_EQ( 0x0, parsed->fragment );

	free( parsed );

	return GREATEST_TEST_RES_PASS;
}

TEST default_port_parse()
{
	parsed_url* parsed;

	parsed = parse_url( "http://testurl.com", 0x0, 0 );
	if( parsed == 0x0 )
		FAILm( "failed to parse url" );

	ASSERT_STR_EQ(        "http", parsed->scheme );
	ASSERT_STR_EQ( "testurl.com", parsed->host );
	ASSERT_STR_EQ(           "/", parsed->path );
	ASSERT_EQ(  80, parsed->port );
	ASSERT_EQ( 0x0, parsed->user );
	ASSERT_EQ( 0x0, parsed->pass );
	ASSERT_EQ( 0x0, parsed->query );
	ASSERT_EQ( 0x0, parsed->fragment );

	free( parsed );

	parsed = parse_url( "ftp://testurl.com", 0x0, 0 );
	if( parsed == 0x0 )
		FAILm( "failed to parse url" );

	ASSERT_STR_EQ(         "ftp", parsed->scheme );
	ASSERT_STR_EQ( "testurl.com", parsed->host );
	ASSERT_STR_EQ(           "/", parsed->path );
	ASSERT_EQ(  21, parsed->port );
	ASSERT_EQ( 0x0, parsed->user );
	ASSERT_EQ( 0x0, parsed->pass );
	ASSERT_EQ( 0x0, parsed->query );
	ASSERT_EQ( 0x0, parsed->fragment );

	free( parsed );

	return GREATEST_TEST_RES_PASS;
}

TEST default_scheme_parse()
{
	parsed_url* parsed;

	parsed = parse_url( "testurl.com", 0x0, 0 );
	if( parsed == 0x0 )
		FAILm( "failed to parse url" );

	ASSERT_STR_EQ( "testurl.com", parsed->host );
	ASSERT_STR_EQ(           "/", parsed->path );

	ASSERT_EQ( 0x0, parsed->scheme );
	ASSERT_EQ(   0, parsed->port );
	ASSERT_EQ( 0x0, parsed->user );
	ASSERT_EQ( 0x0, parsed->pass );
	ASSERT_EQ( 0x0, parsed->query );
	ASSERT_EQ( 0x0, parsed->fragment );

	free( parsed );

	return GREATEST_TEST_RES_PASS;
}

TEST default_scheme_with_user_parse()
{
	parsed_url* parsed;

	parsed = parse_url( "hej:hopp@testurl.com", 0x0, 0 );
	if( parsed == 0x0 )
		FAILm( "failed to parse url" );

	ASSERT_EQ( 0x0, parsed->scheme );
	ASSERT_EQ(   0, parsed->port );
	ASSERT_EQ( 0x0, parsed->query );
	ASSERT_EQ( 0x0, parsed->fragment );
	ASSERT_STR_EQ( "testurl.com", parsed->host );
	ASSERT_STR_EQ(           "/", parsed->path );
	ASSERT_STR_EQ(         "hej", parsed->user );
	ASSERT_STR_EQ(        "hopp", parsed->pass );

	free( parsed );

	return GREATEST_TEST_RES_PASS;
}

TEST simple_query()
{
	parsed_url* parsed;

	parsed = parse_url( "http://testurl.com/whoppa?apa=kossa", 0x0, 0 );
	if( parsed == 0x0 )
		FAILm( "failed to parse url" );

	ASSERT_STR_EQ(        "http", parsed->scheme );
	ASSERT_STR_EQ( "testurl.com", parsed->host );
	ASSERT_STR_EQ(     "/whoppa", parsed->path );
	ASSERT_STR_EQ(   "apa=kossa", parsed->query );

	ASSERT_EQ( 0x0, parsed->user );
	ASSERT_EQ( 0x0, parsed->pass );
	ASSERT_EQ( 0x0, parsed->fragment );

	free( parsed );
	return GREATEST_TEST_RES_PASS;
}

TEST simple_fragment()
{
	parsed_url* parsed;

	parsed = parse_url( "http://testurl.com/whoppa#le_fragment", 0x0, 0 );
	if( parsed == 0x0 )
		FAILm( "failed to parse url" );

	ASSERT_STR_EQ(        "http", parsed->scheme );
	ASSERT_STR_EQ( "testurl.com", parsed->host );
	ASSERT_STR_EQ(     "/whoppa", parsed->path );
	ASSERT_STR_EQ( "le_fragment", parsed->fragment );

	ASSERT_EQ( 0x0, parsed->user );
	ASSERT_EQ( 0x0, parsed->pass );
	ASSERT_EQ( 0x0, parsed->query );


	free( parsed );
	return GREATEST_TEST_RES_PASS;
}

TEST query_and_fragment()
{
	parsed_url* parsed;

	parsed = parse_url( "http://testurl.com/whoppa?apa=kossa#le_query", 0x0, 0 );
	if( parsed == 0x0 )
		FAILm( "failed to parse url" );

	ASSERT_STR_EQ(        "http", parsed->scheme );
	ASSERT_STR_EQ( "testurl.com", parsed->host );
	ASSERT_STR_EQ(     "/whoppa", parsed->path );
	ASSERT_STR_EQ(   "apa=kossa", parsed->query );
	ASSERT_STR_EQ(    "le_query", parsed->fragment );

	ASSERT_EQ( 0x0, parsed->user );
	ASSERT_EQ( 0x0, parsed->pass );

	free( parsed );
	return GREATEST_TEST_RES_PASS;
}

TEST mem_alloc_fail()
{
	// ... test that all buffer-sizes less than needed will not fail the parsing ...

	char buffer[2048];
	for(size_t i = 0; i < sizeof(buffer); ++i)
	{
		parsed_url* parsed = parse_url("http://user:pass@test.com/whoppa?apa=kossa#fragment", buffer, i);
		if(parsed)
		{
			ASSERT_STR_EQ(               "http", parsed->scheme );
			ASSERT_STR_EQ(           "test.com", parsed->host );
			ASSERT_STR_EQ(            "/whoppa", parsed->path );
			ASSERT_STR_EQ(               "user", parsed->user );
			ASSERT_STR_EQ(               "pass", parsed->pass );
			ASSERT_STR_EQ(          "apa=kossa", parsed->query );
			ASSERT_STR_EQ(           "fragment", parsed->fragment );

			ASSERT_EQ( 80, parsed->port );
			return GREATEST_TEST_RES_PASS;
		}
	}
	return GREATEST_TEST_RES_FAIL;
}

TEST ipv6()
{
	char buffer[2048];

	{
		parsed_url* parsed = parse_url( "http://[2001:0db8:85a3:0000:0000:8a2e:0370:7334]/whoppa?apa=kossa#le_query", buffer, sizeof(buffer) );
		if( parsed == 0x0 )
			FAILm( "failed to parse url" );

		ASSERT_STR_EQ(                                    "http", parsed->scheme );
		ASSERT_STR_EQ( "2001:0db8:85a3:0000:0000:8a2e:0370:7334", parsed->host );
		ASSERT_STR_EQ(                                 "/whoppa", parsed->path );
		ASSERT_STR_EQ(                               "apa=kossa", parsed->query );
		ASSERT_STR_EQ(                                "le_query", parsed->fragment );

		ASSERT_EQ( 0x0, parsed->user );
		ASSERT_EQ( 0x0, parsed->pass );
		ASSERT_EQ(  80, parsed->port );
	}

	{
		// with user and pass

		parsed_url* parsed = parse_url( "http://user:pass@[2001:0db8:85a3:0000:0000:8a2e:0370:7334]/whoppa?apa=kossa#le_query", buffer, sizeof(buffer) );
		if( parsed == 0x0 )
			FAILm( "failed to parse url" );

		ASSERT_STR_EQ(                                    "http", parsed->scheme );
		ASSERT_STR_EQ( "2001:0db8:85a3:0000:0000:8a2e:0370:7334", parsed->host );
		ASSERT_STR_EQ(                                 "/whoppa", parsed->path );
		ASSERT_STR_EQ(                                    "user", parsed->user );
		ASSERT_STR_EQ(                                    "pass", parsed->pass );
		ASSERT_STR_EQ(                               "apa=kossa", parsed->query );
		ASSERT_STR_EQ(                                "le_query", parsed->fragment );

		ASSERT_EQ(  80, parsed->port );
	}

	{
		// with port

		parsed_url* parsed = parse_url( "http://[2001:0db8:85a3:0000:0000:8a2e:0370:7334]:1337/whoppa?apa=kossa#le_query", buffer, sizeof(buffer) );
		if( parsed == 0x0 )
			FAILm( "failed to parse url" );

		ASSERT_STR_EQ(                                    "http", parsed->scheme );
		ASSERT_STR_EQ( "2001:0db8:85a3:0000:0000:8a2e:0370:7334", parsed->host );
		ASSERT_STR_EQ(                                 "/whoppa", parsed->path );
		ASSERT_STR_EQ(                               "apa=kossa", parsed->query );
		ASSERT_STR_EQ(                                "le_query", parsed->fragment );

		ASSERT_EQ(  0x0, parsed->user );
		ASSERT_EQ(  0x0, parsed->pass );
		ASSERT_EQ( 1337, parsed->port );
	}

	{
		// local host

		parsed_url* parsed = parse_url( "http://[::1]/whoppa?apa=kossa#le_query", buffer, sizeof(buffer) );
		if( parsed == 0x0 )
			FAILm( "failed to parse url" );

		ASSERT_STR_EQ( "http",      parsed->scheme );
		ASSERT_STR_EQ( "::1",       parsed->host );
		ASSERT_STR_EQ( "/whoppa",   parsed->path );
		ASSERT_STR_EQ( "apa=kossa", parsed->query );
		ASSERT_STR_EQ( "le_query",  parsed->fragment );

		ASSERT_EQ(  0x0, parsed->user );
		ASSERT_EQ(  0x0, parsed->pass );
		ASSERT_EQ(  80, parsed->port );
	}

	return GREATEST_TEST_RES_PASS;
}

TEST ipv6_invalid()
{
	// verify fail on invalid ipv6 address...
	char buffer[2048];

	{
		// no ending ']'
		parsed_url* parsed = parse_url( "http://[::1", buffer, sizeof(buffer) );
		ASSERT_EQ( 0x0, parsed );
	}

	{
		// contain non hex char
		parsed_url* parsed = parse_url( "http://[2001:0db8:85z3::8a2e:0370:7334]", buffer, sizeof(buffer) );
		ASSERT_EQ( 0x0, parsed );
	}

	{
		// contain non a-f,A-F:.
		parsed_url* parsed = parse_url( "http://[2001:0db8:85!3::8a2e:0370:7334]", buffer, sizeof(buffer) );
		ASSERT_EQ( 0x0, parsed );
	}

	return GREATEST_TEST_RES_PASS;
}

GREATEST_SUITE( url_parse )
{
	RUN_TEST( full_url_parse );
	RUN_TEST( full_url_parse_win_style_path );
	RUN_TEST( url_no_host );
	RUN_TEST( url_win_style_abs_path );
	RUN_TEST( url_win_style_abs_path_with_host );
	RUN_TEST( url_win_style_abs_path_with_host_and_port );
	RUN_TEST( url_scheme_is_lower_cased );
	RUN_TEST( url_no_scheme_with_port );
	RUN_TEST( url_casing );
	RUN_TEST( default_port_parse );
	RUN_TEST( default_scheme_parse );
	RUN_TEST( default_scheme_with_user_parse );
	RUN_TEST( simple_query );
	RUN_TEST( simple_fragment );
	RUN_TEST( query_and_fragment );
	RUN_TEST( mem_alloc_fail );
	RUN_TEST( ipv6 );
	RUN_TEST( ipv6_invalid );
}

GREATEST_MAIN_DEFS();

int main( int argc, char **argv )
{
    GREATEST_MAIN_BEGIN();
    RUN_SUITE( url_parse );
    GREATEST_MAIN_END();
}

