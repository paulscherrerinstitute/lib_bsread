# $Header: /cvs/G/CRLOGIC/Makefile,v 1.4 2012/05/25 06:50:13 ebner Exp $

LIBVERSION = 4.4.4
export LIBVERSION

build:
	$(MAKE) build -C src/CRLOGICCore
	$(MAKE) build -C src/CRLOGICTimestamp
	$(MAKE) build -C src/CRLOGICMaxV
	$(MAKE) build -C src/CRLOGICVSC16
	$(MAKE) build -C src/CRLOGICDCR508
	$(MAKE) build -C src/CRLOGICVME58
	$(MAKE) build -C src/CRLOGICVME58E
	$(MAKE) build -C src/CRLOGICECM5xx
	$(MAKE) build -C src/CRLOGICHy8001Trigger
	$(MAKE) build -C src/CRLOGICHy8401
	$(MAKE) build -C src/CRLOGICChannel

clean:
	$(MAKE) clean -C src/CRLOGICCore
	$(MAKE) clean -C src/CRLOGICTimestamp
	$(MAKE) clean -C src/CRLOGICMaxV
	$(MAKE) clean -C src/CRLOGICVSC16
	$(MAKE) clean -C src/CRLOGICDCR508
	$(MAKE) clean -C src/CRLOGICVME58
	$(MAKE) clean -C src/CRLOGICVME58E
	$(MAKE) clean -C src/CRLOGICECM5xx
	$(MAKE) clean -C src/CRLOGICHy8001Trigger
	$(MAKE) clean -C src/CRLOGICHy8401
	$(MAKE) clean -C src/CRLOGICChannel

install: build
	$(MAKE) install -C src/CRLOGICCore
	$(MAKE) install -C src/CRLOGICTimestamp $(PROJECTVARS)
	$(MAKE) install -C src/CRLOGICMaxV $(PROJECTVARS)
	$(MAKE) install -C src/CRLOGICVSC16 $(PROJECTVARS)
	$(MAKE) install -C src/CRLOGICDCR508 $(PROJECTVARS)
	$(MAKE) install -C src/CRLOGICVME58 $(PROJECTVARS)
	$(MAKE) install -C src/CRLOGICVME58E $(PROJECTVARS)
	$(MAKE) install -C src/CRLOGICECM5xx $(PROJECTVARS)
	$(MAKE) install -C src/CRLOGICHy8001Trigger $(PROJECTVARS)
	$(MAKE) install -C src/CRLOGICHy8401 $(PROJECTVARS)
	$(MAKE) install -C src/CRLOGICChannel

uninstall:
	rm /work/sls/config/medm/G_CRLOGIC_expert.adl

	$(MAKE) uninstall -C src/CRLOGICCore
	$(MAKE) uninstall -C src/CRLOGICTimestamp
	$(MAKE) uninstall -C src/CRLOGICMaxV
	$(MAKE) uninstall -C src/CRLOGICVSC16
	$(MAKE) uninstall -C src/CRLOGICDCR508
	$(MAKE) uninstall -C src/CRLOGICVME58
	$(MAKE) uninstall -C src/CRLOGICVME58E
	$(MAKE) uninstall -C src/CRLOGICECM5xx
	$(MAKE) uninstall -C src/CRLOGICHy8001Trigger
	$(MAKE) uninstall -C src/CRLOGICHy8401
	$(MAKE) uninstall -C src/CRLOGICChannel

medm:
	cp App/config/medm/G_CRLOGIC_expert.adl /work/sls/config/medm

help:
	@echo "The following targets are available with this Makefile:-"
	@echo "make (calls default target)"
	@echo "make build (default)"
	@echo "make clean"
	@echo "make install"
	@echo "make uninstall"
	@echo "make help"
