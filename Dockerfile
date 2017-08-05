FROM ubuntu:rolling
MAINTAINER Sebastian Wallat "sebastian.wallat@rub.de"

ENV build_path=/home/build
RUN mkdir -p $build_path
WORKDIR $build_path

RUN sed -i -e 's/#force_color_prompt=yes/force_color_prompt=yes/g' /root/.bashrc
# RUN apt update && apt-get install -y software-properties-common python-software-properties && \
    # add-apt-repository ppa:ubuntu-toolchain-r/test -y && \
RUN  apt update && apt-get install -y gperf build-essential clang bison flex libreadline-dev \
        gawk tcl-dev libffi-dev git mercurial graphviz xdot pkg-config python g++-4.8 cmake \
        cmake-extras extra-cmake-modules autoconf gettext python3 libncurses5-dev tcl-dev tk-dev \
        libgtest-dev googletest ccache

RUN git clone git://github.com/steveicarus/iverilog.git && cd iverilog && autoconf && \
    ./configure --prefix=$HOME/iverilog && make && make install && \
    export PATH=$PATH:$HOME/iverilog/bin

ENV PATH=$PATH:$HOME/iverilog/bin

CMD cmake /home/src -DCMAKE_INSTALL_PREFIX=/usr/local/ && \
    cmake --build /home/build --target all --clean-first -- -j4 && \
    ctest /home/build
