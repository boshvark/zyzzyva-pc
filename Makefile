Z_MAKEFILE = zyzzyva.mak

all: zyzzyva

zyzzyva:
	qmake zyzzyva.pro
	$(MAKE) -f $(Z_MAKEFILE)

clean:
	$(MAKE) -f $(Z_MAKEFILE) clean
