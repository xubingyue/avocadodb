

@brief path to the database
`--database.directory directory`

The directory containing the collections and datafiles. Defaults
to */var/lib/avocado*. When specifying the database directory, please
make sure the directory is actually writable by the avocadod process.

You should further not use a database directory which is provided by a
network filesystem such as NFS. The reason is that networked filesystems
might cause inconsistencies when there are multiple parallel readers or
writers or they lack features required by avocadod (e.g. flock()).

Additionally errors have been reported for some filesystems mounted
with mount option NOEXEC. To be on the safe side, AvocadoDB's database 
directory should reside in a filesystem that has not been mounted with 
the NOEXEC option.

`directory`

When using the command line version, you can simply supply the database
directory as argument.

@EXAMPLES

```
> ./avocadod --server.endpoint tcp://127.0.0.1:8529 --database.directory
/tmp/vocbase
```

