FROM debian:stretch-slim

LABEL maintainer="Jarthianur jarthianur.github@gmail.com" \
    app="VirtualFlightRadar-Backend" \
    description="VirtualFlightRadar-Backend running in a docker container." \
    url="https://github.com/Jarthianur/VirtualFlightRadar-Backend"

EXPOSE 4353

ENV VFRB_VERSION="dock"

RUN apt-get update && \
    apt-get install -no-install-recommends -y curl && \
    apt-get autoremove && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

COPY . /tmp/vfrb/
WORKDIR /tmp/vfrb
RUN ./run.sh build -y
RUN mkdir -p /opt && \
    mv build/vfrb-dock vfrb.ini healthcheck.sh /opt/ && \
    rm -rf /tmp/vfrb

HEALTHCHECK --interval=5m \
    --timeout=10s \
    --start-period=10s \
    --retries=1 \
    CMD [ "/opt/healthcheck.sh" ]

ENTRYPOINT [ "/opt/vfrb-dock", "-c", "/opt/vfrb.ini" ]
