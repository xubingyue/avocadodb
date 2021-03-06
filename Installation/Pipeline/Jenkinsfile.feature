//  -*- mode: groovy-mode

properties(
    [[
      $class: 'BuildDiscarderProperty',
      strategy: [$class: 'LogRotator',
                 artifactDaysToKeepStr: '3',
                 artifactNumToKeepStr: '5',
                 daysToKeepStr: '3',
                 numToKeepStr: '5']
    ]]
)

def defaultLinux = true
def defaultMac = false
def defaultWindows = false
def defaultBuild = true
def defaultCleanBuild = false
def defaultCommunity = true
def defaultEnterprise = true
def defaultRunResilience = false
def defaultRunTests = false
def defaultFullParallel = false

properties([
    parameters([
        booleanParam(
            defaultValue: defaultLinux,
            description: 'build and run tests on Linux',
            name: 'Linux'
        ),
        booleanParam(
            defaultValue: defaultMac,
            description: 'build and run tests on Mac',
            name: 'Mac'
        ),
        booleanParam(
            defaultValue: defaultWindows,
            description: 'build and run tests in Windows',
            name: 'Windows'
        ),
        booleanParam(
            defaultValue: defaultFullParallel,
            description: 'build all os in parallel',
            name: 'fullParallel'
        ),
        booleanParam(
            defaultValue: defaultCleanBuild,
            description: 'clean build directories',
            name: 'cleanBuild'
        ),
        booleanParam(
            defaultValue: defaultCommunity,
            description: 'build and run tests for community',
            name: 'Community'
        ),
        booleanParam(
            defaultValue: defaultEnterprise,
            description: 'build and run tests for enterprise',
            name: 'Enterprise'
        ),
        booleanParam(
            defaultValue: defaultRunResilience,
            description: 'run resilience tests',
            name: 'runResilience'
        ),
        booleanParam(
            defaultValue: defaultRunTests,
            description: 'run tests',
            name: 'runTests'
        )
    ])
])

// start with empty build directory
cleanBuild = params.cleanBuild

// do everything in parallel
fullParallel = params.fullParallel

// build community
useCommunity = params.Community

// build enterprise
useEnterprise = params.Enterprise

// build linux
useLinux = params.Linux

// build mac
useMac = params.Mac

// build windows
useWindows = params.Windows

// run resilience tests
runResilience = params.runResilience

// run tests
runTests = params.runTests

// restrict builds
restrictions = [:]

// -----------------------------------------------------------------------------
// --SECTION--                                             CONSTANTS AND HELPERS
// -----------------------------------------------------------------------------


// github proxy repositiory
proxyRepo = 'http://c1:8088/github.com/avocadodb/avocadodb'

// github repositiory for resilience tests
resilienceRepo = 'http://c1:8088/github.com/avocadodb/resilience-tests'

// github repositiory for enterprise version
enterpriseRepo = 'http://c1:8088/github.com/avocadodb/enterprise'

// Jenkins credentials for enterprise repositiory
credentials = '8d893d23-6714-4f35-a239-c847c798e080'

// source branch for pull requests
sourceBranchLabel = env.BRANCH_NAME

if (env.BRANCH_NAME =~ /^PR-/) {
  def prUrl = new URL("https://api.github.com/repos/avocadodb/avocadodb/pulls/${env.CHANGE_ID}")
  sourceBranchLabel = new groovy.json.JsonSlurper().parseText(prUrl.text).head.label

  def reg = ~/^avocadodb:/
  sourceBranchLabel = sourceBranchLabel - reg
}


// -----------------------------------------------------------------------------
// --SECTION--                                                       SCRIPTS SCM
// -----------------------------------------------------------------------------

def checkoutCommunity() {
    if (cleanBuild) {
        deleteDir()
    }

    retry(3) {
        try {
            checkout(
                changelog: false,
                poll: false,
                scm: [
                    $class: 'GitSCM',
                    branches: [[name: "*/${sourceBranchLabel}"]],
                    doGenerateSubmoduleConfigurations: false,
                    extensions: [],
                    submoduleCfg: [],
                    userRemoteConfigs: [[url: proxyRepo]]])
        }
        catch (exc) {
            echo "GITHUB checkout failed, retrying in 1min"
            sleep 60
            throw exc
        }
    }
}

def checkoutEnterprise() {
    try {
        echo "Trying enterprise branch ${sourceBranchLabel}"

        checkout(
            changelog: false,
            poll: false,
            scm: [
                $class: 'GitSCM',
                branches: [[name: "*/${sourceBranchLabel}"]],
                doGenerateSubmoduleConfigurations: false,
                extensions: [[$class: 'RelativeTargetDirectory', relativeTargetDir: 'enterprise']],
                submoduleCfg: [],
                userRemoteConfigs: [[credentialsId: credentials, url: enterpriseRepo]]])
    }
    catch (exc) {
        echo "Failed ${sourceBranchLabel}, trying enterprise branch devel"

        checkout(
            changelog: false,
            poll: false,
            scm: [
                $class: 'GitSCM',
                branches: [[name: "*/devel"]],
                doGenerateSubmoduleConfigurations: false,
                extensions: [[$class: 'RelativeTargetDirectory', relativeTargetDir: 'enterprise']],
                submoduleCfg: [],
                userRemoteConfigs: [[credentialsId: credentials, url: enterpriseRepo]]])
    }

}

def checkoutResilience() {
    checkout(
        changelog: false,
        poll: false,
        scm: [
            $class: 'GitSCM',
            branches: [[name: "*/master"]],
            doGenerateSubmoduleConfigurations: false,
            extensions: [[$class: 'RelativeTargetDirectory', relativeTargetDir: 'resilience']],
            submoduleCfg: [],
            userRemoteConfigs: [[credentialsId: credentials, url: resilienceRepo]]])

}

def checkCommitMessages() {
    def causes = currentBuild.rawBuild.getCauses()
    def causeDescription = causes[0].getShortDescription();
    def changeLogSets = currentBuild.changeSets
    def seenCommit = false
    def skip = false

    for (int i = 0; i < changeLogSets.size(); i++) {
        def entries = changeLogSets[i].items

        for (int j = 0; j < entries.length; j++) {
            seenCommit = true

            def entry = entries[j]

            def author = entry.author
            def commitId = entry.commitId
            def msg = entry.msg
            def timestamp = new Date(entry.timestamp)

            echo msg

            if (msg ==~ /(?i).*\[ci:[^\]]*clean[ \]].*/) {
                echo "using clean build because message contained 'clean'"
                cleanBuild = true
            }

            if (msg ==~ /(?i).*\[ci:[^\]]*skip[ \]].*/) {
                echo "skipping everything because message contained 'skip'"
                skip = true
            }

            def files = new ArrayList(entry.affectedFiles)

            for (int k = 0; k < files.size(); k++) {
                def file = files[k]
                def editType = file.editType.name
                def path = file.path

                echo "File " + file + ", path " + path
            }
        }
    }

    if (causeDescription =~ /Started by user/) {
        echo "build started by user"
    }
    else if (skip) {
        useLinux = false
        useMac = false
        useWindows = false
        useCommunity = false
        useEnterprise = false
        runResilience = false
        runTests = false
    }
    else {
        if (env.BRANCH_NAME == "devel" || env.BRANCH_NAME == "3.2") {
            echo "build of main branch"

            useLinux = true
            useMac = true
            useWindows = true
            useCommunity = true
            useEnterprise = true
            runResilience = true
            runTests = true
        }
        else if (env.BRANCH_NAME =~ /^PR-/) {
            echo "build of PR"

            useLinux = true
            useMac = true
            useWindows = true
            fullParallel = true
            useCommunity = true
            useEnterprise = true
            runResilience = true
            runTests = true

            restrictions = [
                "build-community-linux" : true,
                "build-community-mac" : true,
                "build-community-windows" : true,
                "build-enterprise-linux" : true,
                "build-enterprise-mac" : true,
                "build-enterprise-windows" : true,
                "test-cluster-community-mmfiles-linux" : true,
                "test-cluster-community-rocksdb-linux" : true,
                "test-cluster-enterprise-mmfiles-linux" : true,
                "test-cluster-enterprise-rocksdb-linux" : true,
                "test-singleserver-community-mmfiles-linux" : true,
                "test-singleserver-community-rocksdb-linux" : true,
                "test-singleserver-enterprise-mmfiles-linux" : true,
                "test-singleserver-enterprise-rocksdb-linux" : true
            ]
        }
        else {
            useLinux = true
            useMac = true
            useWindows = true
            fullParallel = true
            useCommunity = true
            useEnterprise = true
            runResilience = true
            runTests = true

            restrictions = [
                "build-community-mac" : true,
                // "build-community-windows" : true,
                "build-enterprise-linux" : true,
                "test-cluster-enterprise-rocksdb-linux" : true,
                "test-singleserver-community-mmfiles-mac" : true
                // "test-singleserver-community-rocksdb-windows" : true
            ]
        }
    }

    echo """BRANCH_NAME: ${env.BRANCH_NAME}
SOURCE: ${sourceBranchLabel}
CHANGE_ID: ${env.CHANGE_ID}
CHANGE_TARGET: ${env.CHANGE_TARGET}
JOB_NAME: ${env.JOB_NAME}
CAUSE: ${causeDescription}

Linux: ${useLinux}
Mac: ${useMac}
Windows: ${useWindows}
Clean Build: ${cleanBuild}
Full Parallel: ${fullParallel}
Building Community: ${useCommunity}
Building Enterprise: ${useEnterprise}
Running Resilience: ${runResilience}
Running Tests: ${runTests}

Restrictions: ${restrictions.keySet().join(", ")}
"""
}

// -----------------------------------------------------------------------------
// --SECTION--                                                     SCRIPTS STASH
// -----------------------------------------------------------------------------

def stashBinaries(edition, os) {
    stash name: "binaries-${edition}-${os}", includes: "build/bin/**, build/tests/**, build/etc/**, etc/**, Installation/Pipeline/**, js/**, scripts/**, UnitTests/**, utils/**, resilience/**"
}

def unstashBinaries(edition, os) {
    unstash name: "binaries-${edition}-${os}"
}

// -----------------------------------------------------------------------------
// --SECTION--                                                         VARIABLES
// -----------------------------------------------------------------------------

buildJenkins = [
    "linux": "linux && build",
    "mac" : "mac",
    "windows": "windows"
]

buildsSuccess = [:]
allBuildsSuccessful = true

jslintSuccessful = true

testJenkins = [
    "linux": "linux && tests",
    "mac" : "mac",
    "windows": "windows"
]

testsSuccess = [:]
allTestsSuccessful = true

// -----------------------------------------------------------------------------
// --SECTION--                                                    SCRIPTS JSLINT
// -----------------------------------------------------------------------------

def jslint() {
    try {
        sh './Installation/Pipeline/test_jslint.sh'
    }
    catch (exc) {
        jslintSuccessful = false
        throw exc
    }
}

def jslintStep(edition, os) {
    return {
        node(os) {
            stage("jslint-${edition}-${os}") {
                echo "Running jslint test"

                unstashBinaries(edition, os)
                jslint()
            }
        }
    }
}

// -----------------------------------------------------------------------------
// --SECTION--                                                     SCRIPTS TESTS
// -----------------------------------------------------------------------------

def testEdition(edition, os, mode, engine) {
    def arch = "LOG_test_${mode}_${edition}_${engine}_${os}"

    if (os == 'linux' || os == 'mac') {
       sh "rm -rf ${arch}"
       sh "mkdir -p ${arch}"
    }
    else if (os == 'windows') {
        bat "del /F /Q ${arch}"
        powershell "New-Item -ItemType Directory -Force -Path ${arch}"
    }

    try {
        try {
            if (os == 'linux') {
                sh "./Installation/Pipeline/linux/test_${mode}_${edition}_${engine}_${os}.sh 10"
            }
            else if (os == 'mac') {
                sh "./Installation/Pipeline/mac/test_${mode}_${edition}_${engine}_${os}.sh 3"
            }
            else if (os == 'windows') {
                powershell ". .\\Installation\\Pipeline\\windows\\test_${mode}_${edition}_${engine}_${os}.ps1"
            }

            if (os == 'windows') {
                if (findFiles(glob: '*.dmp').length > 0) {
                    error("found dmp file")
                }
            } else {
                if (findFiles(glob: 'core*').length > 0) {
                    error("found core file")
                }
            }
        }
        finally {
            if (os == 'linux' || os == 'mac') {
                sh "find log-output -name 'FAILED_*' -exec cp '{}' . ';'"
                sh "for i in logs log-output; do test -e \"\$i\" && mv \"\$i\" ${arch} || true; done"
                sh "for i in core* tmp; do test -e \"\$i\" && mv \"\$i\" ${arch} || true; done"
                sh "cp -a build/bin/* ${arch}" 
            }
            else if (os == 'windows') {
                powershell "move-item -Force -ErrorAction Ignore logs ${arch}"
                powershell "move-item -Force -ErrorAction Ignore log-output ${arch}"
                powershell "move-item -Force -ErrorAction Ignore .\\build\\bin\\*.dmp ${arch}"
                powershell "move-item -Force -ErrorAction Ignore .\\build\\tests\\*.dmp ${arch}"
                powershell "Copy-Item .\\build\\bin\\* -Include *.exe,*.pdb,*.ilk ${arch}"
            }
        }
    }
    finally {
        archiveArtifacts allowEmptyArchive: true,
                         artifacts: "${arch}/**",
                         defaultExcludes: false

        archiveArtifacts allowEmptyArchive: true,
                         artifacts: "FAILED_*",
                         defaultExcludes: false
    }
}

def testCheck(edition, os, mode, engine) {
    if (! runTests) {
        return false
    }

    if (os == 'linux' && ! useLinux) {
        return false
    }

    if (os == 'mac' && ! useMac) {
        return false
    }

    if (os == 'windows' && ! useWindows) {
        return false
    }

    if (edition == 'enterprise' && ! useEnterprise) {
        return false
    }

    if (edition == 'community' && ! useCommunity) {
        return false
    }

    if (restrictions && !restrictions["test-${mode}-${edition}-${engine}-${os}"]) {
        return false
    }

    return true
}

def testStep(edition, os, mode, engine) {
    return {
        node(testJenkins[os]) {
            def buildName = "${edition}-${os}"

            if (buildsSuccess[buildName]) {
                def name = "${edition}-${os}-${mode}-${engine}"

                stage("test-${name}") {
                    timeout(120) {
                        try {
                            unstashBinaries(edition, os)
                            testEdition(edition, os, mode, engine)
                            testsSuccess[name] = true
                        }
                        catch (exc) {
                            echo "Exception while testing!"
                            echo exc.toString()
                            testsSuccess[name] = false
                            allTestsSuccessful = false
                            throw exc
                        }
                    }
                }
            }
        }
    }
}

def testStepParallel(editionList, osList, modeList) {
    def branches = [:]

    for (edition in editionList) {
        for (os in osList) {
            for (mode in modeList) {
                for (engine in ['mmfiles', 'rocksdb']) {
                    if (testCheck(edition, os, mode, engine)) {
                        def name = "test-${mode}-${edition}-${engine}-${os}";

                        branches[name] = testStep(edition, os, mode, engine)
                    }
                }
            }
        }
    }

    if (branches.size() > 1) {
        parallel branches
    }
    else if (branches.size() == 1) {
        branches.values()[0]()
    }
}

// -----------------------------------------------------------------------------
// --SECTION--                                                SCRIPTS RESILIENCE
// -----------------------------------------------------------------------------

resiliencesSuccess = [:]
allResiliencesSuccessful = true

def testResilience(os, engine, foxx) {
    withEnv(['LOG_COMMUNICATION=debug', 'LOG_REQUESTS=trace', 'LOG_AGENCY=trace']) {
        if (os == 'linux') {
            sh "./Installation/Pipeline/linux/test_resilience_${foxx}_${engine}_${os}.sh"
        }
        else if (os == 'mac') {
            sh "./Installation/Pipeline/mac/test_resilience_${foxx}_${engine}_${os}.sh"
        }
        else if (os == 'windows') {
            powershell ".\\Installation\\Pipeline\\test_resilience_${foxx}_${engine}_${os}.ps1"
        }
    }
}

def testResilienceCheck(os, engine, foxx) {
    if (! runResilience) {
        return false
    }

    if (os == 'linux' && ! useLinux) {
        return false
    }

    if (os == 'mac' && ! useMac) {
        return false
    }

    if (os == 'windows' && ! useWindows) {
        return false
    }

    if (! useCommunity) {
        return false
    }

    if (restrictions && !restrictions["test-resilience-${foxx}-${engine}-${os}"]) {
        return false
    }

    return true
}

def testResilienceStep(os, engine, foxx) {
    return {
        node(testJenkins[os]) {
            def edition = "community"
            def buildName = "${edition}-${os}"

            if (buildsSuccess[buildName]) {
                def name = "${os}-${engine}-${foxx}"
                def arch = "LOG_resilience_${foxx}_${engine}_${os}"

                stage("resilience-${name}") {
                    if (os == 'linux' || os == 'mac') {
                       sh "rm -rf ${arch}"
                       sh "mkdir -p ${arch}"
                    }
                    else if (os == 'windows') {
                        bat "del /F /Q ${arch}"
                        powershell "New-Item -ItemType Directory -Force -Path ${arch}"
                    }

                    try {
                        try {
                            timeout(120) {
                                unstashBinaries(edition, os)
                                testResilience(os, engine, foxx)
                            }

                            if (findFiles(glob: 'resilience/core*').length > 0) {
                                error("found core file")
                            }
                        }
                        catch (exc) {
                            if (os == 'linux' || os == 'mac') {
                                sh "for i in build resilience/core* tmp; do test -e \"\$i\" && mv \"\$i\" ${arch} || true; done"
                            }

                            archiveArtifacts allowEmptyArchive: true,
                                             artifacts: "source.zip",
                                             defaultExcludes: false

                            throw exc
                        }
                        finally {
                            if (os == 'linux' || os == 'mac') {
                                sh "for i in log-output; do test -e \"\$i\" && mv \"\$i\" ${arch}; done"
                            }
                            else if (os == 'windows') {
                                bat "move log-output ${arch}"
                            }
                        }
                    }
                    catch (exc) {
                        resiliencesSuccess[name] = false
                        allResiliencesSuccessful = false

                        throw exc
                    }
                    finally {
                        archiveArtifacts allowEmptyArchive: true,
                                         artifacts: "${arch}/**",
                                         defaultExcludes: false
                    }
                }
            }
        }
    }
}

def testResilienceParallel(osList) {
    def branches = [:]

    for (foxx in ['foxx', 'nofoxx']) {
        for (os in osList) {
            for (engine in ['mmfiles', 'rocksdb']) {
                if (testResilienceCheck(os, engine, foxx)) {
                    def name = "test-resilience-${foxx}-${engine}-${os}"

                    branches[name] = testResilienceStep(os, engine, foxx)
                }
            }
        }
    }

    if (branches.size() > 1) {
        parallel branches
    }
    else if (branches.size() == 1) {
        branches.values()[0]()
    }
}

// -----------------------------------------------------------------------------
// --SECTION--                                                     SCRIPTS BUILD
// -----------------------------------------------------------------------------

def buildEdition(edition, os) {
    def arch = "LOG_build_${edition}_${os}"

    if (os == 'linux' || os == 'mac') {
       sh "rm -rf ${arch}"
       sh "mkdir -p ${arch}"
    }
    else if (os == 'windows') {
        bat "del /F /Q ${arch}"
        powershell "New-Item -ItemType Directory -Force -Path ${arch}"
    }

    try {
        try {
            if (os == 'linux') {
                sh "./Installation/Pipeline/linux/build_${edition}_${os}.sh 64"
            }
            else if (os == 'mac') {
                sh "./Installation/Pipeline/mac/build_${edition}_${os}.sh 16"
            }
            else if (os == 'windows') {
                powershell ". .\\Installation\\Pipeline\\windows\\build_${edition}_${os}.ps1"
            }
        }
        finally {
            if (os == 'linux' || os == 'mac') {
                sh "for i in log-output; do test -e \"\$i\" && mv \"\$i\" ${arch} || true; done"
            }
            else if (os == 'windows') {
                powershell "Move-Item -ErrorAction Ignore -Path log-output/* -Destination ${arch}"
            }
        }
    }
    finally {
        archiveArtifacts allowEmptyArchive: true,
                         artifacts: "${arch}/**",
                         defaultExcludes: false
    }
}

def buildStepCheck(edition, os, full) {
    if (os == 'linux' && ! useLinux) {
        return false
    }

    if (os == 'mac' && ! useMac) {
        return false
    }

    if (os == 'windows' && ! useWindows) {
        return false
    }

    if (edition == 'enterprise' && ! useEnterprise) {
        return false
    }

    if (edition == 'community' && ! useCommunity) {
        return false
    }

    if (restrictions && !restrictions["build-${edition}-${os}"]) {
        return false
    }

    return true
}

def buildStep(edition, os) {
    return {
        node(buildJenkins[os]) {
            def name = "${edition}-${os}"

            try {
                stage("build-${name}") {
                    timeout(30) {
                        checkoutCommunity()
                        checkCommitMessages()
                        if (useEnterprise) {
                            checkoutEnterprise()
                        }
                        checkoutResilience()
                    }

                    timeout(90) {
                        buildEdition(edition, os)
                        stashBinaries(edition, os)
                    }

                    buildsSuccess[name] = true
                }
            }
            catch (exc) {
                buildsSuccess[name] = false
                allBuildsSuccessful = false
                throw exc
            }
        }

        if (fullParallel) {
            testStepParallel([edition], [os], ['cluster', 'singleserver'])
        }
    }
}

def buildStepParallel(osList) {
    def branches = [:]
    def full = false

    for (edition in ['community', 'enterprise']) {
        for (os in osList) {
            if (buildStepCheck(edition, os, full)) {
                branches["build-${edition}-${os}"] = buildStep(edition, os)
            }
        }
    }

    if (branches.size() > 1) {
        parallel branches
    }
    else if (branches.size() == 1) {
        branches.values()[0]()
    }
}

// -----------------------------------------------------------------------------
// --SECTION--                                                          PIPELINE
// -----------------------------------------------------------------------------

def runStage(stage) {
    try {
        stage()
    }
    catch (exc) {
        echo exc.toString()
    }
}

if (fullParallel) {
    runStage { buildStepParallel(['linux', 'mac', 'windows']) }
}
else {
    runStage { buildStepParallel(['linux']) }
    runStage { testStepParallel(['community', 'enterprise'], ['linux'], ['cluster', 'singleserver']) }

    runStage { buildStepParallel(['mac']) }
    runStage { testStepParallel(['community', 'enterprise'], ['mac'], ['cluster', 'singleserver']) }

    runStage { buildStepParallel(['windows']) }
    runStage { testStepParallel(['community', 'enterprise'], ['windows'], ['cluster', 'singleserver']) }

    runStage { testResilienceParallel(['linux', 'mac', 'windows']) }
}

if (buildsSuccess["enterprise-linux"]) {
    jslintStep('enterprise', 'linux')
}
else if (buildsSuccess["community-linux"]) {
    jslintStep('community', 'linux')
}
else if (buildsSuccess["enterprise-mac"]) {
    jslintStep('enterprise', 'mac')
}
else if (buildsSuccess["community-mac"]) {
    jslintStep('community', 'mac')
}

stage('result') {
    node('master') {
        def result = ""

        if (!jslintSuccessful) {
            result += "JSLINT failed\n"
        }

        for (kv in buildsSuccess) {
            result += "BUILD ${kv.key}: ${kv.value}\n"
        }

        for (kv in testsSuccess) {
            result += "TEST ${kv.key}: ${kv.value}\n"
        }

        for (kv in resiliencesSuccess) {
            result += "RESILIENCE ${kv.key}: ${kv.value}\n"
        }

        if (result == "") {
           result = "All tests passed!"
        }

        echo result

        if (! (allBuildsSuccessful
            && allTestsSuccessful
            && allResiliencesSuccessful
            && jslintSuccessful)) {
            error "run failed"
        }
    }
}
