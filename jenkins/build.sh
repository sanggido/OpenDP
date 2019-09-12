docker build -f jenkins/Dockerfile.dev -t opendp .
docker run -v $(pwd):/opendp opendp bash -c "./opendp/jenkins/install.sh"