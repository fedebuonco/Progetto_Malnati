# I don't know why but with ubuntu 20.04 (the LTS) we get some errors ( I still have to try 18.04)
FROM ubuntu:20.10
#In order to not be distrubed setting locales or time zones
ARG DEBIAN_FRONTEND=noninteractive
ARG LD_LIBRARY_PATH=/usr/local/lib
# Installin essentials
RUN apt-get update && apt-get install -y build-essential cmake git autoconf libtool libboost-dev pkg-config tree
# Installing crypto++
RUN git clone https://github.com/weidai11/cryptopp.git \
&& /bin/bash -c 'cd cryptopp; make dynamic; make install PREFIX=/usr/local'
# adding to ld library path also /usr/local/lib and refreshing it
RUN LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
RUN export LD_LIBRARY_PATH && ldconfig
# copy and building of our app
COPY . /app
WORKDIR /app
RUN cmake . && make
# We set the working directory because we use some relative path in the configs
WORKDIR /app
RUN mkdir /cmake-build-debug && cd cmake-build-debug && mkdir Prova
RUN cp ./client ./cmake-build-debug
WORKDIR /app/cmake-build-debug
CMD ["./client","-s","172.28.0.1","3333"]
#CMD ["tree","/usr/local/lib"]
#CMD ["tree","./"]