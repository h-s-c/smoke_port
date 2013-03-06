#!/bin/bash

shopt -s globstar
shopt -s nullglob
for file in ./../media/**/*.mesh
do
 OgreMeshUpgrader -b "$file"
done
