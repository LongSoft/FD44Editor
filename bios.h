#ifndef BIOS_H
#define BIOS_H

#include <QByteArray>

// USB BIOS Flashback file header
const QByteArray UBF_FILE_HEADER            ("\x8B\xA6\x3C\x4A\x23\x77\xFB\x48\x80\x3D\x57\x8C\xC1\xFE\xC4\x4D", 16);
#define UBF_FILE_HEADER_SIZE                0x800

// Descriptor region header
const QByteArray DESCRIPTOR_HEADER_COMMON   ("\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x5A\xA5\xF0\x0F", 20);
const QByteArray DESCRIPTOR_HEADER_RARE     ("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x78\xE5\x8C\x8C", 20);

// BOOTEFI marker
const QByteArray BOOTEFI_HEADER             ("$BOOTEFI$", 9);
#define BOOTEFI_MAGIC_LENGTH                3
#define BOOTEFI_BIOS_VERSION_LENGTH         2
#define BOOTEFI_BIOS_DATE_OFFSET            21
#define BOOTEFI_BIOS_DATE_LENGTH            10
#define BOOTEFI_MOTHERBOARD_NAME_LENGTH     60

// ME header
const QByteArray ME_HEADER                  ("\x20\x20\x80\x0F\x40\x00\x00\x10\x00\x00\x00\x00\x00\x00\x00\x00", 16);
const QByteArray ME_VERSION_HEADER          ("\x24\x4D\x4E\x32", 4);
#define ME_VERSION_OFFSET                   4
#define ME_VERSION_LENGTH                   8

// GbE header
const QByteArray GBE_HEADER                 ("\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xC3\x10", 10);
#define GBE_MAC_OFFSET                      (-6)
const QByteArray GBE_MAC_STUB               ("\x88\x88\x88\x88\x87\x88", 6);
#define GBE_VERSION_OFFSET                  (-2)
#define GBE_VERSION_LENGTH                  2

// FD44 module
const QByteArray MODULE_HEADER              ("\x0B\x82\x44\xFD\xAB\xF1\xC0\x41\xAE\x4E\x0C\x55\x55\x6E\xB9\xBD", 16);
const QByteArray MODULE_VERSION_6_SERIES    ("\xD3", 1);
const QByteArray MODULE_VERSION_C602        ("\xD1", 1);
const QByteArray MODULE_VERSION_7_SERIES    ("\xCD", 1);
#define MODULE_HEADER_BSA_OFFSET            28
const QByteArray MODULE_HEADER_BSA          ("BSA_", 4);
#define MODULE_HEADER_LENGTH                36
#define MODULE_LENGTH_OFFSET                20

// ASCII MAC
const QByteArray ASCII_MAC_HEADER_6_SERIES  ("\x0B\x01\x0D\x00", 4);
const QByteArray ASCII_MAC_HEADER_7_SERIES  ("\x0B\x01\x00\x80\x09\x0D\x00", 7);
#define ASCII_MAC_OFFSET                    2
#define MAC_LENGTH                          6
#define ASCII_MAC_LENGTH                    13

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
typedef struct {
    QByteArray motherboard_name;
    QByteArray bios_version;
    QByteArray bios_date;
} bootefi_t;

typedef struct {
    QByteArray me_version;
} me_t;

typedef struct {
    QByteArray mac;
    QByteArray gbe_version;
} gbe_t;

typedef struct {
    QByteArray mac;
    char mac_magic;
    QByteArray dts_key;
    QByteArray dts_magic;
    QByteArray uuid;
    QByteArray mbsn;
} module_t;

enum data_state_e {ParseError, Empty, Valid};
enum mac_e {GbE, ASCII, UUID};
enum dts_e {None, Short, Long};

typedef struct {
    bootefi_t be;
    me_t me;
    gbe_t gbe;
    module_t module;
    data_state_e state;
} data_t;

enum uuid_e {UuidPresent, UuidAbsent};
enum mbsn_e {MbsnPresent, MbsnAbsent};

typedef struct {
    char name[BOOTEFI_MOTHERBOARD_NAME_LENGTH];
    QByteArray module_version;
    mac_e mac_type;
    QByteArray mac_header;
    char mac_magic;
    dts_e dts_type;
    QByteArray dts_header;
    QByteArray dts_magic;
    uuid_e uuid_status;
    QByteArray uuid_header;
    mbsn_e mbsn_status;
    QByteArray mbsn_header;
} motherboard_t;

typedef struct {
    motherboard_t mb;
    data_t data; 
} bios_t;

#endif // BIOS_H
