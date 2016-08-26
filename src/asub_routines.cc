#include <stdio.h>
#include <cstring>
#include <string>
#include <stdexcept>
#include <iostream>

#include <aSubRecord.h>
#include <registryFunction.h>
#include <dbStaticLib.h>
#include <epicsString.h>
#include <errlog.h>
#include <recSup.h>

#include "epics_bsread.h"
#include <stdlib.h>

#include <epicsExport.h>
#include <time.h>


long bsread_configure_init(aSubRecord*);
long bsread_configure(aSubRecord*);
long bsread_read_init(aSubRecord*);
long bsread_read(aSubRecord*);


using namespace std;

// Setting PACT ensures that the record is not processed if init failed.
static long fail_init(aSubRecord *prec)
{
    prec->brsv = 1;
    prec->pact = 1;
    return -1;
}

static long fail_process(aSubRecord *prec)
{
    prec->brsv = 1;
    return -1;
}

long bsread_configure_init(aSubRecord* prec){
    bsread_debug(3,"configure init");

    //Fetch bsread instance
    prec->dpvt = (void*)epicsBSRead::get_instance("default");


    if (prec->fta != DBF_CHAR) {
        errlogPrintf("FTA has invalid type. Must be a CHAR");
        return fail_init(prec);
    }

  return 0;
}


long bsread_configure(aSubRecord* prec){
    bsread_debug(3,"bsread config via asub");
    /* Reading from a string waveform */
    char const *configuration = (char const *) prec->a;
        if (strnlen(configuration, prec->noa) == prec->noa) {
            errlogPrintf("Config is not null terminated!\n");
            return fail_process(prec);
        }
    try{
        ((bsread::BSRead*)(prec->dpvt))->configure(string(configuration));
    }
    catch(runtime_error & e){
        errlogPrintf("Problem parsing BSDAQ configuration: %s\n", e.what());
        return fail_process(prec);
    }

    return 0;
}


long bsread_read_init(aSubRecord* prec){
    bsread_debug(3,"read init");

    //Fetch bsread instance
    prec->dpvt = (void*)epicsBSRead::get_instance("default");

    // INPA = bunch ID
    if (prec->fta != DBF_ULONG) {
        errlogPrintf("FTA must be ULONG.\n");
        return fail_init(prec);
    }
    if (prec->noa != 1) {
        errlogPrintf("INPA must be a scalar.\n");
        return fail_init(prec);
    }
    //INPB is master timestamp seconds
    if (prec->ftb != DBF_ULONG) {
        errlogPrintf("FTB must be ULONG.\n");
        return fail_init(prec);
    }
    if (prec->nob != 1) {
        errlogPrintf("INPB must be a scalar.\n");
        return fail_init(prec);
    }
    //INPC is master timestamp nsec
    if (prec->ftc != DBF_ULONG) {
        errlogPrintf("FTC must be ULONG.\n");
        return fail_init(prec);
    }
    if (prec->noc != 1) {
        errlogPrintf("INPC must be a scalar.\n");
        return fail_init(prec);
    }

    //INPD is pulseID offset
    if (prec->ftd != DBF_DOUBLE) {
        errlogPrintf("FTD must be DOUBLE.\n");
        return fail_init(prec);
    }
    if (prec->nod != 1) {
        errlogPrintf("INPD must be a scalar.\n");
        return fail_init(prec);
    }


    // VALA = snapshot duration
    if (prec->ftva != DBF_DOUBLE) {
        errlogPrintf("FTVA must be DOUBLE.\n");
        return fail_init(prec);
    }
    if (prec->nova != 1) {
        errlogPrintf("NOVA must be 1.\n");
        return fail_init(prec);
    }

    // VALB = number of timeouts
    if (prec->ftvb != DBF_ULONG) {
        errlogPrintf("FTVB must be ULONG\n");
        return fail_init(prec);
    }
    if (prec->novb != 1){
        errlogPrintf("NOVB must be 1\n");
        return fail_init(prec);
    }

    // VALC = number of dropped messages
    if (prec->ftvc != DBF_ULONG) {
        errlogPrintf("FTVC must be ULONG\n");
        return fail_init(prec);
    }
    if (prec->novc != 1){
        errlogPrintf("NOVB must be 1\n");
        return fail_init(prec);
    }

    // VALD = status of inhibit bit
    if (prec->ftvd != DBF_ULONG) {
        errlogPrintf("FTVD must be ULONG\n");
        return fail_init(prec);
    }
    if (prec->novd != 1){
        errlogPrintf("NOVD must be 1\n");
        return fail_init(prec);
    }
    return 0;
}


long bsread_read(aSubRecord* prec){
    bsread_debug(5,"asub bsred read invoked");
    //Extract pulse id
    unsigned long* a = (unsigned long*)(prec->a);
    double* d = (double*)(prec->d);

    unsigned long pulse_id = a[0]+d[0];
    long ret = 0; //Return value

    //Extract timestamp
    bsread::timestamp t;
    t.sec = (((unsigned long*)(prec->b))[0])+ 631152000u; //  convert epics to unix epoch
    t.nsec = ((unsigned long*)(prec->c))[0];


    //Serialization performance measurment
    epicsTimeStamp t0,t1;
    epicsTimeGetCurrent(&t0);


    bsread::BSRead* sender = (bsread::BSRead*)(prec->dpvt);
    sender->send(pulse_id,t);

//    BSRead::get_instance()->read(pulse_id,t);

    epicsTimeGetCurrent(&t1);
    double timeSpan = (t1.secPastEpoch * 1e9 + t1.nsec) - (t0.secPastEpoch * 1e9 + t0.nsec);
    timeSpan/=1e6; //time in ms

    // Put the serialization time into VALA.
    *(double *)prec->vala = timeSpan;
    prec->neva = 1;

    //If it takes more than 2 ms to sample the data than we have an error
    if(timeSpan > 2.0){
        (*(unsigned long*)prec->valb)++; //increase number of "timeouts"
        ret = -1; //Return -1 to put record in alarm state
    }

    //Update the overflow count
    (*(unsigned long*)prec->valc) = sender->zmq_overflows();

    //Update inhibit
    (*(unsigned long*)prec->vald) = sender->get_inhibit();

    return ret;
}


extern "C"{
    epicsRegisterFunction(bsread_configure_init);
    epicsRegisterFunction(bsread_configure);
    epicsRegisterFunction(bsread_read_init);
    epicsRegisterFunction(bsread_read);
}
