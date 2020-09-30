FROM ubuntu
WORKDIR /root

# Install dependencies
RUN apt update && apt upgrade
RUN apt install -y git

# Clone repository
RUN git clone https://github.com/Electrostatics/apbs.git apbs
RUN cd apbs && git checkout master
RUN cd apbs && git submodule init && git submodule update
