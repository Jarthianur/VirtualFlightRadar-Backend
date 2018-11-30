FROM alpine:3.8 AS build

ENV VFRB_VERSION="dock" \
    VFRB_LINK_STATIC="yes"

RUN apk add --no-cache bash

COPY . /tmp/vfrb/
WORKDIR /tmp/vfrb
RUN ./run.sh build -y
RUN mkdir /opt && \
    mv build/vfrb-dock vfrb.ini healthcheck.sh /opt/

FROM alpine:3.8

LABEL maintainer="Jarthianur <jarthianur.github@gmail.com>" \
    app="VirtualFlightRadar-Backend" \
    description="VirtualFlightRadar-Backend running in a docker container." \
    url="https://github.com/Jarthianur/VirtualFlightRadar-Backend"

EXPOSE 4353

RUN apk add --no-cache libstdc++ curl

COPY --from=build /opt/* /opt/

HEALTHCHECK --interval=5m \
    --timeout=10s \
    --start-period=10s \
    --retries=1 \
    CMD [ "/opt/healthcheck.sh" ]

ENTRYPOINT [ "/opt/vfrb-dock", "-c", "/opt/vfrb.ini" ]
