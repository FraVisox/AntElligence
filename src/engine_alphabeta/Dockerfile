# Use a base image with build tools and Python
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    g++ \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

# Set workdir
WORKDIR /app

# Copy files
COPY . .

# Build the shared object
RUN ./build.sh

# Default command to run the Python script
CMD ["python3", "use_module.py"]