# Targets here are just a convenience for using
# docker to manage the development environment.

image_develop:
	docker pull ricejasonf/clang && docker build -f ./docker/Dockerfile-develop -t nbdl:develop .

develop: 
	docker run --rm -it -v ${shell pwd}:/opt/src:ro nbdl:develop

image_web_develop:
	docker pull ricejasonf/clang && docker build -f ./docker/Dockerfile-web_develop -t nbdl:web_develop .

web_develop: 
	docker run --rm -it -v ${shell pwd}:/opt/src:ro nbdl:web_develop
