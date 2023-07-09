add_deps("tinyparam", "jsonrpc", "cjson")
target("bgservice")
    set_kind("binary")
    add_files("bgservice-src/*.c")

target("cliapp")
    set_kind("binary")
    add_files("cliapp-src/*.c")

target("uiapp")
    set_kind("binary")
    add_files("uiapp-src/*.c")

-- target("snapclient")
--     set_kind("binary")
--     add_files("snapclient-src/*.c")

-- target("snapserver")
--     set_kind("binary")
--     add_files("snapserver-src/*.c")
