default: release

.DEFAULT:
	cd src && $(MAKE) $@
test: clean release
	./test.sh