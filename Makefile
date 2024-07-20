

all: scons run

scons:
	@scons

run:
	@scons run

reset:
	@make clean
	@scons

debug:
	@scons debug

bochs:
	@scons bochs

clean:
	@clear
	@scons -c

.PHONY: all run reset debug bochs scons clean