#!/bin/bash

mkdir src/proto
cd proto
~/sw/nanopb/protoc --nanopb_out=../src/proto portal.proto
cd ..
