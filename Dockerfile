FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    g++ \
    make \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN git submodule init && git submodule update
RUN make static

FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=builder /app/dist/ ./
COPY config/ ./config/

EXPOSE 8080
ENV LIGHTHOUSE_IP=0.0.0.0
ENV LIGHTHOUSE_PORT=8080

ENTRYPOINT ["./chat"]
CMD ["--ip", "0.0.0.0", "--port", "8080"]
