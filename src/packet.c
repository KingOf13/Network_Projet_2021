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
    //new->L = 0;
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
  if(len<11) {
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
  pkt_status_code statustr = pkt_set_tr(pkt,TR);
  if(statustr!=PKT_OK) {
      return statustr;
  }

  /* WINDOW */
  uint8_t WINDOW = b1&0b00011111;
  pkt_status_code statuswindow = pkt_set_window(pkt,WINDOW);
  if(statuswindow!=PKT_OK) {
      return statuswindow;
  }


  /**** BYTE LENGTH ****/
  uint8_t secondByte;
  memcpy(&secondByte,&data[1],1);
  int L = secondByte>>7;
  L = 0b00000001&L;

  if(L==0) {
      uint16_t here = secondByte&0b01111111;
      pkt_status_code statuslength = pkt_set_length(pkt,here);
      if(statuslength!=PKT_OK) {
          return statuslength;
      }
  }

  else if(L==1){
      uint16_t here;
      memcpy(&here, &data[1], 2);
      here = ntohs(here);
      here = here&0b0111111111111111;
      pkt_status_code statuslength = pkt_set_length(pkt,here);
      if(statuslength !=PKT_OK) {
          return statuslength;
      }
  }


  /**** BYTE SEQNUM ****/
  uint8_t fourthByte;
  memcpy(&fourthByte, &data[2+L], 1);
  pkt_status_code seqnum_s = pkt_set_seqnum(pkt,fourthByte);
  if(seqnum_s!=PKT_OK) {
    return seqnum_s;
  }


  /**** BYTE TIMESTAMP ****/
  uint32_t fifthByte;
  memcpy(&fifthByte, &data[3+L], 4);
  pkt_status_code timestamp_s = pkt_set_timestamp(pkt,fifthByte);
  if(timestamp_s!=PKT_OK) {
      return timestamp_s;
  }


  /**** BYTE CRC1 ****/
  uint32_t sixthByte;
  memcpy(&sixthByte,&data[7+L],4);
  sixthByte = ntohl(sixthByte);


  /**** CRC32 HEADER VERIFICATION ****/
  uint32_t crc1 = crc32(0L, Z_NULL, 0);
  if(TR==0) {
      crc1 = crc32(crc1,(const Bytef*) &data[0], 7+L);
      if(sixthByte!=crc1) {
          return E_CRC;
      }
  }
  else {
      char *header = (char *)malloc(sizeof(char)*(7+L));
      memcpy(header,&data[0],7+L);
      header[0] = header[0]&0b11011111;
      crc1 = crc32(crc1,(const Bytef*) header, 7+L);
      if(sixthByte!=crc1) {
          return E_CRC;
      }
      free(header);
  }
  pkt_status_code CRC1_s = pkt_set_crc1(pkt,crc1);
  if(CRC1_s!=PKT_OK) {
      return CRC1_s;
  }


  /**** BYTES PAYLOAD ****/
  int payloadLength = pkt->length;
  char *payload = (char *) malloc(sizeof(char)*payloadLength);
  memcpy(payload,&data[11+L],payloadLength);
  pkt_status_code payload_s = pkt_set_payload(pkt,payload,payloadLength);
  free(payload);
  if(payload_s!=PKT_OK) {
      return payload_s;
  }


  /**** CRC32 PAYLOAD VERIFICATION ****/
  uint32_t seventhByte;
  memcpy(&seventhByte,&data[11 + L + pkt->length],4);
  seventhByte = ntohl(seventhByte);

  if(pkt->payload!=NULL && pkt->tr==0) {
      uint32_t crc2 = crc32(0L, Z_NULL, 0);
      crc2 = crc32(crc2,(const Bytef*) &data[11+L], pkt->length);
      if(seventhByte!=crc2) {
          return E_CRC;
      }
      pkt_status_code statuscrc2 = pkt_set_crc2(pkt,crc2);
      if(statuscrc2!=PKT_OK) {
          return statuscrc2;
      }
  }

  return PKT_OK;
}

pkt_status_code pkt_encode(const pkt_t* pkt, char *buf, size_t *len)
{
    /* Your code will be inserted here */
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
    pkt->length;
}

uint32_t pkt_get_timestamp   (const pkt_t* pkt)
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
    return PKT_OK;
}

pkt_status_code pkt_set_seqnum(pkt_t *pkt, const uint8_t seqnum)
{
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
    /* Your code will be inserted here */
}

ssize_t predict_header_length(const pkt_t *pkt)
{
    /* Your code will be inserted here */
}
