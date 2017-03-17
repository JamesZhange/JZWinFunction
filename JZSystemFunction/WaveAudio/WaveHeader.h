#ifndef _JZ_WAVE_AUDIO_HEADER_DEFINE_H_
#define _JZ_WAVE_AUDIO_HEADER_DEFINE_H_



typedef unsigned int		U32;
typedef unsigned short		U16;
typedef unsigned char		U8;

/*********************************/
/*        Wave header            */
/*********************************/

typedef struct
{
	U8		szRiffID[4];  // 'R','I','F','F'
	U32		dwRiffSize;
	U8		szRiffFormat[4]; // 'W','A','V','E'
}RIFF_HEADER;

typedef struct
{
	U16		wFormatTag;
	U16		wChannels;
	U32		dwSamplesPerSec;
	U32		dwAvgBytesPerSec;
	U16		wBlockAlign;
	U16		wBitsPerSample;
	//U16        pack;        //∏Ωº”–≈œ¢
}WAVE_FORMAT;

typedef struct
{
	U8				szFmtID[4]; // 'f','m','t',' '
	U32				dwFmtSize;
	WAVE_FORMAT		wavFormat;
}FMT_BLOCK;

typedef struct
{
	U8			szFactID[4]; // 'f','a','c','t'
	U32			dwFactSize;
}FACT_BLOCK;

typedef struct
{
	U8			szDataID[4]; // 'd','a','t','a'
	U32			dwDataSize;
}DATA_BLOCK;

typedef struct
{
	RIFF_HEADER   riff_header;
	FMT_BLOCK     fmt_block;
	FACT_BLOCK    fact_block;
	DATA_BLOCK    data_block;
}WAVEDEC_HEADER;







#endif // _JZ_WAVE_AUDIO_HEADER_DEFINE_H_
