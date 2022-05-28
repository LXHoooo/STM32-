#ifndef _EDCODE_H
#define _EDCODE_H

#include "stm32f10x.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "speex/speex.h"
#include "stdint.h"
#include "arch.h"

extern __IO int16_t ADCBuff[];
extern __IO int16_t DACBuff[];
extern u8 encode_out[];
extern u8 decode_out[];

#endif
