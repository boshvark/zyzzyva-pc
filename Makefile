Z_MAKEFILE = zyzzyva.mak
Z_QMAKE_FLAGS =

.PHONY : zyzzyva

all: zyzzyva

zyzzyva:
	qmake "$(Z_QMAKE_FLAGS)" zyzzyva.pro
	$(MAKE) -f $(Z_MAKEFILE)

clean:
	$(MAKE) -f $(Z_MAKEFILE) clean
