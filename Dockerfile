FROM mparno/muq:latest

USER root
#RUN apt update && \
#    apt install -y python3-aiohttp python3-requests python3-numpy python3-h5py


USER muq-user

RUN mkdir -p /home/muq-user/server/build

COPY CMakeLists.txt umbridge-localapprox.cpp /home/muq-user/server/

WORKDIR /home/muq-user/server/build

RUN cmake -DMUQ_DIR=~/muq-install/lib/cmake/MUQ .. && \
    make

CMD ./umbridge-localapprox http://localhost:4242
