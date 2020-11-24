#!/bin/bash

for external_dep in bem fetk geoflow_c pb_s_am
do
  echo cd $external_dep
  cd $external_dep
  echo 'git checkout master && git pull'
  git checkout master && git pull
  echo cd ..
  cd ..
  echo git add $external_dep
  git add $external_dep
done

echo "git commit -m 'updating submodule to latest'"
git commit -m "updating submodule to latest"
