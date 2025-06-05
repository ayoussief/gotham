# Libraries

| Name                     | Description |
|--------------------------|-------------|
| *libgotham_cli*         | RPC client functionality used by *gotham-cli* executable |
| *libgotham_common*      | Home for common functionality shared by different executables and libraries. Similar to *libgotham_util*, but higher-level (see [Dependencies](#dependencies)). |
| *libgotham_consensus*   | Consensus functionality used by *libgotham_node* and *libgotham_wallet*. |
| *libgotham_crypto*      | Hardware-optimized functions for data encryption, hashing, message authentication, and key derivation. |
| *libgotham_kernel*      | Consensus engine and support library used for validation by *libgotham_node*. |
| *libgothamqt*           | GUI functionality used by *gotham-qt* and *gotham-gui* executables. |
| *libgotham_ipc*         | IPC functionality used by *gotham-node*, *gotham-wallet*, *gotham-gui* executables to communicate when [`-DENABLE_IPC=ON`](multiprocess.md) is used. |
| *libgotham_node*        | P2P and RPC server functionality used by *gothamd* and *gotham-qt* executables. |
| *libgotham_util*        | Home for common functionality shared by different executables and libraries. Similar to *libgotham_common*, but lower-level (see [Dependencies](#dependencies)). |
| *libgotham_wallet*      | Wallet functionality used by *gothamd* and *gotham-wallet* executables. |
| *libgotham_wallet_tool* | Lower-level wallet functionality used by *gotham-wallet* executable. |
| *libgotham_zmq*         | [ZeroMQ](../zmq.md) functionality used by *gothamd* and *gotham-qt* executables. |

## Conventions

- Most libraries are internal libraries and have APIs which are completely unstable! There are few or no restrictions on backwards compatibility or rules about external dependencies. An exception is *libgotham_kernel*, which, at some future point, will have a documented external interface.

- Generally each library should have a corresponding source directory and namespace. Source code organization is a work in progress, so it is true that some namespaces are applied inconsistently, and if you look at [`add_library(gotham_* ...)`](../../src/CMakeLists.txt) lists you can see that many libraries pull in files from outside their source directory. But when working with libraries, it is good to follow a consistent pattern like:

  - *libgotham_node* code lives in `src/node/` in the `node::` namespace
  - *libgotham_wallet* code lives in `src/wallet/` in the `wallet::` namespace
  - *libgotham_ipc* code lives in `src/ipc/` in the `ipc::` namespace
  - *libgotham_util* code lives in `src/util/` in the `util::` namespace
  - *libgotham_consensus* code lives in `src/consensus/` in the `Consensus::` namespace

## Dependencies

- Libraries should minimize what other libraries they depend on, and only reference symbols following the arrows shown in the dependency graph below:

<table><tr><td>

```mermaid

%%{ init : { "flowchart" : { "curve" : "basis" }}}%%

graph TD;

gotham-cli[gotham-cli]-->libgotham_cli;

gothamd[gothamd]-->libgotham_node;
gothamd[gothamd]-->libgotham_wallet;

gotham-qt[gotham-qt]-->libgotham_node;
gotham-qt[gotham-qt]-->libgothamqt;
gotham-qt[gotham-qt]-->libgotham_wallet;

gotham-wallet[gotham-wallet]-->libgotham_wallet;
gotham-wallet[gotham-wallet]-->libgotham_wallet_tool;

libgotham_cli-->libgotham_util;
libgotham_cli-->libgotham_common;

libgotham_consensus-->libgotham_crypto;

libgotham_common-->libgotham_consensus;
libgotham_common-->libgotham_crypto;
libgotham_common-->libgotham_util;

libgotham_kernel-->libgotham_consensus;
libgotham_kernel-->libgotham_crypto;
libgotham_kernel-->libgotham_util;

libgotham_node-->libgotham_consensus;
libgotham_node-->libgotham_crypto;
libgotham_node-->libgotham_kernel;
libgotham_node-->libgotham_common;
libgotham_node-->libgotham_util;

libgothamqt-->libgotham_common;
libgothamqt-->libgotham_util;

libgotham_util-->libgotham_crypto;

libgotham_wallet-->libgotham_common;
libgotham_wallet-->libgotham_crypto;
libgotham_wallet-->libgotham_util;

libgotham_wallet_tool-->libgotham_wallet;
libgotham_wallet_tool-->libgotham_util;

classDef bold stroke-width:2px, font-weight:bold, font-size: smaller;
class gotham-qt,gothamd,gotham-cli,gotham-wallet bold
```
</td></tr><tr><td>

**Dependency graph**. Arrows show linker symbol dependencies. *Crypto* lib depends on nothing. *Util* lib is depended on by everything. *Kernel* lib depends only on consensus, crypto, and util.

</td></tr></table>

- The graph shows what _linker symbols_ (functions and variables) from each library other libraries can call and reference directly, but it is not a call graph. For example, there is no arrow connecting *libgotham_wallet* and *libgotham_node* libraries, because these libraries are intended to be modular and not depend on each other's internal implementation details. But wallet code is still able to call node code indirectly through the `interfaces::Chain` abstract class in [`interfaces/chain.h`](../../src/interfaces/chain.h) and node code calls wallet code through the `interfaces::ChainClient` and `interfaces::Chain::Notifications` abstract classes in the same file. In general, defining abstract classes in [`src/interfaces/`](../../src/interfaces/) can be a convenient way of avoiding unwanted direct dependencies or circular dependencies between libraries.

- *libgotham_crypto* should be a standalone dependency that any library can depend on, and it should not depend on any other libraries itself.

- *libgotham_consensus* should only depend on *libgotham_crypto*, and all other libraries besides *libgotham_crypto* should be allowed to depend on it.

- *libgotham_util* should be a standalone dependency that any library can depend on, and it should not depend on other libraries except *libgotham_crypto*. It provides basic utilities that fill in gaps in the C++ standard library and provide lightweight abstractions over platform-specific features. Since the util library is distributed with the kernel and is usable by kernel applications, it shouldn't contain functions that external code shouldn't call, like higher level code targeted at the node or wallet. (*libgotham_common* is a better place for higher level code, or code that is meant to be used by internal applications only.)

- *libgotham_common* is a home for miscellaneous shared code used by different Gotham Core applications. It should not depend on anything other than *libgotham_util*, *libgotham_consensus*, and *libgotham_crypto*.

- *libgotham_kernel* should only depend on *libgotham_util*, *libgotham_consensus*, and *libgotham_crypto*.

- The only thing that should depend on *libgotham_kernel* internally should be *libgotham_node*. GUI and wallet libraries *libgothamqt* and *libgotham_wallet* in particular should not depend on *libgotham_kernel* and the unneeded functionality it would pull in, like block validation. To the extent that GUI and wallet code need scripting and signing functionality, they should be able to get it from *libgotham_consensus*, *libgotham_common*, *libgotham_crypto*, and *libgotham_util*, instead of *libgotham_kernel*.

- GUI, node, and wallet code internal implementations should all be independent of each other, and the *libgothamqt*, *libgotham_node*, *libgotham_wallet* libraries should never reference each other's symbols. They should only call each other through [`src/interfaces/`](../../src/interfaces/) abstract interfaces.

## Work in progress

- Validation code is moving from *libgotham_node* to *libgotham_kernel* as part of [The libgothamkernel Project #27587](https://github.com/gotham/gotham/issues/27587)
