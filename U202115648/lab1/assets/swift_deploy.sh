#!/bin/bash

git clone https://github.com/cs-course/openstack-swift-docker.git

# build docker image
docker run -v /srv --name SWIFT_DATA busybox

# prepare datavolumn
docker run -v /srv --name SWIFT_DATA busybox

# run container
docker run -d --name openstack-swift -p 12345:8080 --volumes-from SWIFT_DATA -t openstack-swift-docker
