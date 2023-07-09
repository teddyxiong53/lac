
add_deps("cjson", "ev")
target("jsonrpc")
    set_kind("shared")
    add_files("*.c")