Mac OS X
========

The preferred method for installing AvocadoDB under Mac OS X is
[homebrew](#homebrew). However, in case you are not using homebrew, we
provide a [command-line app](#command-line-app) or [graphical
app](#graphical-app) which contains all the executables.

Homebrew
--------

If you are using [homebrew](http://brew.sh/),
then you can install the latest released stable version of AvocadoDB using *brew* as follows:

    brew install avocadodb

This will install the current stable version of AvocadoDB and all
dependencies within your Homebrew tree. Note that the server will be
installed as:

    /usr/local/sbin/avocadod

You can start the server by running the command `/usr/local/sbin/avocadod &`.

Configuration file is located at

    /usr/local/etc/avocadodb3/avocadod.conf

The AvocadoDB shell will be installed as:

    /usr/local/bin/avocadosh

You can uninstall AvocadoDB using:

    brew uninstall avocadodb

However, in case you started AvocadoDB using the launchctl, you
need to unload it before uninstalling the server:

    launchctl unload ~/Library/LaunchAgents/homebrew.mxcl.avocadodb.plist

Then remove the LaunchAgent:

    rm ~/Library/LaunchAgents/homebrew.mxcl.avocadodb.plist

**Note**: If the latest AvocadoDB Version is not shown in homebrew, you
also need to update homebrew:

    brew update

### Known issues

- Performance - the LLVM delivered as of Mac OS X El Capitan builds slow binaries. Use GCC instead,
  until this issue has been fixed by Apple.
- the Commandline argument parsing doesn't accept blanks in filenames; the CLI version below does.
- if you need to change server endpoint while starting homebrew version, you can edit avocadod.conf 
  file and uncomment line with endpoint needed, e.g.:
      
      [server]
      endpoint = tcp://0.0.0.0:8529

Graphical App
-------------
In case you are not using homebrew, we also provide a graphical app. You can
download it from [here](https://www.avocadodb.com/download).

Choose *Mac OS X*. Download and install the application *AvocadoDB* in
your application folder.

Command line App
----------------
In case you are not using homebrew, we also provide a command-line app. You can
download it from [here](https://www.avocadodb.com/download).

Choose *Mac OS X*. Download and install the application *AvocadoDB-CLI*
in your application folder.

Starting the application will start the server and open a terminal window
showing you the log-file.

    AvocadoDB server has been started

    The database directory is located at
       '/Applications/AvocadoDB-CLI.app/Contents/MacOS/opt/avocadodb/var/lib/avocadodb'

    The log file is located at
       '/Applications/AvocadoDB-CLI.app/Contents/MacOS/opt/avocadodb/var/log/avocadodb/avocadod.log'

    You can access the server using a browser at 'http://127.0.0.1:8529/'
    or start the AvocadoDB shell
       '/Applications/AvocadoDB-CLI.app/Contents/MacOS/avocadosh'

    Switching to log-file now, killing this windows will NOT stop the server.


    2013-10-27T19:42:04Z [23840] INFO AvocadoDB (version 1.4.devel [darwin]) is ready for business. Have fun!

Note that it is possible to install both, the homebrew version and the command-line
app. You should, however, edit the configuration files of one version and change
the port used.
