# Targets here are just a convenience for using
# docker to manage the development environment.

image_develop:
	docker pull ricejasonf/emscripten \
	&& docker build --no-cache=true -f ./docker/Dockerfile-develop -t nbdl:develop .

develop: 
	docker run --rm -it -v ${shell pwd}:/opt/src:ro nbdl:develop

image_web_develop:
	docker pull ricejasonf/emscripten \
	&& docker build --no-cache=true -f ./docker/Dockerfile-web_develop -t nbdl:web_develop .

web_develop: 
	docker run --rm -it -v ${shell pwd}:/opt/src:ro nbdl:web_develop

image_tvossimulator_develop:
	docker pull ricejasonf/emscripten \
	&& docker build --no-cache=true -f ./docker/Dockerfile-tvossimulator_develop \
		-t nbdl:tvossimulator_develop .

tvossimulator_develop: 
	docker run --rm -it -v ${shell pwd}:/opt/src:ro nbdl:tvossimulator_develop
