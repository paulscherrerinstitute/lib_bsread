##BSREAD USING EVR
##
## Optional parameters/macros [default]
##  - SYS [$(IOC)]
##  - BSREAD_PORT [9999]
##  - BSREAD_MODE [PUSH]
##	- EVR [EVR0]
##  - BSREAD_EVENT [28]
##	- BSREAD_PULSEID [$(SYS=$(IOC))-$(EVR):RX-PULSEID]
##	- BSREAD_TS_SEC [$(SYS=$(IOC))-$(EVR):RX-MTS-SEC]
##	- BSREAD_TS_NSEC [$(SYS=$(IOC))-$(EVR):RX-MTS-NSEC]
##  - PULSEID_OFFSET [0]
##  - EVENT_NAME [$(EVENT_NAME=bsread$(BSREAD_PORT=9999))]

require bsread
$(NO_EVR=)dbLoadRecords "bsread_evr.template", "P=$(SYS=$(IOC))-BSREAD,EVR=$(EVR=EVR0),BSREAD_EVENT=$(BSREAD_EVENT=28),EVENT_NAME=$(EVENT_NAME=bsread$(BSREAD_PORT=9999))"
dbLoadRecords "bsread.template", "P=$(SYS=$(IOC))-BSREAD$(BSREAD_PORT=9999),BSREAD_PORT=$(BSREAD_PORT=9999),EVENT_NAME=$(EVENT_NAME=bsread$(BSREAD_PORT=9999)),PULSEID_OFFSET=$(PULSEID_OFFSET=0),BSREAD_PULSEID=$(BSREAD_PULSEID=$(SYS=$(IOC))-$(EVR=EVR0):RX-PULSEID),BSREAD_TS_SEC=$(BSREAD_TS_SEC=$(SYS=$(IOC))-$(EVR=EVR0):RX-MTS-SEC),BSREAD_TS_NSEC=$(BSREAD_TS_NSEC=$(SYS=$(IOC))-$(EVR=EVR0):RX-MTS-NSEC)"

bsreadConfigure($(BSREAD_PORT=9999),$(BSREAD_PORT=9999),$(BSREAD_MODE=PUSH),$(HWM=10))
