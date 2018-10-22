FROM debian:stretch-slim



EXPOSE 4353

ENV VFRB_OPT="-c /opt/vfrb.ini"

RUN apt-get update && apt-get -y install git

USER 1000

WORKDIR /tmp
RUN git clone https://github.com/Jarthianur/VirtualFlightRadar-Backend.git --branch dev-3 --single-branch
WORKDIR /tmp/VirtualFlightRadar-Backend
RUN ls -la && ./run.sh build -y
RUN mkdir /opt && mv build/vfrb vfrb.ini /opt/
RUN rm -rf /tmp/VirtualFlightRadar-Backend

ENTRYPOINT [ "/opt/vfrb" ]
CMD [ "${VFRB_OPT}" ]
