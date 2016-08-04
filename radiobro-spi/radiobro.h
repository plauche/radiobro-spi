/**************************************************
Alba Orbital
Module                          : radiobro.h
Last Modification               : 06/07/2016
Last Update                     :
Author                          : Conor Forde, Rowen Simpson
**************************************************/

#ifndef _RADIOBRO_H_
#define _RADIOBRO_H_

#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

extern void RadioInit(void);

extern void TempVoltageThreshold(const int8_t *field, const uint8_t *value);

extern void AX25(const int8_t *port, const int8_t *destination_callsign,
                              int8_t ax25_destination, const int8_t *source_callsign, int8_t ax25_source);

extern void TransmitModulation(const int8_t *port, const bool report,
                              const int8_t *symbol_val, const int8_t *dev_val, const int8_t *modulation);

extern void TransmitEncoding(const int8_t *port, const bool report,
                              const int8_t *transmit_power, const int8_t *nzri, const int8_t *lsfr,
                              const int8_t *start_flags, const int8_t *kut, const int8_t *bin_preamble,
                              const int8_t *encrypt_enable, const int8_t *encrypt_key);

extern void EnableTransmitPort(const int8_t *port, const bool report, const bool enabled);

extern void RecieveModulation(const bool report, const int8_t *symbol_val,
                              const int8_t *dev_val, const int8_t *modulation,
                              const int8_t *bandwidth, const int8_t *if_freq);

extern void RecieveEncoding(const bool report, const int8_t *lna, const int8_t *nzri,
                              const int8_t *lsfr, const int8_t *packet_len_max);

extern void ReceivePortEnableDisable(const bool report, const bool enable);

extern void DataShuttle(const int8_t *data);

extern void KissTNC(const int8_t *port, const int8_t *destination_callsign,
                    int8_t ax25_destination, const int8_t *source_callsign, int8_t ax25_source, const int8_t *data);

extern void OutputChannelControl(const int8_t *output_pin, const int8_t *pin_behaviour);

extern void RadioPropertiesPacket();

extern void Housekeeping();

extern void RadioTestTrans(const int8_t *port, const int8_t *duration);


#endif /* _RADIOBRO_H_ */
