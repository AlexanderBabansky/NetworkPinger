# Use the official Ubuntu image as the base image
FROM ubuntu:latest AS build

# Set the working directory in the container
WORKDIR /app

RUN export DEBIAN_FRONTEND=noninteractive
# Install necessary dependencies
RUN apt-get update && apt-get install -y \
    g++ \
    gcc \
    python3 \
    python3-pip \
    python3-venv \    
    cmake

RUN python3 -m venv myenv
RUN myenv/bin/pip install conan
RUN myenv/bin/conan profile detect
COPY . /app/src

RUN myenv/bin/conan install src/conanfile.txt -of=build --build=missing

WORKDIR /app/build

RUN cmake ../src -DCMAKE_BUILD_TYPE=Release
RUN make
RUN cp /app/build/microservice/NetworkPingerService /app/

# Stage 2
FROM ubuntu:latest
COPY --from=build /app/NetworkPingerService /usr/local/bin/
CMD ["/usr/local/bin/NetworkPingerService"]
