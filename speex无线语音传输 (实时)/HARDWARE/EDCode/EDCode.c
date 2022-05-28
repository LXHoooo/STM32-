#include "EDCode.h"

SpeexBits bitsEncode;/*����ṹ��*/  
SpeexBits bitsDecode;/*����ṹ��*/ 

void *enc_state, *dec_state;/*ά�ֱ������״̬*/
int quality = 4, complexity=1, vbr=0, enh=1;/* SPEEX PARAMETERS, MUST REMAINED UNCHANGED */


/*����SPEEX��ʼ������*/
void Speex_Init(void)
{
  /* Speex encoding initializations */ 
  speex_bits_init(&bitsEncode);
	speex_bits_init(&bitsDecode);
  enc_state = speex_encoder_init(&speex_nb_mode);/*խ��ģʽ*/
  speex_encoder_ctl(enc_state, SPEEX_SET_VBR, &vbr);/*�ɱ�����*/
  speex_encoder_ctl(enc_state, SPEEX_SET_QUALITY,&quality);/*��������*/
  speex_encoder_ctl(enc_state, SPEEX_SET_COMPLEXITY, &complexity);/*���Ӷ�ֵ*/
  /* speex decoding intilalization */
  dec_state = speex_decoder_init(&speex_nb_mode);/*խ��ģʽ*/
  speex_decoder_ctl(dec_state, SPEEX_SET_ENH, &enh);/*֪������ǿ*/
}
/*�������*/
void Encode(void)
{
		speex_bits_reset(&bitsEncode);
		/* Encode the frame */
		speex_encode_int(enc_state, (spx_int16_t*)ADCBuff, &bitsEncode);/*��rebufȡ���ݱ���浽encode*/
		/* Copy the bits to an array of char that can be decoded */
		speex_bits_write(&bitsEncode, (char *)encode_out, 20);/*������浽encode�����20����ѹ������й�*/ 
}
/*�������*/
void Decode(void)
{
	  speex_bits_reset(&bitsDecode);
		speex_bits_read_from(&bitsDecode, (char *)decode_out, 20);/*���ڽ��������*/
		speex_decode_int(dec_state, &bitsDecode, (spx_int16_t*)DACBuff);/*��������ݵĴ�ŵط�*/
}
