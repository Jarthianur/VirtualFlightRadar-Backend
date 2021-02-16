FROM alpine AS build

ENV VFRB_LINK_STATIC="yes" \
    VFRB_BIN_TAG="dock"

RUN apk add --no-cache bash

COPY . /tmp/vfrb/
RUN echo -en "$VFRB_BIN_TAG" > /tmp/vfrb/version.txt
WORKDIR /tmp/vfrb
RUN ./run.sh build -y
RUN mkdir /opt && \
    mv build/vfrb-dock healthcheck.sh /opt/ && \
    mv vfrb.ini.in /opt/vfrb.ini

FROM alpine

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
