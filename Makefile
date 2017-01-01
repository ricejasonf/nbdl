# Targets here are just a convenience for using
# docker to manage the development environment.

image:
	docker pull ricejasonf/clang && docker build -t nbdl:dev .

develop: 
	docker run --rm -it -v ${shell pwd}:/opt/src:ro nbdl:dev
