#!/bin/bash

FROM=$1
TO=$2
mkdir -p "${TO}"
rsync -a --exclude "*.a" "${FROM}/" "${TO}"
