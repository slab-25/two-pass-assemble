FROM ubuntu:16.04

# Install system dependencies
RUN apt-get update && apt-get install -y     build-essential     gcc     gdb     make     bash     nano     && apt-get clean

# Set default gcc flags for C90 compliance
ENV CFLAGS="-std=c90 -Wall -Wextra -pedantic"

# Create working directory (mapped via docker-compose)
WORKDIR /project
