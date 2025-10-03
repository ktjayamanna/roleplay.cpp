# Development Dockerfile for C Web Server
# This creates a minimal development environment for learning C

FROM ubuntu:22.04

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install essential development tools
RUN apt-get update && apt-get install -y \
    gcc \
    g++ \
    make \
    gdb \
    valgrind \
    nano \
    curl \
    netcat-openbsd \
    tree \
    git \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /workspace

# Copy project files into the container
COPY . /workspace

# Expose port 8080 for the web server
EXPOSE 8080

# Keep container running for development
CMD ["tail", "-f", "/dev/null"]

