.PHONY: image_tvossimulator_sysroot metabench
# Targets here are just a convenience for using
# docker to manage the development environment.

SOURCE_DIR = ${shell pwd}

image_develop:
	docker build --no-cache=true -f ./docker/Dockerfile-develop -t nbdl:develop .

develop:
	docker run --rm -it -v ${shell pwd}:/opt/src:ro nbdl:develop

image_web_develop:
	docker build --no-cache=true -f ./docker/Dockerfile-web_develop -t nbdl:web_develop .

web_develop:
	docker run --rm -it -v ${shell pwd}:/opt/src:ro nbdl:web_develop

image_tvossimulator_sysroot:
	$(eval SYSROOT ?= /Applications/Xcode.app/Contents/Developer/Platforms/AppleTVSimulator.platform/Developer/SDKs/AppleTVSimulator.sdk)
	bsdtar -cf - \
		-s ',^,foo/,' \
		./docker/Dockerfile-sysroot /usr/include /usr/lib \
		-C ${SYSROOT}/ ./usr ./System/Library/Frameworks \
	| docker build -f ./foo/docker/Dockerfile-sysroot -t tvossimulator_sysroot -

image_tvossimulator_develop:
	docker build -f ./docker/Dockerfile-tvossimulator_develop \
		-t nbdl:tvossimulator_develop .

tvossimulator_develop:
	docker run --rm -it \
		-v ${SOURCE_DIR}:/opt/src:ro \
		nbdl:tvossimulator_develop

	# UIKit include directory
	#${SYSROOT}/System/Library/Frameworks/UIKit.framework/Headers \

# METABENCH

image_metabench:
	docker pull ricejasonf/emscripten \
	&& docker build --no-cache=true -f ./docker/Dockerfile-metabench -t nbdl:metabench .

metabench:
	docker run --rm -it -p 8080:8080 -v ${shell pwd}:/opt/src:ro nbdl:metabench
