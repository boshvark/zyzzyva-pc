Z_MAKEFILE = zyzzyva.mak

.PHONY : zyzzyva

all: zyzzyva

zyzzyva:
	qmake MAKEFILE=$(Z_MAKEFILE) zyzzyva.pro
	$(MAKE) -f $(Z_MAKEFILE)

clean:
	$(MAKE) -f $(Z_MAKEFILE) clean
