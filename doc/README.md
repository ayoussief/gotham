Gotham Core
=============

Setup
---------------------
Gotham Core is the original Gotham client and it builds the backbone of the network. It downloads and, by default, stores the entire history of Gotham transactions, which requires several hundred gigabytes or more of disk space. Depending on the speed of your computer and network connection, the synchronization process can take anywhere from a few hours to several days or more.

To download Gotham Core, visit [gothamcore.org](https://gothamcore.org/en/download/).

Running
---------------------
The following are some helpful notes on how to run Gotham Core on your native platform.

### Unix

Unpack the files into a directory and run:

- `bin/gotham-qt` (GUI) or
- `bin/gothamd` (headless)
- `bin/gotham` (wrapper command)

The `gotham` command supports subcommands like `gotham gui`, `gotham node`, and `gotham rpc` exposing different functionality. Subcommands can be listed with `gotham help`.

### Windows

Unpack the files into a directory, and then run gotham-qt.exe.

### macOS

Drag Gotham Core to your applications folder, and then run Gotham Core.

### Need Help?

* See the documentation at the [Gotham Wiki](https://en.gotham.it/wiki/Main_Page)
for help and more information.
* Ask for help on [Gotham StackExchange](https://gotham.stackexchange.com).
* Ask for help on #gotham on Libera Chat. If you don't have an IRC client, you can use [web.libera.chat](https://web.libera.chat/#gotham).
* Ask for help on the [GothamTalk](https://gothamtalk.org/) forums, in the [Technical Support board](https://gothamtalk.org/index.php?board=4.0).

Building
---------------------
The following are developer notes on how to build Gotham Core on your native platform. They are not complete guides, but include notes on the necessary libraries, compile flags, etc.

- [Dependencies](dependencies.md)
- [macOS Build Notes](build-osx.md)
- [Unix Build Notes](build-unix.md)
- [Windows Build Notes](build-windows-msvc.md)
- [FreeBSD Build Notes](build-freebsd.md)
- [OpenBSD Build Notes](build-openbsd.md)
- [NetBSD Build Notes](build-netbsd.md)

Development
---------------------
The Gotham repo's [root README](/README.md) contains relevant information on the development process and automated testing.

- [Developer Notes](developer-notes.md)
- [Productivity Notes](productivity.md)
- [Release Process](release-process.md)
- [Source Code Documentation (External Link)](https://doxygen.gothamcore.org/)
- [Translation Process](translation_process.md)
- [Translation Strings Policy](translation_strings_policy.md)
- [JSON-RPC Interface](JSON-RPC-interface.md)
- [Unauthenticated REST Interface](REST-interface.md)
- [BIPS](bips.md)
- [Dnsseed Policy](dnsseed-policy.md)
- [Benchmarking](benchmarking.md)
- [Internal Design Docs](design/)

### Resources
* Discuss on the [GothamTalk](https://gothamtalk.org/) forums, in the [Development & Technical Discussion board](https://gothamtalk.org/index.php?board=6.0).
* Discuss project-specific development on #gotham-core-dev on Libera Chat. If you don't have an IRC client, you can use [web.libera.chat](https://web.libera.chat/#gotham-core-dev).

### Miscellaneous
- [Assets Attribution](assets-attribution.md)
- [gotham.conf Configuration File](gotham-conf.md)
- [CJDNS Support](cjdns.md)
- [Files](files.md)
- [Fuzz-testing](fuzzing.md)
- [I2P Support](i2p.md)
- [Init Scripts (systemd/upstart/openrc)](init.md)
- [Managing Wallets](managing-wallets.md)
- [Multisig Tutorial](multisig-tutorial.md)
- [Offline Signing Tutorial](offline-signing-tutorial.md)
- [P2P bad ports definition and list](p2p-bad-ports.md)
- [PSBT support](psbt.md)
- [Reduce Memory](reduce-memory.md)
- [Reduce Traffic](reduce-traffic.md)
- [Tor Support](tor.md)
- [Transaction Relay Policy](policy/README.md)
- [ZMQ](zmq.md)

License
---------------------
Distributed under the [MIT software license](/COPYING).
