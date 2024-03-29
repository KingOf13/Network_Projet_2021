#include "packet_interface.h"

/* Extra #includes */
/* Your code will be inserted here */

/* Extra code */
/* Your code will be inserted here */

pkt_t* pkt_new()
{
    pkt_t *new = (pkt_t *) malloc(sizeof(pkt_t));
    if(new==NULL) return NULL;

    new->type = 0;
    new->tr = 0;
    new->window = 0;
    new->length = 0;
    new->seqnum = 0;
    new->timestamp = 0;
    new->crc1 = 0;
    new->payload = NULL;
    return new;
}

void pkt_del(pkt_t *pkt)
{
  if(pkt!=NULL) {
      if(pkt_get_payload(pkt)!=NULL) {
          free(pkt->payload);
      }
      free(pkt);
      pkt = NULL;
  }
}

pkt_status_code pkt_decode(const char *data, const size_t len, pkt_t *pkt)
{

  if(len==0 || data==NULL) {
      return E_UNCONSISTENT;
  }

  if(len<7) {
      return E_NOHEADER;
  }
  if(len<10) {
      return E_UNCONSISTENT;
  }
    if (pkt_get_type(pkt) == PTYPE_DATA && pkt_get_tr(pkt) == 1) {
        return E_UNCONSISTENT;
    }
  /**** PREMIER BYTE ****/
  uint8_t firstByte;
  memcpy(&firstByte,&data[0],1);


  /* TYPE */
  uint8_t TYPE = firstByte&0b11000000;
  TYPE = TYPE>>6;
  pkt_status_code statustype = pkt_set_type(pkt,TYPE);
  if(statustype!=PKT_OK) {

      return statustype;
  }


  /* TR */
  uint8_t TR = firstByte&0b00100000;
  TR = TR>>5;
  pkt_status_code tr_s = pkt_set_tr(pkt,TR);
  if(tr_s!=PKT_OK) {
      return tr_s;
  }
  /* WINDOW */

  uint8_t WINDOW = firstByte&0b00011111;
  pkt_status_code window_s = pkt_set_window(pkt,WINDOW);
  if(window_s!=PKT_OK) {
      return window_s;
  }


  int offset = 0;
  /**** LENGTH ****/
  if(pkt_get_type(pkt) == PTYPE_DATA){
      offset = 2;
      uint16_t length;
      memcpy(&length, &data[1], 2);
      length = ntohs(length);
      pkt_status_code length_s = pkt_set_length(pkt,length);
      if(length_s !=PKT_OK) {
          return length_s;
      }
  }





  /*** SEQNUM ***/
  uint8_t seqnum;
  memcpy(&seqnum, &data[1+offset], 1);
  pkt_status_code seqnum_s = pkt_set_seqnum(pkt,seqnum);
  if(seqnum_s!=PKT_OK) {
    return seqnum_s;
  }
  /**** TIMESTAMP ****/
  uint32_t timestamp;
  memcpy(&timestamp, &data[2+offset], 4);
  pkt_status_code timestamp_s = pkt_set_timestamp(pkt,timestamp);
  if(timestamp_s!=PKT_OK) {
      return timestamp_s;
  }

  /**** BYTE CRC1 ****/
  uint32_t CRC1;
  memcpy(&CRC1,&data[6+offset],4);
  CRC1 = ntohl(CRC1);
  // CRC STILL NOT WORKING
  //CRC1 = 0;
    //printf("first %d\n", CRC1);
  /**** CRC32 HEADER VERIFICATION ****/
  uint32_t crc1 = crc32(0L, Z_NULL, 0);
  if(TR==0) {
      crc1 = crc32(crc1,(const Bytef*) &data[0], 6+offset);
      //printf("second %d, %d\n", CRC1, crc1);
      if(CRC1!=crc1) {

          return E_CRC;
      }
  }
  else {
      char *header = (char *)malloc(sizeof(char)*(6+offset));
      memcpy(header,&data[0],6+offset);
      header[0] = header[0]&0b11011111;
      crc1 = crc32(crc1,(const Bytef*) header, 6+offset);
      //printf("second %d, %d\n", CRC1, crc1);
      free(header);
      if(CRC1!=crc1) {
          return E_CRC;
      }
      
  }
  pkt_status_code CRC1_s = pkt_set_crc1(pkt,crc1);
  if(CRC1_s!=PKT_OK) {
      return CRC1_s;
  }


  /**** BYTES PAYLOAD ****/
  int payloadLength = pkt_get_length(pkt);
  //printf("size %d\n", payloadLength);
  char *payload = (char *) malloc(sizeof(char)*payloadLength);
  memcpy(payload,&data[10+offset], payloadLength);
  //printf("payload: %s\n", payload);
  pkt_status_code payload_s = pkt_set_payload(pkt,payload,payloadLength);
  free(payload);
  if(payload_s!=PKT_OK) {
      return payload_s;
  }



  /**** CRC32 PAYLOAD VERIFICATION ****/
  uint32_t CRC2;
  memcpy(&CRC2,&data[10 + offset + pkt->length],4);
  CRC2 = ntohl(CRC2);
  if(pkt_get_length(pkt) != 0 && pkt->tr==0) {
      uint32_t crc2 = crc32(0L, Z_NULL, 0);
      crc2 = crc32(crc2,(const Bytef*) &data[10+offset], pkt->length);
      if(CRC2!=crc2) {
          return E_CRC;
      }
      pkt_status_code CRC2_s = pkt_set_crc2(pkt,crc2);
      if(CRC2_s!=PKT_OK) {
          return CRC2_s;
      }
  }
    /*fprintf(stderr, "DECODE\n");
    fprintf(stderr, "len %d\n", pkt_get_length(pkt));
    fprintf(stderr, "type %d\n", pkt_get_type(pkt));
    fprintf(stderr, "tr %d\n", pkt_get_tr(pkt));
    fprintf(stderr, "timestamp %d\n", pkt_get_timestamp(pkt));
    fprintf(stderr, "seqnum %d\n", pkt_get_seqnum(pkt));
    fprintf(stderr, "payload %s\n", pkt_get_payload(pkt));
    fprintf(stderr, "crc1 %d\n", pkt_get_crc1(pkt));
    fprintf(stderr, "crc2 %d\n", pkt_get_crc2(pkt));
    fprintf(stderr, "HEY\n");*/

  return PKT_OK;
}



pkt_status_code pkt_encode(const pkt_t* pkt, char *buf, size_t* len)
{
  uint16_t payload_length = pkt_get_length(pkt);
  uint16_t header_length = predict_header_length(pkt);
  if (*len < ((size_t) header_length + (size_t) 8 + (size_t) payload_length)) return E_NOMEM;

  int place = 0;

  uint8_t type = (uint8_t) pkt_get_type(pkt) << 6;
  uint8_t TR = pkt_get_tr(pkt) << 5;
  uint8_t window = pkt_get_window(pkt);
  if(pkt_get_tr(pkt) != 0){return E_TR;}
  buf[place] = type + TR + window;
  place ++;

  if (pkt_get_type(pkt) == PTYPE_DATA) {
    uint16_t length = htons(payload_length);
    memcpy(&buf[place], &length, 2);
    place += 2;
  }

  buf[place] = (char) pkt_get_seqnum(pkt);
  place ++;

  uint32_t tmstamp = pkt_get_timestamp(pkt);
  memcpy(&buf[place], &tmstamp, 4);
  place += 4;

  uLong CRC1;
  CRC1 = crc32(0L, Z_NULL, 0);
  CRC1 = crc32(CRC1, (const unsigned char *) buf, header_length-4);
  CRC1 = htonl(CRC1);
  memcpy(&buf[place], &CRC1, 4);
  place += 4;

  uLong  CRC2;

  if (pkt_get_type(pkt) == PTYPE_DATA && pkt_get_payload(pkt) != NULL) {
    memcpy(&buf[place], pkt_get_payload(pkt), payload_length);
    CRC2 = crc32(0L, Z_NULL, 0);
    CRC2 = crc32(CRC2, (const unsigned char *) &buf[place], payload_length);
    CRC2 = htonl(CRC2);
    place += payload_length;
    memcpy(&buf[place], &CRC2, 4);
    place +=4;
  }

  *len = place;

    /*fprintf(stderr, "ENCODE\n");
    fprintf(stderr, "len %d\n", pkt_get_length(pkt));
    fprintf(stderr, "type %d\n", pkt_get_type(pkt));
    fprintf(stderr, "tr %d\n", pkt_get_tr(pkt));
    fprintf(stderr, "timestamp %d\n", pkt_get_timestamp(pkt));
    fprintf(stderr, "seqnum %d\n", pkt_get_seqnum(pkt));
    fprintf(stderr, "payload %s\n", pkt_get_payload(pkt));
    fprintf(stderr, "crc1 %d\n", pkt_get_crc1(pkt));
    fprintf(stderr, "crc2 %d\n", pkt_get_crc2(pkt));
    fprintf(stderr, "HEY\n");*/
  return PKT_OK;
}



ptypes_t pkt_get_type  (const pkt_t* pkt)
{
    return pkt->type;
}

uint8_t  pkt_get_tr(const pkt_t* pkt)
{
    return pkt->tr;
}

uint8_t  pkt_get_window(const pkt_t* pkt)
{
    return pkt->window;
}

uint8_t  pkt_get_seqnum(const pkt_t* pkt)
{
    return pkt->seqnum;
}

uint16_t pkt_get_length(const pkt_t* pkt)
{
   return pkt->length;
}

uint32_t pkt_get_timestamp(const pkt_t* pkt)
{
    return pkt->timestamp;
}

uint32_t pkt_get_crc1   (const pkt_t* pkt)
{
    return pkt->crc1;
}

uint32_t pkt_get_crc2   (const pkt_t* pkt)
{
    return pkt->crc2;
}

const char* pkt_get_payload(const pkt_t* pkt)
{
    return pkt->payload;
}


pkt_status_code pkt_set_type(pkt_t *pkt, const ptypes_t type)
{
    if(type != PTYPE_ACK && type != PTYPE_NACK && type != PTYPE_DATA){return E_TYPE;}
    pkt->type = type;
    return PKT_OK;
}

pkt_status_code pkt_set_tr(pkt_t *pkt, const uint8_t tr)
{
    pkt->tr = tr;
    return PKT_OK;
}

pkt_status_code pkt_set_window(pkt_t *pkt, const uint8_t window)
{
    if(window > MAX_WINDOW_SIZE){return E_WINDOW;}
    pkt->window = window;
    //printf("receive win: %d, %d\n", window, pkt->window);
    return PKT_OK;
}

pkt_status_code pkt_set_seqnum(pkt_t *pkt, const uint8_t seqnum)
{
    //if(seqnum > 255){return E_SEQNUM;}
    pkt->seqnum = seqnum;
    return PKT_OK;
}

pkt_status_code pkt_set_length(pkt_t *pkt, const uint16_t length)
{
    if (length > MAX_PAYLOAD_SIZE)
        return E_LENGTH;

    pkt->length = length;
    return PKT_OK;
}

pkt_status_code pkt_set_timestamp(pkt_t *pkt, const uint32_t timestamp)
{
    pkt->timestamp = timestamp;
    return PKT_OK;
}

pkt_status_code pkt_set_crc1(pkt_t *pkt, const uint32_t crc1)
{
    pkt->crc1 = crc1;
    return PKT_OK;
}

pkt_status_code pkt_set_crc2(pkt_t *pkt, const uint32_t crc2)
{
    pkt->crc2 = crc2;
    return PKT_OK;
}

pkt_status_code pkt_set_payload(pkt_t *pkt,
                                const char *data,
                                const uint16_t length)
{
    if(pkt == NULL) {
        return E_UNCONSISTENT;
    }
    if(length >512){
        return E_LENGTH;
    }

    pkt_status_code statuslength = pkt_set_length(pkt, length);
    if(statuslength!=PKT_OK) {
        return statuslength;
    }

    pkt->payload = (char*) malloc(length);
    if(pkt->payload==NULL) {
        free(pkt->payload);
        return E_NOMEM;
    }
	memcpy(pkt->payload, data, length);

    return PKT_OK;
}

ssize_t predict_header_length(const pkt_t *pkt)
{
    if (pkt_get_type(pkt) == PTYPE_DATA) {
        if (pkt_get_length(pkt) > 512 ) { //|| pkt_get_length(pkt) < 0
            return -1;
        }
        else {
            return 12;
        }
    }
    else {
	    if (pkt_get_length(pkt) > 0) {
            return -1;
        }
        else {
            return 10;
        }
    }
}
