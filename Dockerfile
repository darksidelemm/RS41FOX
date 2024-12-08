FROM debian:bookworm-slim

RUN apt update && \
    apt upgrade -y && \
    apt install -y --no-install-recommends \
    cmake \
    make \
    g++ \
    gcc-arm-none-eabi


COPY docker_build.sh /build.sh
RUN chmod +x /build.sh

ENTRYPOINT ["/bin/bash", "/build.sh"]