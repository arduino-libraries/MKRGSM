/*
  This file is part of the MKR GSM library.
  Copyright (C) 2017  Arduino AG (http://www.arduino.cc/)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _GSM_BAND_H_INCLUDED
#define _GSM_BAND_H_INCLUDED

#include "GSM.h"

#define NUMBEROFBANDS 9
#define GSM_MODE_UNDEFINED "UNDEFINED"
#define GSM_MODE_EGSM "EGSM_MODE"
#define GSM_MODE_DCS "DCS_MODE"
#define GSM_MODE_PCS "PCS_MODE"
#define GSM_MODE_EGSM_DCS "EGSM_DCS_MODE"
#define GSM_MODE_GSM850_PCS "GSM850_PCS_MODE"
#define GSM_MODE_GSM850_EGSM_DCS_PCS "GSM850_EGSM_DCS_PCS_MODE"
#define GSM_MODE_UMTS "UMTS_MODE"
#define GSM_MODE_GSM850_EGSM_PCS_UMTS "GSM850_EGSM_PCS_UMTS_MODE"


enum GSM3GSMBand {UNDEFINED, EGSM_MODE, DCS_MODE, PCS_MODE, EGSM_DCS_MODE, GSM850_PCS_MODE, GSM850_EGSM_DCS_PCS_MODE, GSM_UMTS_MODE, GSM_GSM850_EGSM_PCS_UMTS_MODE};

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

private:

  /** Change the Radio access technology
      @param act   desired access technology
      @return true if success, false otherwise
  */
  bool setRAT(const char* act);
};

#endif
