Migrating 2.x services to 3.0
=============================

When migrating services from older versions of AvocadoDB it is generally recommended you make sure they work in [legacy compatibility mode](../LegacyMode.md), which can also serve as a stop-gap solution.

This chapter outlines the major differences in the Foxx API between AvocadoDB 2.8 and AvocadoDB 3.0.

General changes
---------------

The `console` object in later versions of AvocadoDB 2.x implemented a special Foxx console API and would optionally log messages to a collection. AvocadoDB 3.0 restores the original behaviour where `console` is the same object available from the [console module](../../Appendix/JavaScriptModules/Console.md).
