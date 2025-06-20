27.2 Release Notes
=====================

Gotham Core version 27.2 is now available from:

  <https://gothamcore.org/bin/gotham-core-27.2/>

This release includes various bug fixes and performance
improvements, as well as updated translations.

Please report bugs using the issue tracker at GitHub:

  <https://github.com/gotham/gotham/issues>

To receive security and update notifications, please subscribe to:

  <https://gothamcore.org/en/list/announcements/join/>

How to Upgrade
==============

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes in some cases), then run the
installer (on Windows) or just copy over `/Applications/Gotham-Qt` (on macOS)
or `gothamd`/`gotham-qt` (on Linux).

Upgrading directly from a version of Gotham Core that has reached its EOL is
possible, but it might take some time if the data directory needs to be migrated. Old
wallet versions of Gotham Core are generally supported.

Compatibility
==============

Gotham Core is supported and extensively tested on operating systems
using the Linux Kernel 3.17+, macOS 11.0+, and Windows 7 and newer. Gotham
Core should also work on most other Unix-like systems but is not as
frequently tested on them. It is not recommended to use Gotham Core on
unsupported systems.

Notable changes
===============

### P2P

- #30394 net: fix race condition in self-connect detection

### Init

- #30435 init: change shutdown order of load block thread and scheduler

### RPC

- #30357 Fix cases of calls to FillPSBT errantly returning complete=true

### PSBT

- #29855 psbt: Check non witness utxo outpoint early

### Test

- #30552 test: fix constructor of msg_tx

### Doc

- #30504 doc: use proper doxygen formatting for CTxMemPool::cs

### Build

- #30283 upnp: fix build with miniupnpc 2.2.8
- #30633 Fixes for GCC 15 compatibility

### CI

- #30193 ci: move ASan job to GitHub Actions from Cirrus CI
- #30299 ci: remove unused bcc variable from workflow

Credits
=======

Thanks to everyone who directly contributed to this release:

- Ava Chow
- Cory Fields
- Martin Zumsande
- Matt Whitlock
- Max Edwards
- Sebastian Falbesoner
- Vasil Dimov
- willcl-ark

As well as to everyone that helped with translations on
[Transifex](https://www.transifex.com/gotham/gotham/).
