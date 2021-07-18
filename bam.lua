--[[
 Simple, STB-style, parser for URL:s as specified by RFC1738 ( http://www.ietf.org/rfc/rfc1738.txt )

 compile with URL_PARSER_IMPLEMENTATION defined for implementation.
 compile with URL_PARSER_IMPLEMENTATION_STATIC defined for static implementation.

 version 1.0, June, 2014

 Copyright (C) 2014- Fredrik Kihlander

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
--]]

BUILD_PATH = "local"

config   = "debug"

local settings = NewSettings() -- {}

settings.cc.includes:Add("include")
if family == 'windows' then
	platform = "winx64"
else
	platform = "linux_x86_64"
	settings.cc.flags:Add( "-Wconversion", "-Wextra", "-Wall", "-Werror", "-Wstrict-aliasing=2" )
end

local output_path = PathJoin( BUILD_PATH, PathJoin( platform, config ) )
local output_func = function(settings, path) return PathJoin(output_path, PathFilename(PathBase(path)) .. settings.config_ext) end
settings.cc.Output = output_func
settings.lib.Output = output_func
settings.link.Output = output_func

local tests  = Link( settings, 'url_tests', Compile( settings, 'test/url_parse_tests.cpp' ) )

test_args = " -v"
if ScriptArgs["test"]     then test_args = test_args .. " -t " .. ScriptArgs["test"] end
if ScriptArgs["suite"]    then test_args = test_args .. " -s " .. ScriptArgs["suite"] end

if family == "windows" then
        AddJob( "test",  "unittest",  string.gsub( tests, "/", "\\" ) .. test_args, tests, tests )
else
        AddJob( "test",     "unittest",  tests .. test_args, tests, tests )
        AddJob( "valgrind", "valgrind",  "valgrind -v --leak-check=full --track-origins=yes " .. tests .. test_args, tests, tests )
end

PseudoTarget( "all", tests, listdir )
DefaultTarget( "all" )
