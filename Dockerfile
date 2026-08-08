# Build Stage
FROM ubuntu:22.04 AS builder
RUN apt-get update && apt-get install -y g++ make
WORKDIR /app
COPY . .
# Compile main.cpp with static linking for Windows sockets if cross-compiling, or plain g++ for Linux:
RUN g++ main.cpp -o server -lpthread

# Final Run Stage
FROM ubuntu:22.04
WORKDIR /app
COPY --from=builder /app/server .
COPY --from=builder /app/public ./public

EXPOSE 8080
CMD ["./server"]