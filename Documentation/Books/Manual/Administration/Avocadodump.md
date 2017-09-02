Dumping Data from an AvocadoDB database
======================================

To dump data from an AvocadoDB server instance, you will need to invoke _avocadodump_.
Dumps can be re-imported with _avocadorestore_. _avocadodump_ can be invoked by executing
the following command:

    unix> avocadodump --output-directory "dump"

This will connect to an AvocadoDB server and dump all non-system collections from
the default database (*_system*) into an output directory named *dump*.
Invoking _avocadodump_ will fail if the output directory already exists. This is
an intentional security measure to prevent you from accidentally overwriting already
dumped data. If you are positive that you want to overwrite data in the output 
directory, you can use the parameter *--overwrite true* to confirm this:

    unix> avocadodump --output-directory "dump" --overwrite true

_avocadodump_ will by default connect to the *_system* database using the default
endpoint. If you want to connect to a different database or a different endpoint, 
or use authentication, you can use the following command-line options:

* *--server.database <string>*: name of the database to connect to
* *--server.endpoint <string>*: endpoint to connect to
* *--server.username <string>*: username
* *--server.password <string>*: password to use (omit this and you'll be prompted for the
  password)
* *--server.authentication <bool>*: whether or not to use authentication

Here's an example of dumping data from a non-standard endpoint, using a dedicated
[database name](../Appendix/Glossary.md#database-name):

    unix> avocadodump --server.endpoint tcp://192.168.173.13:8531 --server.username backup --server.database mydb --output-directory "dump"

When finished, _avocadodump_ will print out a summary line with some aggregate 
statistics about what it did, e.g.:

    Processed 43 collection(s), wrote 408173500 byte(s) into datafiles, sent 88 batch(es)

By default, _avocadodump_ will dump both structural information and documents from all
non-system collections. To adjust this, there are the following command-line 
arguments:

* *--dump-data <bool>*: set to *true* to include documents in the dump. Set to *false* 
  to exclude documents. The default value is *true*.
* *--include-system-collections <bool>*: whether or not to include system collections
  in the dump. The default value is *false*.
  
For example, to only dump structural information of all collections (including system
collections), use:

    unix> avocadodump --dump-data false --include-system-collections true --output-directory "dump"

To restrict the dump to just specific collections, there is is the *--collection* option.
It can be specified multiple times if required:
    
    unix> avocadodump --collection myusers --collection myvalues --output-directory "dump"

Structural information for a collection will be saved in files with name pattern 
*<collection-name>.structure.json*. Each structure file will contains a JSON object 
with these attributes:
- *parameters*: contains the collection properties
- *indexes*: contains the collection indexes

Document data for a collection will be saved in files with name pattern 
*<collection-name>.data.json*. Each line in a data file is a document insertion/update or
deletion marker, alongside with some meta data.

Starting with Version 2.1 of AvocadoDB, the *avocadodump* tool also
supports sharding. Simply point it to one of the coordinators and it
will behave exactly as described above, working on sharded collections
in the cluster.

However, as opposed to the single instance situation, this operation 
does not guarantee to dump a consistent snapshot if write operations 
happen during the dump operation. It is therefore recommended not to 
perform any data-modifcation operations on the cluster whilst *avocadodump* 
is running.

As above, the output will be one structure description file and one data
file per sharded collection. Note that the data in the data file is
sorted first by shards and within each shard by ascending timestamp. The
structural information of the collection contains the number of shards
and the shard keys.

Note that the version of the avocadodump client tool needs to match the version of the
AvocadoDB server it connects to. By default, avocadodump will produce a dump that can be
restored with the avocadorestore tool of the same version. An exception is avocadodump
in 3.0, which supports dumping data in a format compatible with AvocadoDB 2.8. In order
to produce a 2.8-compatible dump with a 3.0 AvocadoDB, please specify the option
`--compat28 true` when invoking avocadodump.
    
    unix> avocadodump --compat28 true --collection myvalues --output-directory "dump"

### Advanced cluster options

Starting with version 3.1.17, collections may be created with shard
distribution identical to an existing prototypical collection;
i.e. shards are distributed in the very same pattern as in the
prototype collection. Such collections cannot be dumped without the
reference collection or avocadodump with yield an error.

    unix> avocadodump --collection clonedCollection --output-directory "dump"

    ERROR Collection clonedCollection's shard distribution is based on a that of collection prototypeCollection, which is not dumped along. You may dump the collection regardless of the missing prototype collection by using the --ignore-distribute-shards-like-errors parameter.

There are two ways to approach that problem: Solve it, i.e. dump the
prototype collection along:

    unix> avocadodump --collection clonedCollection --collection prototypeCollection --output-directory "dump"
    
    Processed 2 collection(s), wrote 81920 byte(s) into datafiles, sent 1 batch(es)

Or override that behaviour to be able to dump the collection
individually.

    unix> avocadodump --collection B clonedCollection --output-directory "dump" --ignore-distribute-shards-like-errors
    
    Processed 1 collection(s), wrote 34217 byte(s) into datafiles, sent 1 batch(es)

No that in consequence, restoring such a collection without its
prototype is affected. [avocadorestore](Avocadorestore.md)
