add_deps("tinyparam", "jsonrpc", "cjson")
target("server")
    set_kind("binary")
    add_files("server.c")

target("client1")
    set_kind("binary")
    add_files("client1.c")

target("client2")
    set_kind("binary")
    add_files("client2.c")

