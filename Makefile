Z_MAKEFILE = zyzzyva.mak

all: zyzzyva
	@echo "** done **"

zyzzyva:
	qmake zyzzyva.pro
	$(MAKE) -f $(Z_MAKEFILE)

clean:
	$(MAKE) -f $(Z_MAKEFILE) clean
