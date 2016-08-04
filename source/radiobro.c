/**************************************************
Alba Orbital
Module name                     : radiobro.c
Last Modification               : 06/07/2016
Last Update                     :
Author                          : Conor Forde, Rowen Simpson
**************************************************/

#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "kubos-hal/spi.h"

#define MODULATION_SIZE         3
#define TRANSMIT_SIZE           7
#define RECIEVE_SIZE            5
#define TRANSMIT_PACKETS        11
int8_t static const *FREQUENCY = "437176000";
//int8_t static const *CALLSIGN = "ON3ELI";

#define SPI_BUS K_SPI1

void RadioInit(void)
{
    /* init SPI */
    KSPIConf conf = {
        .role = K_SPI_MASTER,
        .direction = K_SPI_DIRECTION_2LINES,
        .data_size = K_SPI_DATASIZE_8BIT,
        .speed = 10000
    };
    k_spi_init(SPI_BUS, &conf);
}

// send a packet via spi
void Send(const int8_t *envelope) {
  P1OUT &= ~0x04;
  k_spi_write(SPI_BUS, envelope, strlen(envelope));
  P1OUT |= 0x04;
}

void PacketBuilder(const bool report, const int8_t *packet,
    const int8_t *properties[], const int8_t *prefixes[], const int16_t size) {

    char *local_packet = (char *)packet;                        // set local variable

    if(!report) {                                               // if no report is required
        strcat(local_packet, ",");
        for(int16_t i=0; i<=size; i++) {                        // loop for inserting info into packet
            strcat(local_packet, (char const*)prefixes[i]);     // prefix packet hopper
            strcat(local_packet, ",");
            strcat(local_packet, (char const*)properties[i]);   // property packet hopper

            if(i == size){
                break;
            }
            strcat(local_packet, ",");
        }
    }

    strcat(local_packet, "\r");               // insert carriage return to end of packet
    Send((int8_t const*)local_packet);
}


void TempVoltageThreshold(const int8_t *field, const uint8_t *value) {

    int8_t packet[12] = "s,s,0,";                               // Standard packet
    packet[4] = *field;                                         // Insert the field to be altered
    strcat((char *)packet, (char const *)value);                // Add the required value to the string
    strcat((char *)packet, "\r");                               // Add the carriage return
    Send(packet);                                               // Send the packet via spi
}

void AX25(const int8_t *port, const int8_t *destination_callsign, int8_t ax25_destination, const int8_t *source_callsign, int8_t ax25_source) {
  int8_t packet[40] = "s,a,0";
  int data;
  packet[4] = *port;
  Send(packet);
  for(int i = 0; i <= 5; i++)
  {
    data = (int)destination_callsign[i] << 1;
    k_spi_write(SPI_BUS, &data, 1);
  }
    k_spi_write(SPI_BUS, &ax25_destination, 1);

  for(int i = 0; i <= 7; i++)
  {
    data = (int)source_callsign[i] << 1;
    k_spi_write(SPI_BUS, &data, 1);
  }
    k_spi_write(SPI_BUS, &ax25_source, 1);
}

void TransmitModulation(const int8_t *port, const bool report,
    const int8_t *symbol_val, const int8_t *dev_val, const int8_t *modulation) {
    int8_t packet[40] = "s,0,m";                                                        // standard packet
    packet[2] = *port;                                                                  // insert port number
    int8_t const *prefixes[4] = {"f","r","d","m"};                                      // packet prefix values
    int8_t const *properties[4] = {FREQUENCY, symbol_val, dev_val, modulation};         // packet values array
    static const int array_size = 3;                                                    // packet builder loop size
    PacketBuilder(report, packet, properties, prefixes, array_size);
}

void TransmitEncoding(const int8_t *port, const bool report,
    const int8_t *transmit_power, const int8_t *nzri, const int8_t *lsfr,
    const int8_t *start_flags, const int8_t *kut, const int8_t *bin_preamble,
    const int8_t *encrypt_enable, const int8_t *encrypt_key) {
    int8_t packet[65] = "s,0,e";                                                                                                // standard packet
    packet[2] = *port;                                                                                                          // insert port number
    int8_t const *prefixes[8] = {"p", "n", "l", "s", "t", "b", "e", "k"};                                                       // packet prefix values
    int8_t const *properties[8] = {transmit_power, nzri, lsfr, start_flags, kut, bin_preamble, encrypt_enable, encrypt_key};    // packet values array
    static const int array_size = 7;                                                                                            // packet builder loop size
    PacketBuilder(report, packet, properties, prefixes, array_size);
}

void EnableTransmitPort(const int8_t *port, const bool report, const bool enabled) {
    int8_t packet[TRANSMIT_PACKETS];                                                                                            // standard packet

    if(report) {                                                                                                                // report statement
        memcpy(packet, "s,0,p\r", 7);
    } else {                                                            // property change statement
        if(enabled) {
            memcpy(packet, "s,0,p,e,e\r", TRANSMIT_PACKETS);
        } else {
            memcpy(packet, "s,0,p,e,d\r", TRANSMIT_PACKETS);
        }
    }
    packet[2] = *port;                          // insert relevent port
    Send(packet);
}

void RecieveModulation(const bool report, const int8_t *symbol_val,
                        const int8_t *dev_val, const int8_t *modulation,
                        const int8_t *bandwidth, const int8_t *if_freq) {
    int8_t packet[50] = "s,r,m";                                                                        // standard packet
    const int8_t *prefixes[6] = {"f", "r", "d", "m", "b", "i"};                                         // packet prefix values
    const int8_t *properties[6] = {FREQUENCY, symbol_val, dev_val, modulation, bandwidth, if_freq};     // packet values array
    PacketBuilder(report, packet, properties, prefixes, RECIEVE_SIZE);
}

void RecieveEncoding(const bool report, const int8_t *lna, const int8_t *nzri, const int8_t *lsfr, const int8_t *packet_len_max)
{
  int8_t packet[30] = "s,r,e";                                                  // standard Packet
  const int8_t *properties[4] = {lna,nzri,lsfr,packet_len_max};                 // collection of arguments
  const int8_t *prefixes[4] = {"a","n","l","p"};                                // Prefix array
  static const int array_size = 3;                                              // array size
  PacketBuilder(report, packet, properties, prefixes, array_size);
}

void ReceivePortEnableDisable(const bool report, const bool enable){
  int8_t packet[11];                                                    //standard packet
  if(report)                                                            //report statement
  {
    memcpy(packet, "s,r,p\r", 7);
  }
  else                                                                  //property change statement
  {
    if(enable)
      {
        memcpy(packet, "s,r,p,e,e\r", 11);
      }
    else
      {
        memcpy(packet, "s,r,p,e,d\r", 11);
      }
  }
 Send((const int8_t *)packet);                                                  // send the Packet via spi
}

void DataShuttle(const int8_t *data){
  int8_t packet[1000] = "$";                                                    // data packet of max size 1000
  strcat((char*)packet,(char const *)data);                                     // add data to packet
  strcat((char*)packet,"\r");
  Send(packet);
}

void KissTNC(const int8_t *port, const int8_t *destination_callsign, int8_t ax25_destination, const int8_t *source_callsign, int8_t ax25_source, const int8_t *data) {
  int8_t packet[1000] = "$";
  int buf;
  buf = 0xC0;
  k_spi_write(SPI_BUS, &buf, 1);
  k_spi_write(SPI_BUS, port, 1);
  for(int i = 0; i <= 5; i++)
  {
    data = (int)destination_callsign[i] << 1;
    k_spi_write(SPI_BUS, &data, 1);
  }
    k_spi_write(SPI_BUS, &ax25_destination, 1);

  for(int i = 0; i <= 7; i++)
  {
    data = (int)source_callsign[i] << 1;
    k_spi_write(SPI_BUS, &data, 1);
  }
    k_spi_write(SPI_BUS, &ax25_source, 1);
    data = 0x03;
    k_spi_write(SPI_BUS, &data, 1);
    data = 0xF0;
    k_spi_write(SPI_BUS, &data, 1);

    strcat((char*)packet,(char const *)data);
    strcat((char*)packet,"\r");
    Send(packet);

    data = 0xC0;
    k_spi_write(SPI_BUS, &data, 1);
}

void OutputChannelControl(const int8_t *output_pin, const int8_t *pin_behaviour){
  int8_t packet[5] = "O";
  packet[1] = *output_pin;
  packet[2] = *pin_behaviour;
  strcat((char*)packet,"\r");
  Send(packet);
}

void RadioPropertiesPacket(){
  Send("p");
}

void Housekeeping(){
  Send("h");
}

void RadioTestTrans(const int8_t *port, const int8_t *duration){
int8_t packet[5] = "w,";
packet[2] = *port;
strcat((char*)packet,",");
strcat((char*)packet,(char *)duration);
strcat((char*)packet,",");
strcat((char*)packet,"w");
Send(packet);
}
