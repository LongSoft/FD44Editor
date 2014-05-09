/* bios.h

  Copyright (c) 2012, Nikolaj Schlej. All rights reserved.
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

*/

#ifndef BIOS_H
#define BIOS_H

#include <stdint.h>
#include <QByteArray>

// Capsule header
typedef struct _EFI_CAPSULE_HEADER {
    uint8_t   CapsuleGuid[16];
    uint32_t  HeaderSize;
    uint32_t  Flags;
    uint32_t  CapsuleImageSize;
} EFI_CAPSULE_HEADER;

// AMI Aptio extended capsule header
typedef struct _APTIO_CAPSULE_HEADER {
    EFI_CAPSULE_HEADER    CapsuleHeader;
    uint16_t              RomImageOffset;	// offset in bytes from the beginning of the capsule header to the start of
                                            // the capsule volume

} APTIO_CAPSULE_HEADER;

// AMI Aptio extended capsule GUID
const QByteArray APTIO_CAPSULE_GUID
("\x8B\xA6\x3C\x4A\x23\x77\xFB\x48\x80\x3D\x57\x8C\xC1\xFE\xC4\x4D", 16);

// BOOTEFI marker
const QByteArray BOOTEFI_HEADER             ("$BOOTEFI$", 9);
#define BOOTEFI_MAGIC_LENGTH                3
#define BOOTEFI_BIOS_VERSION_LENGTH         2
#define BOOTEFI_BIOS_DATE_OFFSET            21
#define BOOTEFI_BIOS_DATE_LENGTH            10
#define BOOTEFI_MOTHERBOARD_NAME_LENGTH     60
#define BOOTEFI_RECOVERY_NAME_LENGTH        12
#define BOOTEFI_RECOVERY_NAME_OFFSET        40

// ME header
const QByteArray ME_HEADER		            ("\x00\x00\x00\x00\x24\x46\x50\x54", 8);
const QByteArray ME_3M_SIGN					("\x4F\x50\x52\x31\xFF\xFF\xFF\xFF", 8);
const QByteArray ME_5M_SIGN					("\x42\x49\x45\x4C\xFF\xFF\xFF\xFF", 8);
const QByteArray ME_VERSION_HEADER          ("\x24\x4D\x4E\x32", 4);
#define ME_VERSION_OFFSET                   4
#define ME_VERSION_LENGTH                   8

// GbE header
const QByteArray GBE_HEADER                 ("\xFF\xFF\xFF\xFF\xC3\x10", 6);
#define GBE_MAC_OFFSET                      (-10)
const QByteArray GBE_MAC_STUB               ("\x88\x88\x88\x88\x87\x88", 6);
#define GBE_VERSION_OFFSET                  (-6)
#define GBE_VERSION_LENGTH                  2

// FD44 module
const QByteArray MODULE_HEADER              ("\x0B\x82\x44\xFD\xAB\xF1\xC0\x41\xAE\x4E\x0C\x55\x55\x6E\xB9\xBD", 16);
#define MODULE_VERSION_OFFSET               25
const QByteArray MODULE_VERSIONS            ("\x02\x04\x08\x10", 4);
#define MODULE_VERSION_LENGTH               1
#define MODULE_HEADER_BSA_OFFSET            28
const QByteArray MODULE_HEADER_BSA          ("BSA_", 4);
#define MODULE_HEADER_LENGTH                36
#define MODULE_LENGTH_OFFSET                20

// ASCII MAC
#define MAC_LENGTH                          6
const QByteArray ASCII_MAC_HEADER_6_SERIES  ("\x0B\x01\x0D\x00", 4);
const QByteArray ASCII_MAC_HEADER_7_SERIES  ("\x0B\x01\x00\x80\x09\x0D\x00", 7);
#define ASCII_MAC_OFFSET                    2
#define ASCII_MAC_LENGTH                    13
#define ASCII_MAC_MAGIC_LENGTH              1

// DTS key
#define DTS_KEY_LENGTH 8
const QByteArray DTS_SHORT_HEADER_6_SERIES  ("\x8B\x04\x26\x00", 4);
const QByteArray DTS_SHORT_PART2            ("\x04\x04\x32\x55\xF8\x00\xA2\x02\xA1\x00\x40\x63\x43\x10\xFE\x81\x03\xDF\x40\xB2\x00\x20\x00\x73\x3C\x10\x08\x00\x00\x00", 30);

const QByteArray DTS_LONG_HEADER_6_SERIES   ("\x8B\x04\x4E\x00", 4);
const QByteArray DTS_LONG_HEADER_7_SERIES   ("\x8B\x04\x00\x00\x00\x4E\x00\x00\x02", 9);
const QByteArray DTS_LONG_HEADER_X79        ("\x8B\x00\x00\x04\x4E\x00\x00", 7);

const QByteArray DTS_LONG_PART2             ("\x04\x04\x32\x55\xF8\x00\xA2\x02\xA1\x00\x40\x63\x43\x10\x84\x83\x03\xDF\x40\x80\x00\x20\x00\x73\x3C\x10\x08\x00\x60\x0F", 30);

const QByteArray DTS_LONG_MAGIC_V1          ("\x43\x10\x15\x04\x20\x00\x3C\x10\x00\x00\x00\x43\x10", 13);
const QByteArray DTS_LONG_MAGIC_V2          ("\x06\x11\x15\x04\x20\x00\x3C\x10\x00\x00\x00\x00\x00", 13); //P67-M PRO
const QByteArray DTS_LONG_MAGIC_V3          ("\x43\x10\x84\x83\x20\x00\x3C\x10\x00\x00\x00\x43\x10", 13); //P67 WS
#define DTS_LONG_MAGIC_LENGTH               13

const QByteArray DTS_LONG_PART3             ("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 13);
const QByteArray DTS_LONG_MASK              ("\x00\x00\x00\xFF\xFF\x00\x00\x00", 8);
const QByteArray DTS_LONG_PART4             ("\x04\x00\x00\x23\x33\x00", 6);

// System UUID
const QByteArray UUID_HEADER_6_SERIES       ("\x01\x08\x10\x00", 4);
const QByteArray UUID_HEADER_7_SERIES       ("\x01\x08\x00\x80\x09\x10\x00\x01\x00", 9);
const QByteArray UUID_HEADER_X79            ("\x01\x00\x00\x08\x10\x00\x00", 7);
#define UUID_LENGTH                         16

// Motherboard S/N
const QByteArray MBSN_HEADER_6_SERIES       ("\x02\x07\x10\x00", 4);
const QByteArray MBSN_HEADER_7_SERIES       ("\x02\x07\x00\x80\x09\x10\x00\x02\x00", 9);
const QByteArray MBSN_HEADER_X79            ("\x02\x00\x00\x07\x10\x00\x00", 7);
#define MBSN_BODY_LENGTH                    16

// BIOS data structures
enum bios_state_e {ParseError, Empty, Valid, HasNotDetectedValues};
enum mac_e {UUID, ASCII, GbE, MacNotDetected};
enum dts_e {None, Short, Long, DtsNotDetected};
enum me_e {ME_15M, ME_3M, ME_5M};

typedef struct {
// BIOS info
QByteArray motherboard_name;
QByteArray recovery_name;
QByteArray bios_version;
QByteArray bios_date;
me_e me_type;
QByteArray me_version;
QByteArray module_version;
QByteArray gbe_version;
// MAC
mac_e mac_type;
QByteArray mac_header;
QByteArray mac_magic;
QByteArray mac;
// DTS key
dts_e dts_type;
QByteArray dts_magic;
QByteArray dts_short_header;
QByteArray dts_long_header;
QByteArray dts_key;
// UUID
QByteArray uuid_header;
QByteArray uuid;
// MBSN
QByteArray mbsn_header;
QByteArray mbsn;
// BIOS state
bios_state_e state;
} bios_t;

#endif // BIOS_H
