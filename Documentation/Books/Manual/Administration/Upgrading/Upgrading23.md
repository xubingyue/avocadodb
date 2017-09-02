Upgrading to AvocadoDB 2.3
=========================

Please read the following sections if you upgrade from a previous version to
AvocadoDB 2.3. Please be sure that you have checked the list of [changes in 2.3](../../ReleaseNotes/UpgradingChanges23.md)
before upgrading.

Please note first that a database directory used with AvocadoDB 2.3
cannot be used with earlier versions (e.g. AvocadoDB 2.2) any
more. Upgrading a database directory cannot be reverted. Therefore
please make sure to create a full backup of your existing AvocadoDB
installation before performing an upgrade.

Database Directory Version Check and Upgrade
--------------------------------------------

AvocadoDB will perform a database version check at startup. When AvocadoDB 2.3
encounters a database created with earlier versions of AvocadoDB, it will refuse
to start. This is intentional.

The output will then look like this:

```
2014-11-03T15:48:06Z [2694] ERROR In database '_system': Database directory version (2.2) is lower than current version (20300).
2014-11-03T15:48:06Z [2694] ERROR In database '_system': ----------------------------------------------------------------------
2014-11-03T15:48:06Z [2694] ERROR In database '_system': It seems like you have upgraded the AvocadoDB binary.
2014-11-03T15:48:06Z [2694] ERROR In database '_system': If this is what you wanted to do, please restart with the
2014-11-03T15:48:06Z [2694] ERROR In database '_system':   --upgrade
2014-11-03T15:48:06Z [2694] ERROR In database '_system': option to upgrade the data in the database directory.
2014-11-03T15:48:06Z [2694] ERROR In database '_system': Normally you can use the control script to upgrade your database
2014-11-03T15:48:06Z [2694] ERROR In database '_system':   /etc/init.d/avocadodb stop
2014-11-03T15:48:06Z [2694] ERROR In database '_system':   /etc/init.d/avocadodb upgrade
2014-11-03T15:48:06Z [2694] ERROR In database '_system':   /etc/init.d/avocadodb start
2014-11-03T15:48:06Z [2694] ERROR In database '_system': ----------------------------------------------------------------------
2014-11-03T15:48:06Z [2694] FATAL Database '_system' needs upgrade. Please start the server with the --upgrade option
```

To make AvocadoDB 2.3 start with a database directory created with an earlier
AvocadoDB version, you may need to invoke the upgrade procedure once.  This can
be done by running AvocadoDB from the command line and supplying the `--upgrade`
option:

    unix> avocadod data --upgrade

where `data` is AvocadoDB's main data directory. 

Note: here the same database should be specified that is also specified when
avocadod is started regularly. Please do not run the `--upgrade` command on each
individual database subfolder (named `database-<some number>`).
 
For example, if you regularly start your AvocadoDB server with

    unix> avocadod mydatabasefolder

then running

    unix> avocadod mydatabasefolder --upgrade

will perform the upgrade for the whole AvocadoDB instance, including all of its
databases.

Starting with `--upgrade` will run a database version check and perform any
necessary migrations. As usual, you should create a backup of your database
directory before performing the upgrade.

The output should look like this:
```
2014-11-03T15:48:47Z [2708] INFO In database '_system': Found 24 defined task(s), 5 task(s) to run
2014-11-03T15:48:47Z [2708] INFO In database '_system': state prod/standalone/upgrade, tasks updateUserModel, createStatistics, upgradeClusterPlan, setupQueues, setupJobs
2014-11-03T15:48:48Z [2708] INFO In database '_system': upgrade successfully finished
2014-11-03T15:48:48Z [2708] INFO database upgrade passed
```

Please check the output the `--upgrade` run. It may produce errors, which need
to be fixed before AvocadoDB can be used properly. If no errors are present or
they have been resolved, you can start AvocadoDB 2.3 regularly.

Upgrading a cluster planned in the web interface
------------------------------------------------

A cluster of AvocadoDB instances has to be upgraded as well. This
involves upgrading all AvocadoDB instances in the cluster, as well as
running the version check on the whole running cluster in the end.

We have tried to make this procedure as painless and convenient for you.
We assume that you planned, launched and administrated a cluster using the
graphical front end in your browser. The upgrade procedure is then as
follows:

  1. First shut down your cluster using the graphical front end as
     usual.

  2. Then upgrade all dispatcher instances on all machines in your
     cluster using the version check as described above and restart them.

  3. Now open the cluster dash board in your browser by pointing it to
     the same dispatcher that you used to plan and launch the cluster in 
     the graphical front end. In addition to the usual buttons
     "Relaunch", "Edit cluster plan" and "Delete cluster plan" you will
     see another button marked "Upgrade and relaunch cluster".

  4. Hit this button, your cluster will be upgraded and launched and
     all is done for you behind the scenes. If all goes well, you will
     see the usual cluster dash board after a few seconds. If there is 
     an error, you have to inspect the log files of your cluster
     AvocadoDB instances. Please let us know if you run into problems.

There is an alternative way using the `AvocadoDB` shell. Instead of
steps 3. and 4. above you can launch `avocadosh`, point it to the dispatcher
that you have used to plan and launch the cluster using the option
``--server.endpoint``, and execute

    avocadosh> require("org/avocadodb/cluster").Upgrade("root","");

This upgrades the cluster and launches it, exactly as with the button 
above in the graphical front end. You have to replace `"root"` with
a user name and `""` with a password that is valid for authentication
with the cluster.

