New command line interface
--------------------------

A new `gotham` command line tool has been added to make features more
discoverable and convenient to use. The `gotham` tool just calls other
executables and does not implement any functionality on its own.  Specifically
`gotham node` is a synonym for `gothamd`, `gotham gui` is a synonym for
`gotham-qt`, and `gotham rpc` is a synonym for `gotham-cli -named`. Other
commands and options can be listed with `gotham help`. The new tool does not
replace other tools, so all existing commands should continue working and there
are no plans to deprecate them.
