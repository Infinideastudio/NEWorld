/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2002             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: toplevel libogg include
 last mod: $Id: ogg.h 7188 2004-07-20 07:26:04Z xiphmont $

 ********************************************************************/
#ifndef _OGG_H
#define _OGG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "os_types.h"

typedef struct {
  long endbyte;
  int  endbit;

  unsigned char *buffer;
  unsigned char *ptr;
  long storage;
} oggpack_buffer;

/* ogg_page is used to encapsulate the data in one Ogg bitstream page *****/

typedef struct {
  unsigned char *header;
  long header_len;
  unsigned char *body;
  long body_len;
} ogg_page;

/* ogg_stream_state contains the current encode/decode state of a logical
   Ogg bitstream **********************************************************/

typedef struct {
  unsigned char   *body_data;    /* bytes from packet bodies */
  long    body_storage;          /* storage elements allocated */
  long    body_fill;             /* elements stored; fill mark */
  long    body_returned;         /* elements of fill returned */


  int     *lacing_vals;      /* The values that will go to the segment table */
  ogg_int64_t *granule_vals; /* granulepos values for headers. Not compact
				this way, but it is simple coupled to the
				lacing fifo */
  long    lacing_storage;
  long    lacing_fill;
  long    lacing_packet;
  long    lacing_returned;

  unsigned char    header[282];      /* working space for header encode */
  int              header_fill;

  int     e_o_s;          /* set when we have buffered the last packet in the
                             logical bitstream */
  int     b_o_s;          /* set after we've written the initial page
                             of a logical bitstream */
  long    serialno;
  long    pageno;
  ogg_int64_t  packetno;      /* sequence number for decode; the framing
                             knows where there's a hole in the data,
                             but we need coupling so that the codec
                             (which is in a seperate abstraction
                             layer) also knows about the gap */
  ogg_int64_t   granulepos;

} ogg_stream_state;

/* ogg_packet is used to encapsulate the data and metadata belonging
   to a single raw Ogg/Vorbis packet *************************************/

typedef struct {
  unsigned char *packet;
  long  bytes;
  long  b_o_s;
  long  e_o_s;

  ogg_int64_t  granulepos;
  
  ogg_int64_t  packetno;     /* sequence number for decode; the framing
				knows where there's a hole in the data,
				but we need coupling so that the codec
				(which is in a seperate abstraction
				layer) also knows about the gap */
} ogg_packet;

typedef struct {
  unsigned char *data;
  int storage;
  int fill;
  int returned;

  int unsynced;
  int headerbytes;
  int bodybytes;
} ogg_sync_state;

/* Ogg BITSTREAM PRIMITIVES: bitstream ************************/

void  oggpack_writeinit(oggpack_buffer *b);
void  oggpack_writetrunc(oggpack_buffer *b,long bits);
void  oggpack_writealign(oggpack_buffer *b);
void  oggpack_writecopy(oggpack_buffer *b,void *source,long bits);
 void  oggpack_reset(oggpack_buffer *b);
 void  oggpack_writeclear(oggpack_buffer *b);
 void  oggpack_readinit(oggpack_buffer *b,unsigned char *buf,int bytes);
 void  oggpack_write(oggpack_buffer *b,unsigned long value,int bits);
 long  oggpack_look(oggpack_buffer *b,int bits);
 long  oggpack_look1(oggpack_buffer *b);
 void  oggpack_adv(oggpack_buffer *b,int bits);
 void  oggpack_adv1(oggpack_buffer *b);
 long  oggpack_read(oggpack_buffer *b,int bits);
 long  oggpack_read1(oggpack_buffer *b);
 long  oggpack_bytes(oggpack_buffer *b);
 long  oggpack_bits(oggpack_buffer *b);
 unsigned char *oggpack_get_buffer(oggpack_buffer *b);

 void  oggpackB_writeinit(oggpack_buffer *b);
 void  oggpackB_writetrunc(oggpack_buffer *b,long bits);
 void  oggpackB_writealign(oggpack_buffer *b);
 void  oggpackB_writecopy(oggpack_buffer *b,void *source,long bits);
 void  oggpackB_reset(oggpack_buffer *b);
 void  oggpackB_writeclear(oggpack_buffer *b);
 void  oggpackB_readinit(oggpack_buffer *b,unsigned char *buf,int bytes);
 void  oggpackB_write(oggpack_buffer *b,unsigned long value,int bits);
 long  oggpackB_look(oggpack_buffer *b,int bits);
 long  oggpackB_look1(oggpack_buffer *b);
 void  oggpackB_adv(oggpack_buffer *b,int bits);
 void  oggpackB_adv1(oggpack_buffer *b);
 long  oggpackB_read(oggpack_buffer *b,int bits);
 long  oggpackB_read1(oggpack_buffer *b);
 long  oggpackB_bytes(oggpack_buffer *b);
 long  oggpackB_bits(oggpack_buffer *b);
 unsigned char *oggpackB_get_buffer(oggpack_buffer *b);

/* Ogg BITSTREAM PRIMITIVES: encoding **************************/

 int      ogg_stream_packetin(ogg_stream_state *os, ogg_packet *op);
 int      ogg_stream_pageout(ogg_stream_state *os, ogg_page *og);
 int      ogg_stream_flush(ogg_stream_state *os, ogg_page *og);

/* Ogg BITSTREAM PRIMITIVES: decoding **************************/

 int      ogg_sync_init(ogg_sync_state *oy);
 int      ogg_sync_clear(ogg_sync_state *oy);
 int      ogg_sync_reset(ogg_sync_state *oy);
 int	ogg_sync_destroy(ogg_sync_state *oy);

 char    *ogg_sync_buffer(ogg_sync_state *oy, long size);
 int      ogg_sync_wrote(ogg_sync_state *oy, long bytes);
 long     ogg_sync_pageseek(ogg_sync_state *oy,ogg_page *og);
 int      ogg_sync_pageout(ogg_sync_state *oy, ogg_page *og);
 int      ogg_stream_pagein(ogg_stream_state *os, ogg_page *og);
 int      ogg_stream_packetout(ogg_stream_state *os,ogg_packet *op);
 int      ogg_stream_packetpeek(ogg_stream_state *os,ogg_packet *op);

/* Ogg BITSTREAM PRIMITIVES: general ***************************/

 int      ogg_stream_init(ogg_stream_state *os,int serialno);
 int      ogg_stream_clear(ogg_stream_state *os);
 int      ogg_stream_reset(ogg_stream_state *os);
 int      ogg_stream_reset_serialno(ogg_stream_state *os,int serialno);
 int      ogg_stream_destroy(ogg_stream_state *os);
 int      ogg_stream_eos(ogg_stream_state *os);

 void     ogg_page_checksum_set(ogg_page *og);

 int      ogg_page_version(ogg_page *og);
 int      ogg_page_continued(ogg_page *og);
 int      ogg_page_bos(ogg_page *og);
 int      ogg_page_eos(ogg_page *og);
 ogg_int64_t  ogg_page_granulepos(ogg_page *og);
 int      ogg_page_serialno(ogg_page *og);
 long     ogg_page_pageno(ogg_page *og);
 int      ogg_page_packets(ogg_page *og);

 void     ogg_packet_clear(ogg_packet *op);


#ifdef __cplusplus
}
#endif

#endif  /* _OGG_H */






