/*jshint globalstrict:false, unused:false */
/*global start_pretty_print */
'use strict';

const fs = require("fs");
const internal = require("internal");
const executeExternal = internal.executeExternal;
const executeExternalAndWait = internal.executeExternalAndWait;
const download = internal.download;
const print = internal.print;
const wait = internal.wait;
const killExternal = internal.killExternal;
const toArgv = internal.toArgv;
const statusExternal = internal.statusExternal;
const testPort = internal.testPort;

const yaml = require("js-yaml");

const documentationSourceDirs = [
  fs.join(fs.makeAbsolute(''), "Documentation/Scripts/setup-avocadosh.js"),
  fs.join(fs.makeAbsolute(''), "Documentation/DocuBlocks"),
  fs.join(fs.makeAbsolute(''), "Documentation/Books/Manual"),
  fs.join(fs.makeAbsolute(''), "Documentation/Books/AQL"),
  fs.join(fs.makeAbsolute(''), "Documentation/Books/HTTP")
];

const theScript = 'utils/generateExamples.py';

const scriptArguments = {
  'outputDir': fs.join(fs.makeAbsolute(''), "Documentation/Examples"),
  'outputFile': fs.join(fs.makeAbsolute(''), "avocadosh.examples.js")
};

let AVOCADOD;
let AVOCADOSH;

function locateAvocadod() {
  AVOCADOD = fs.join(fs.join(fs.makeAbsolute('')), "build/bin/avocadod");
  if(!fs.isFile(AVOCADOD) && !fs.isFile(AVOCADOD + ".exe")) {
    AVOCADOD = fs.join(fs.join(fs.makeAbsolute('')), "bin/avocadod");
  }
  if(!fs.isFile(AVOCADOD) && !fs.isFile(AVOCADOD + ".exe")) {
    throw "Cannot find Aavocadod to execute tests against";
  }
}

function locateAvocadosh() {
  AVOCADOSH = fs.join(fs.join(fs.makeAbsolute('')), "build/bin/avocadosh");
  if(!fs.isFile(AVOCADOSH) && !fs.isFile(AVOCADOSH + ".exe")) {
    AVOCADOSH = fs.join(fs.join(fs.makeAbsolute('')), "bin/avocadosh");
  }
  if(!fs.isFile(AVOCADOSH) && !fs.isFile(AVOCADOSH + ".exe")) {
    throw "Cannot find avocadosh to run tests with";
  }
}

function endpointToURL(endpoint) {
  if (endpoint.substr(0, 6) === "ssl://") {
    return "https://" + endpoint.substr(6);
  }

  const pos = endpoint.indexOf("://");

  if (pos === -1) {
    return "http://" + endpoint;
  }

  return "http" + endpoint.substr(pos);
}

function findFreePort() {
  while (true) {
    const port = Math.floor(Math.random() * (65536 - 1024)) + 1024;
    const free = testPort("tcp://0.0.0.0:" + port);

    if (free) {
      return port;
    }
  }

  return 8529;
}

function main(argv) {
  let thePython = 'python';
  if (fs.exists('build/CMakeCache.txt')) {
    let CMakeCache = fs.readFileSync('build/CMakeCache.txt');
    thePython = CMakeCache.toString().match(/^PYTHON_EXECUTABLE:FILEPATH=(.*)$/m)[1];
  }
  let options = {};
  let serverEndpoint = '';
  let startServer = true;
  let instanceInfo = {};
  let serverCrashed = false;
  let protocol = 'tcp';
  let tmpDataDir = fs.getTempFile();
  let count = 0;

  try {
    options = internal.parseArgv(argv, 0);
  } catch (x) {
    print("failed to parse the options: " + x.message);
    return -1;
  }
  if (options.hasOwnProperty('withPython')) {
    thePython = options.withPython;
  }

  if (options.hasOwnProperty('onlyThisOne')) {
    scriptArguments.onlyThisOne = options.onlyThisOne;
  }

  if (options.hasOwnProperty('server.endpoint')) {
    if (scriptArguments.hasOwnProperty('onlyThisOne')) {
      throw("don't run the full suite on pre-existing servers");
    }
    startServer = false;
    serverEndpoint = options['server.endpoint'];
    
  }

  let args = [theScript].concat(internal.toArgv(scriptArguments));
  args = args.concat(['--avocadoshSetup']);
  args = args.concat(documentationSourceDirs);

  let res = executeExternalAndWait(thePython, args);

  if (res.exit !== 0) {
    print("parsing the examples failed - aborting!");
    print(res);
    return -1;
  }

  if (startServer) {
    let port = findFreePort();
    instanceInfo.port = port;
    serverEndpoint = protocol + "://127.0.0.1:" + port;

    instanceInfo.url = endpointToURL(serverEndpoint);

    fs.makeDirectoryRecursive(fs.join(tmpDataDir, "data"));

    let serverArgs = {};
    fs.makeDirectoryRecursive(fs.join(tmpDataDir, "apps"));

    serverArgs["configuration"] = "none";
    serverArgs["database.directory"] = fs.join(tmpDataDir, "data");
    serverArgs["javascript.app-path"] = fs.join(tmpDataDir, "apps");
    serverArgs["javascript.startup-directory"] = "js";
    serverArgs["javascript.module-directory"] = "enterprise/js";
    serverArgs["log.file"] = fs.join(tmpDataDir, "log");
    serverArgs["server.authentication"] = "false";
    serverArgs["server.endpoint"] = serverEndpoint;
    serverArgs["server.threads"] = "3";

    print("================================================================================");
    print(AVOCADOD);
    print(toArgv(serverArgs));
    locateAvocadod();
    instanceInfo.pid = executeExternal(AVOCADOD, toArgv(serverArgs)).pid;

    // Wait until the server is up:
    count = 0;
    instanceInfo.endpoint = serverEndpoint;

    while (true) {
      wait(0.5, false);
      let r = download(instanceInfo.url + "/_api/version", "");

      if (!r.error && r.code === 200) {
        break;
      }

      count++;

      if (count % 60 === 0) {
        res = statusExternal(instanceInfo.pid, false);

        if (res.status !== "RUNNING") {
          print("start failed - process is gone: " + yaml.safeDump(res));
          return 1;
        }
      }
    }
  }

  let avocadoshArgs = {
    'configuration': fs.join(fs.makeAbsolute(''), 'etc', 'relative', 'avocadosh.conf'),
    'server.password': "",
    'server.endpoint': serverEndpoint,
    'javascript.execute': scriptArguments.outputFile
  };

  locateAvocadosh();
  print("--------------------------------------------------------------------------------");
  print(AVOCADOSH);
  print(internal.toArgv(avocadoshArgs));
  res = executeExternalAndWait(AVOCADOSH, internal.toArgv(avocadoshArgs));

  if (startServer) {
    if (typeof(instanceInfo.exitStatus) === 'undefined') {
      download(instanceInfo.url + "/_admin/shutdown", "", {method: "DELETE"});

      print("Waiting for server shut down");
      count = 0;
      let bar = "[";

      while (1) {
        instanceInfo.exitStatus = statusExternal(instanceInfo.pid, false);

        if (instanceInfo.exitStatus.status === "RUNNING") {
          count++;
          if (typeof(options.valgrind) === 'string') {
            wait(1);
            continue;
          }
          if (count % 10 === 0) {
            bar = bar + "#";
          }
          if (count > 600) {
            print("forcefully terminating " + yaml.safeDump(instanceInfo.pid) +
              " after 600 s grace period; marking crashy.");
            serverCrashed = true;
            killExternal(instanceInfo.pid);
            break;
          } else {
            wait(1);
          }
        } else if (instanceInfo.exitStatus.status !== "TERMINATED") {
          if (instanceInfo.exitStatus.hasOwnProperty('signal')) {
            print("Server shut down with : " +
              yaml.safeDump(instanceInfo.exitStatus) +
              " marking build as crashy.");
            serverCrashed = true;
            break;
          }
          if (internal.platform.substr(0, 3) === 'win') {
            // Windows: wait for procdump to do its job...
            statusExternal(instanceInfo.monitor, true);
          }
        } else {
          print("Server shutdown: Success.");
          break; // Success.
        }
      }

      if (count > 10) {
        print("long Server shutdown: " + bar + ']');
      }

    }
  }

  if (res.exit != 0) {
    throw("generating examples failed!");
  }

  return 0;
}

main(ARGUMENTS);
