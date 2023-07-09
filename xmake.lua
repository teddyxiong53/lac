-- define project
set_project("lac")
set_xmakever("2.3.2")
set_version("0.0.1", {build = "%Y%m%d%H%M"})

-- set common flags
-- set_warnings("all", "error")
-- set_languages("c")

-- add build modes
add_rules("mode.release", "mode.debug")

add_links("c","m","rt", "pthread")
add_includedirs("include", "libev","jsonrpc", "tinyparam", "cjson", "tinyds", "tinythpool", "tinyutils",
    "mqtt_client", "tinyev"
)

-- includes sub-projects
includes(
"app",
"cjson",
"demo",
"elk",
"event_handler",
"libev",
"jsonrpc",

-- "libevent",
"lua",
-- "mongoose",
"mqtt_client",
"test",
-- "tinyds",
"tinyev",
"tinyparam",
"tinythpool",
"tinyutils"
)
