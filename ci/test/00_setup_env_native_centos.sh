#!/usr/bin/env bash
#
# Copyright (c) 2020-present The Gotham Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

export LC_ALL=C.UTF-8

export CONTAINER_NAME=ci_native_centos
export CI_IMAGE_NAME_TAG="quay.io/centos/centos:stream10"
export CI_BASE_PACKAGES="gcc-c++ glibc-devel libstdc++-devel ccache make ninja-build git python3 python3-pip which patch xz procps-ng ksh rsync coreutils bison e2fsprogs cmake"
export PIP_PACKAGES="pyzmq"
export GOAL="install"
export GOTHAM_CONFIG="\
 -DWITH_ZMQ=ON \
 -DBUILD_GUI=ON \
 -DREDUCE_EXPORTS=ON \
 -DAPPEND_CPPFLAGS='-D_GLIBCXX_ASSERTIONS' \
 -DCMAKE_BUILD_TYPE=Debug \
"
