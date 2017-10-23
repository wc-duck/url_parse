[![Build Status](https://travis-ci.org/wc-duck/url_parse.svg?branch=master)](https://travis-ci.org/wc-duck/url_parse)
[![Build status](https://ci.appveyor.com/api/projects/status/hgrs9idixsu3hgrc?svg=true)](https://ci.appveyor.com/project/wc-duck/url-parse)

# url.h
Simple, STB-style, parser for URL:s as specified by RFC1738 ( http://www.ietf.org/rfc/rfc1738.txt )

compile with URL_PARSER_IMPLEMENTATION defined for implementation.
compile with URL_PARSER_IMPLEMENTATION_STATIC defined for static implementation.

# example

```c++

#define URL_PARSER_IMPLEMENTATION
#include "url.h"

#include <stdio.h>

int main(int argc, char** argv)
{
    if(argc == 0)
    {
        printf("no url provided!\n");
        return 0;
    }

    // See url.h to not use malloc() for allocation.
    parsed_url* parsed = parse_url( argv[1], 0x0, 0 );
    if( parsed == 0x0 )
        printf("failed to parse URL: %s\n", argv[1]);

    printf( "scheme: %s\n", parsed->scheme );
    printf( "host:   %s\n", parsed->host );
    printf( "port:   %d\n", parsed->port );
    printf( "path:   %s\n", parsed->path );
    printf( "user:   %s\n", parsed->user );
    printf( "pass:   %s\n", parsed->pass );

    free( parsed );
    return 0;
}

```

Contributions are happily accepted!
