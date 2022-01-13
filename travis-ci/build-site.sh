#!/bin/bash

html_base_dir="html"

root=$(git rev-parse --show-toplevel)
cd "$root"

# create base dir for HTML files
mkdir $html_base_dir

# run make
echo "make clean"
echo "##########################################################################"
time make clean
echo "make html"
echo "##########################################################################"
time make html
echo "rsync [...]"
echo "##########################################################################"
time rsync -a --exclude-from=".rsync-exclude" * $html_base_dir/
