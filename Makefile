default: release

.DEFAULT:
	cd src && $(MAKE) $@
test: release
	./test.sh