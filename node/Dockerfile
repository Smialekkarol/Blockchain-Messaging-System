FROM ubuntu:20.04

RUN mkdir /workspace

RUN apt-get update && apt-get upgrade -y

RUN DEBIAN_FRONTEND=noninteractive apt install -y \
  git \
  g++ \
  make \
  build-essential \
  wget \
  pkg-config \
  cmake \
  vim \
  openssh-server \
  sudo \
  openssl \
  libssl-dev \
  libsasl2-dev \
  libev-dev

# libraries
RUN cd /home && wget https://downloads.sourceforge.net/project/boost/boost/1.80.0/boost_1_80_0.tar.gz \
  && tar xfz boost_1_80_0.tar.gz \
  && rm boost_1_80_0.tar.gz \
  && cd boost_1_80_0 \
  && ./bootstrap.sh \
  && ./b2 install \
  && cd /home \
  && rm -rf boost_1_80_0

RUN cd /home && wget https://github.com/google/googletest/archive/refs/tags/release-1.12.1.tar.gz \
  && tar xfz release-1.12.1.tar.gz \
  && rm release-1.12.1.tar.gz \
  && cd googletest-release-1.12.1 \
  && mkdir build \
  && cd build \
  && cmake .. \
  && make -j $(nproc) \
  && make install \
  && make clean \
  && ldconfig \
  && cd .. \
  && rm -rf googletest-release-1.12.1

RUN cd /home && wget https://github.com/akheron/jansson/releases/download/v2.14/jansson-2.14.tar.gz \
  && tar xfz jansson-2.14.tar.gz \
  && rm jansson-2.14.tar.gz \
  && cd jansson-2.14 \
  && ./configure \
  && make -j $(nproc) \
  && make install \
  && make clean \
  && cd .. \
  && rm -rf jansson-2.14

RUN cd /home && git clone https://github.com/CopernicaMarketingSoftware/AMQP-CPP.git \
  && cd AMQP-CPP \
  && mkdir build \
  && cd build \
  && cmake .. -DAMQP-CPP_BUILD_SHARED=ON -DAMQP-CPP_LINUX_TCP=ON \
  && cmake --build . --target install \
  && cd ../.. \
  && rm -rf AMQP-CPP

RUN cd /home && wget https://download.redis.io/redis-stable.tar.gz \
  && tar -xzvf redis-stable.tar.gz \
  && rm -rf redis-stable.tar.gz \
  && cd redis-stable \
  && make -j $(nproc) \
  && make install \
  && make clean \
  && ldconfig \
  && cd .. \
  && rm -rf redis-stable

RUN cd /home && git clone https://github.com/sewenew/redis-plus-plus.git \
  && cd redis-plus-plus \
  && mkdir build \
  && cd build \
  && cmake .. \
  && make -j $(nproc) \
  && make install \
  && make clean \
  && ldconfig \
  && cd ../.. \
  && rm -rf hiredis

RUN cd /home && git clone https://github.com/gabime/spdlog.git \
  && cd spdlog \
  && mkdir build \
  && cd build \
  && cmake .. \
  && make -j $(nproc) \
  && make install \
  && make clean \
  && cd ../.. \
  && rm -rf spdlog

RUN cd /home && git clone https://github.com/jbeder/yaml-cpp.git \
  && cd yaml-cpp \
  && mkdir build \
  && cd build \
  && cmake .. -DYAML_BUILD_SHARED_LIBS=ON \
  && make -j $(nproc) \
  && make install \
  && make clean \
  && cd ../.. \
  && rm -rf yaml-cpp

COPY install_rabbitmq.sh /home/install_rabbitmq.sh
RUN /home/install_rabbitmq.sh

CMD ["bash"]
