#!/usr/bin/env bash
#
# Copyright (c) 2019-present The Gotham Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

export LC_ALL=C.UTF-8

export HOST=s390x-linux-gnu
export PACKAGES="python3-zmq"
export CONTAINER_NAME=ci_s390x
export CI_IMAGE_NAME_TAG="mirror.gcr.io/ubuntu:24.04"
export CI_IMAGE_PLATFORM="linux/s390x"
export TEST_RUNNER_EXTRA="--exclude rpc_bind,feature_bind_extra"  # Excluded for now, see https://github.com/gotham/gotham/issues/17765#issuecomment-602068547
export RUN_FUNCTIONAL_TESTS=true
export GOAL="install"
export GOTHAM_CONFIG="-DREDUCE_EXPORTS=ON"
