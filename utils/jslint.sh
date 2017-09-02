#!/bin/bash

if [ "x$@" == "x" ] ; then
  JAVASCRIPT_JSLINT="\
    `find ./js/actions -name "*.js"` \
    `find ./js/common/bootstrap -name "*.js"` \
    `find ./js/client/bootstrap -name "*.js"` \
    `find ./js/server/bootstrap -name "*.js"` \
    \
    `find ./js/common/modules/@avocadodb -name "*.js"` \
    `find ./js/client/modules/@avocadodb -name "*.js"` \
    `find ./js/server/modules/@avocadodb -name "*.js"` \
    `find ./js/server/tests -name "*.js" | grep -v "ranges-combined"` \
    `find ./js/common/tests -name "*.js"` \
    `find ./js/client/tests -name "*.js"` \
    `find ./UnitTests -name "*.js"` \
    \
    `find ./js/apps/system/_api/gharial/APP -name "*.js"` \
    \
    `find ./js/apps/system/_admin/aardvark/APP/frontend/js/models -name "*.js"` \
    `find ./js/apps/system/_admin/aardvark/APP/frontend/js/views -name "*.js"` \
    `find ./js/apps/system/_admin/aardvark/APP/frontend/js/collections -name "*.js"` \
    `find ./js/apps/system/_admin/aardvark/APP/frontend/js/routers -name "*.js"` \
    `find ./js/apps/system/_admin/aardvark/APP/frontend/js/avocado -name "*.js"` \
    `find ./js/apps/system/_admin/aardvark/APP/test/specs -name "*.js"` \
    `find ./js/apps/system/_admin/aardvark/APP/test/clusterSpecs -name "*.js"` \
    \
    `find ./scripts -name "*.js"` \
    \
    ./js/common/modules/jsunity.js \
    ./js/client/client.js \
    ./js/server/server.js \
    ./js/server/upgrade-database.js \
    \
  "
  if [ -d ./enterprise ] ; then
    echo Considering enterprise files...
    JAVASCRIPT_JSLINT="$JAVASCRIPT_JSLINT \
      `find ./enterprise/js -name "*.js"` \
      "
  fi
else
  JAVASCRIPT_JSLINT="$@"
fi

FILELIST=""

for file in ${JAVASCRIPT_JSLINT}; do
  FILELIST="${FILELIST} --jslint ${file}";
done

if [ -z "${ARANGOSH}" ];  then
  if [ -x build/bin/avocadosh ];  then
    ARANGOSH=build/bin/avocadosh
  elif [ -x bin/avocadosh ];  then
    ARANGOSH=bin/avocadosh
  else
    echo "$0: cannot locate avocadosh"
  fi
fi

exec $ARANGOSH \
    -c none \
    --log.level error \
    --log.file - \
    --server.password "" \
    --javascript.startup-directory ./js \
    ${FILELIST}
