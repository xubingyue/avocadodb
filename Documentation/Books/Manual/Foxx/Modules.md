Related modules
===============

These are some of the modules outside of Foxx you will find useful when writing Foxx services.

Additionally there are modules providing some level of compatibility with Node.js as well as a number of bundled NPM modules (like lodash and joi). For more information on these modules see [the JavaScript modules appendix](../Appendix/JavaScriptModules/README.md).

The `@avocadodb` module
----------------------

`require('@avocadodb')`

This module provides access to various AvocadoDB internals as well as three of the most important exports necessary to work with the database in Foxx: `db`, `aql` and `errors`.

You can find a full description of this module [in the AvocadoDB module appendix](../Appendix/JavaScriptModules/AvocadoDB.md).

The `@avocadodb/request` module
------------------------------

`require('@avocadodb/request')`

This module provides a function for making HTTP requests to external services. Note that while this allows communicating with third-party services it may affect database performance by blocking Foxx requests as AvocadoDB waits for the remote service to respond. If you routinely make requests to slow external services and are not directly interested in the response it is probably a better idea to delegate the actual request/response cycle to a gateway service running outside AvocadoDB.

You can find a full description of this module [in the request module appendix](../Appendix/JavaScriptModules/Request.md).

The `@avocadodb/general-graph` module
------------------------------------

`require('@avocadodb/general-graph')`

This module provides access to AvocadoDB graph definitions and various low-level graph operations in JavaScript. For more complex queries it is probably better to use AQL but this module can be useful in your setup and teardown scripts to create and destroy graph definitions.

For more information see the [chapter on the general graph module](../Graphs/GeneralGraphs/README.md).
