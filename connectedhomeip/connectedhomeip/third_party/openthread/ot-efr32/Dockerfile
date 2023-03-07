FROM ubuntu:20.04

ENV TZ="America/New_York"
ENV repo_dir="/ot-efr32"

# Install packages
RUN apt-get update && \
      apt-get -y install sudo tzdata && \
      rm -rf /var/lib/apt/lists/*

# Copy scripts
RUN mkdir -p ${repo_dir}/third_party/silabs/slc
COPY ./script ${repo_dir}/script
COPY ./openthread/script ${repo_dir}/openthread/script

# bootstrap
RUN ./ot-efr32/script/bootstrap
