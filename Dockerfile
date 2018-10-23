FROM debian:stretch-slim

EXPOSE 4353

ENV VFRB_RUN_OPT="" \
    VFRB_VERSION="dock"

COPY . /tmp/vfrb/
WORKDIR /tmp/vfrb
RUN ./run.sh build -y
RUN mkdir -p /opt && mv build/vfrb-dock vfrb.ini /opt/ && rm -rf /tmp/vfrb && chown -R $(id -u) /opt

ENTRYPOINT [ "/opt/vfrb-dock", "-c", "/opt/vfrb.ini" ]
