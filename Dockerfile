FROM debian:buster-slim AS build

ENV VFRB_LINK_STATIC="yes" \
    VFRB_BIN_TAG="docker"

COPY . /vfrb/
RUN echo -en "$VFRB_BIN_TAG" > /vfrb/version.txt
WORKDIR /vfrb
RUN ./run.sh build -y
RUN mkdir -p /opt && \
    mv build/vfrb-docker healthcheck.sh /opt/ && \
    mv vfrb.conf.in /opt/vfrb.conf

FROM debian:buster-slim

LABEL maintainer="Jarthianur <jarthianur.github@gmail.com>" \
    app="VirtualFlightRadar-Backend" \
    description="VirtualFlightRadar-Backend running in a docker container." \
    url="https://github.com/Jarthianur/VirtualFlightRadar-Backend"

EXPOSE 4353

RUN apt update -qq && apt dist-upgrade -y -qq && \
    apt install -y -qq \
      curl && \
    rm -rf /var/lib/apt/lists/*

RUN useradd -s /bin/bash -m -d /opt/vfrb -U -c '' vfrb

COPY --from=build /opt/* /opt/vfrb/

RUN chown -R vfrb. /opt/vfrb

USER vfrb

HEALTHCHECK --interval=5m \
    --timeout=10s \
    --start-period=10s \
    --retries=1 \
    CMD [ "/opt/vfrb/healthcheck.sh" ]

ENTRYPOINT [ "/opt/vfrb/vfrb-docker", "-c", "/opt/vfrb/vfrb.conf" ]
