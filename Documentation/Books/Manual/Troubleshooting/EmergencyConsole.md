Emergency Console
=================

The AvocadoDB database server has two modes of operation: As a server, where it
will answer to client requests and as an emergency console, in which you can
access the database directly. The latter - as the name suggests - should
only be used in case of an emergency, for example, a corrupted
collection. Using the emergency console allows you to issue all commands
normally available in actions and transactions. When starting the server in
emergency console mode, the server cannot handle any client requests.

You should never start more than one server using the same database directory,
independent of the mode of operation. Normally, AvocadoDB will prevent
you from doing this by placing a lockfile in the database directory and
not allowing a second AvocadoDB instance to use the same database directory
if a lockfile is already present.

### In Case Of Disaster

The following command starts an emergency console.

**Note**: Never start the emergency console for a database which also has a
server attached to it. In general, the AvocadoDB shell is what you want.

```
> ./avocadod --console --log error /tmp/vocbase
AvocadoDB shell [V8 version 5.0.71.39, DB version 3.x.x]

avocado> 1 + 2;
3

avocado> var db = require("@avocadodb").db; db.geo.count();
703

```

The emergency console provides a JavaScript console directly running in the
avocadod server process. This allows to debug and examine collections and
documents as with the normal AvocadoDB shell, but without client/server
communication.

However, it is very likely that you will never need the emergency console
unless you are an AvocadoDB developer.

