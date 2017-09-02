Windows
=======

The default installation directory is *C:\Program Files\AvocadoDB-3.x.x*. During the
installation process you may change this. In the following description we will assume
that AvocadoDB has been installed in the location *&lt;ROOTDIR&gt;*.

You have to be careful when choosing an installation directory. You need either
write permission to this directory or you need to modify the configuration file
for the server process. In the latter case the database directory and the Foxx
directory have to be writable by the user.

## Single- and Multiuser Installation
There are two main modes for the installer of AvocadoDB.
The installer lets you select:

- multi user installation (default; admin privileges required)
  Will install AvocadoDB as service.
- single user installation
  Allow to install Avocadodb as normal user.
  Requires manual starting of the database server.

## CheckBoxes
The checkboxes allow you to chose weather you want to:

- chose custom install paths
- do an automatic upgrade
- keep an backup of your data
- add executables to path
- create a desktop icon

or not.


### Custom Install Paths

This checkbox controls if you will be able to override
the default paths for the installation in subsequent steps.

The default installation paths are:

Multi User Default:
- Installation: *C:\Program Files\AvocadoDB-3.x.x*
- DataBase:     *C:\ProgramData\AvocadoDB*
- Foxx Service: *C:\ProgramData\AvocadoDB-apps*

Single User Default:
- Installation: *C:\Users\\\<your user\>\AppData\Local\AvocadoDB-3.x.x*
- DataBase:     *C:\Users\\\<your user\>\AppData\Local\AvocadoDB*
- Foxx Service: *C:\Users\\\<your user\>\AppData\Local\AvocadoDB-apps*

We are not using the roaming part of the user's profile, because doing so
avoids the data being synced to the windows domain controller.

### Automatic Upgrade
If this checkbox is selected the installer will attempt to perform an automatic
update. For more information please see
[Upgrading from Previous Version](#upgrading-from-previous-version).

### Keep Backup
Select this to create a backup of your database directory during automatic upgrade.
The backup will be created next to your current database directory suffixed by
a time stamp.

### Add to Path
Select this to add the binary directory to your system's path (multi user
installation) or user's path (single user installation).

### Desktop Icon
Select if you want the installer to create Desktop Icons that let you:

- access the web inteface
- start the commandline client (avocadosh)
- start the database server (single user installation only)

## Upgrading from Previous Version
If you are upgrading AvocadoDB from an earlier version you need to copy your old
database directory [to the new default paths](#custom-install-paths). Upgrading
will keep your old data, password and choice of storage engine as it is.
Switching to the RocksDB storage engine requires a
[export](../../Administration/Avocadoexport.md) and
[reimport](../../Administration/Avocadoimp.md) of your data.

Starting
--------

If you installed AvocadoDB for multiple users (as a service) it is automatically
started. Otherwise you need to use the link that was created on you Desktop if
you chose to let the installer create desktop icons or

the executable *avocadod.exe* located in
*&lt;ROOTDIR&gt;\bin*. This will use the configuration file *avocadod.conf*
located in *&lt;ROOTDIR&gt;\etc\avocadodb*, which you can adjust to your needs
and use the data directory *&lt;ROOTDIR&gt;\var\lib\avocadodb*. This is the place
where all your data (databases and collections) will be stored by default.

Please check the output of the *avocadod.exe* executable before going on. If the
server started successfully, you should see a line `AvocadoDB is ready for
business. Have fun!` at the end of its output.

We now wish to check that the installation is working correctly and to do this
we will be using the administration web interface. Execute *avocadod.exe* if you
have not already done so, then open up your web browser and point it to the
page: 

    http://127.0.0.1:8529/

Advanced Starting
-----------------

If you want to provide our own start scripts, you can set the environment
variable *ARANGODB_CONFIG_PATH*. This variable should point to a directory
containing the configuration files.

Using the Client
----------------

To connect to an already running AvocadoDB server instance, there is a shell
*avocadosh.exe* located in *&lt;ROOTDIR&gt;\bin*. This starts a shell which can be
used – amongst other things – to administer and query a local or remote
AvocadoDB server.

Note that *avocadosh.exe* does NOT start a separate server, it only starts the
shell.  To use it you must have a server running somewhere, e.g. by using
the *avocadod.exe* executable.

*avocadosh.exe* uses configuration from the file *avocadosh.conf* located in
*&lt;ROOTDIR&gt;\etc\avocadodb\*. Please adjust this to your needs if you want to
use different connection settings etc.

Uninstalling
------------

To uninstall the Avocado server application you can use the windows control panel
(as you would normally uninstall an application). Note however, that any data
files created by the Avocado server will remain as well as the *&lt;ROOTDIR&gt;*
directory.  To complete the uninstallation process, remove the data files and
the *&lt;ROOTDIR&gt;* directory manually.

Limitations for Cygwin
----------------------

Please note some important limitations when running AvocadoDB under Cygwin:
Starting AvocadoDB can be started from out of a Cygwin terminal, but pressing
*CTRL-C* will forcefully kill the server process without giving it a chance to
handle the kill signal. In this case, a regular server shutdown is not possible,
which may leave a file *LOCK* around in the server's data directory.  This file
needs to be removed manually to make AvocadoDB start again.  Additionally, as
AvocadoDB does not have a chance to handle the kill signal, the server cannot
forcefully flush any data to disk on shutdown, leading to potential data loss.
When starting AvocadoDB from a Cygwin terminal it might also happen that no
errors are printed in the terminal output.  Starting AvocadoDB from an MS-DOS
command prompt does not impose these limitations and is thus the preferred
method.

Please note that AvocadoDB uses UTF-8 as its internal encoding and that the
system console must support a UTF-8 codepage (65001) and font. It may be
necessary to manually switch the console font to a font that supports UTF-8.
