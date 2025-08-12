# Use a base image with build tools and Python
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    g++ \
    make \
    cmake 

# Set the working directory
WORKDIR /app

# Copy everything into the container
COPY . .

# Build the C++ shared library
WORKDIR /app
RUN make clean
RUN make main

# Set library path so the Python script can find the .so

# Return to the root app directory to run the Python script
WORKDIR /app

# Default command
CMD ["/app/main"]
