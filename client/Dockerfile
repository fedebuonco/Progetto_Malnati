#FROM ubuntu:20.04
#ARG DEBIAN_FRONTEND=noninteractive
#RUN apt-get update && \
#	apt-get install -y build-essential cmake git autoconf libtool pkg-config libcrypto++-dev libboost-all-dev

FROM cpp-build-base:0.1.0 AS build
WORKDIR /src
COPY . ./
RUN cmake . && make