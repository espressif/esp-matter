#!/bin/bash

. config.sh

required_packages=($SOX_APP $INOTIFY_WAIT_APP)


check_installed_packages()
{
  for package in ${required_packages[@]}; do 
    check_if_package_exists $package; 
  done
}


init()
{
  check_installed_packages	
}
