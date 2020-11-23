# I don't know why but with ubuntu 20.04 (the LTS) we get some errors ( I still have to try 18.04)
FROM ubuntu:20.10
#In order to not be distrubed setting locales or time zones
ARG DEBIAN_FRONTEND=noninteractive
ARG LD_LIBRARY_PATH=/usr/local/lib
# Installin essentials
RUN apt-get update && apt-get install -y build-essential cmake git autoconf libtool pkg-config tree
# Installing boost 1.74
RUN apt-get install wget
RUN wget --max-redirect 3 https://dl.bintray.com/boostorg/release/1.74.0/source/boost_1_74_0.tar.gz
RUN mkdir -p /usr/include/boost && tar zxf boost_1_74_0.tar.gz -C /usr/include/boost --strip-components=1
# Installing crypto++
RUN git clone https://github.com/weidai11/cryptopp.git \
&& /bin/bash -c 'cd cryptopp; make dynamic; make install PREFIX=/usr/local'
# adding to ld library path also /usr/local/lib and refreshing it
RUN LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
RUN export LD_LIBRARY_PATH && ldconfig
# copy and building of our app
RUN ls
COPY . /app
RUN cmake /app
# We set the working directory because we use some relative path in the configs
WORKDIR /app/cmake-build-debug/
CMD ["./server"]
#CMD ["tree","./"]