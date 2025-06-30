# Use a base image with build tools and Python
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    g++ \
    make \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory
WORKDIR /app

# Copy everything into the container
COPY . .

# Build the C++ shared library
WORKDIR /app/cpp
RUN make

# Set library path so the Python script can find the .so
ENV LD_LIBRARY_PATH=/app/cpp

# Return to the root app directory to run the Python script
WORKDIR /app

# Default command
CMD ["python3", "engine.py"]
