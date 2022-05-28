#include "EDCode.h"

SpeexBits bitsEncode;/*编码结构体*/  
SpeexBits bitsDecode;/*解码结构体*/ 

void *enc_state, *dec_state;/*维持编码解码状态*/
int quality = 4, complexity=1, vbr=0, enh=1;/* SPEEX PARAMETERS, MUST REMAINED UNCHANGED */


/*定义SPEEX初始化函数*/
void Speex_Init(void)
{
  /* Speex encoding initializations */ 
  speex_bits_init(&bitsEncode);
	speex_bits_init(&bitsDecode);
  enc_state = speex_encoder_init(&speex_nb_mode);/*窄带模式*/
  speex_encoder_ctl(enc_state, SPEEX_SET_VBR, &vbr);/*可变速率*/
  speex_encoder_ctl(enc_state, SPEEX_SET_QUALITY,&quality);/*质量参数*/
  speex_encoder_ctl(enc_state, SPEEX_SET_COMPLEXITY, &complexity);/*复杂度值*/
  /* speex decoding intilalization */
  dec_state = speex_decoder_init(&speex_nb_mode);/*窄带模式*/
  speex_decoder_ctl(dec_state, SPEEX_SET_ENH, &enh);/*知觉性增强*/
}
/*编码程序*/
void Encode(void)
{
		speex_bits_reset(&bitsEncode);
		/* Encode the frame */
		speex_encode_int(enc_state, (spx_int16_t*)ADCBuff, &bitsEncode);/*从rebuf取数据编码存到encode*/
		/* Copy the bits to an array of char that can be decoded */
		speex_bits_write(&bitsEncode, (char *)encode_out, 20);/*编好码后存到encode，这个20是与压缩存放有关*/ 
}
/*解码程序*/
void Decode(void)
{
	  speex_bits_reset(&bitsDecode);
		speex_bits_read_from(&bitsDecode, (char *)decode_out, 20);/*用于解码的数据*/
		speex_decode_int(dec_state, &bitsDecode, (spx_int16_t*)DACBuff);/*解码后数据的存放地方*/
}
