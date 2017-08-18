#ifndef _GSM_BAND_H_INCLUDED
#define _GSM_BAND_H_INCLUDED

#include "GSM.h"

#define NUMBEROFBANDS 7
#define GSM_MODE_UNDEFINED "UNDEFINED"
#define GSM_MODE_EGSM "EGSM_MODE"
#define GSM_MODE_DCS "DCS_MODE"
#define GSM_MODE_PCS "PCS_MODE"
#define GSM_MODE_EGSM_DCS "EGSM_DCS_MODE"
#define GSM_MODE_GSM850_PCS "GSM850_PCS_MODE"
#define GSM_MODE_GSM850_EGSM_DCS_PCS "GSM850_EGSM_DCS_PCS_MODE"

typedef enum GSM3GSMBand {UNDEFINED, EGSM_MODE, DCS_MODE, PCS_MODE, EGSM_DCS_MODE, GSM850_PCS_MODE, GSM850_EGSM_DCS_PCS_MODE};

class GSMBand {

public:

  /** Constructor
      @param trace    If true, dumps all AT dialogue to Serial
   */
  GSMBand(bool trace = false);

  /** Forces modem hardware restart, so we begin from scratch
      @return always returns IDLE status
   */
  GSM3_NetworkStatus_t begin();

  /** Get current modem work band 
      @return current modem work band
   */   
  String getBand();

  /** Changes the modem operating band 
      @param band     Desired new band
      @return true if success, false otherwise
   */     
  bool setBand(String band);
};

#endif
