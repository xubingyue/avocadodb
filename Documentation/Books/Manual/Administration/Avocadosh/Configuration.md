AvocadoDB Shell Configuration
============================

_avocadosh_ will look for a user-defined startup script named *.avocadosh.rc* in the
user's home directory on startup. The home directory will likely be `/home/<username>/`
on Unix/Linux, and is determined on Windows by peeking into the environment variables
`%HOMEDRIVE%` and `%HOMEPATH%`. 

If the file *.avocadosh.rc* is present in the home directory, _avocadosh_ will execute
the contents of this file inside the global scope.

You can use this to define your own extra variables and functions that you need often.
For example, you could put the following into the *.avocadosh.rc* file in your home
directory:

```js
// "var" keyword avoided intentionally...
// otherwise "timed" would not survive the scope of this script
global.timed = function (cb) {
  console.time("callback");
  cb();
  console.timeEnd("callback");
};
```

This will make a function named *timed* available in _avocadosh_ in the global scope.

You can now start _avocadosh_ and invoke the function like this:

```js
timed(function () { 
  for (var i = 0; i < 1000; ++i) {
    db.test.save({ value: i }); 
  }
});
```

Please keep in mind that, if present, the *.avocadosh.rc* file needs to contain valid
JavaScript code. If you want any variables in the global scope to survive you need to
omit the *var* keyword for them. Otherwise the variables will only be visible inside
the script itself, but not outside.
