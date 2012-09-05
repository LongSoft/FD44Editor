#ifndef BIOS_H
#define BIOS_H

#include <QByteArray>
#include "motherboards.h"

// USB BIOS Flashback file header
static const char UBF_FILE_HEADER[] =							{'\x8B','\xA6','\x3C','\x4A','\x23',
                                                                 '\x77','\xFB','\x48','\x80','\x3D',
                                                                 '\x57','\x8C','\xC1','\xFE','\xC4',
                                                                 '\x4D'};
#define UBF_FILE_HEADER_SIZE 0x800

// BOOTEFI marker
static const char BOOTEFI_HEADER[] =                            {'$','B','O','O','T','E','F','I','$'};
#define BOOTEFI_MAGIC_LENGTH 3
#define BOOTEFI_BIOS_VERSION_LENGTH 2
#define BOOTEFI_BIOS_DATE_OFFSET 21
#define BOOTEFI_BIOS_DATE_LENGTH 10

// ME header
static const char ME_HEADER[] =                                 {'\x20','\x20','\x80','\x0F','\x40',
                                                                 '\x00','\x00','\x10','\x00','\x00',
                                                                 '\x00','\x00','\x00','\x00','\x00',
                                                                 '\x00'};
static const char ME_VERSION_HEADER[] =                         {'\x24','\x4D','\x4E','\x32'};
#define ME_VERSION_OFFSET 4
#define ME_VERSION_LENGTH 8

// GbE header
static const char GBE_HEADER[] =                                {'\xFF','\xFF','\xFF','\xFF','\xFF',
                                                                 '\xFF','\xFF','\xFF','\xC3','\x10'};
#define GBE_MAC_OFFSET (-6)
static const char GBE_MAC_STUB[] =                              {'\x88','\x88','\x88','\x88','\x87',
                                                                 '\x88'};
#define GBE_VERSION_OFFSET (-2)
#define GBE_VERSION_LENGTH 2

// FD44 module structure
static const char MODULE_HEADER[] =                             {'\x0B','\x82','\x44','\xFD','\xAB',
                                                                 '\xF1','\xC0','\x41','\xAE','\x4E',
                                                                 '\x0C','\x55','\x55','\x6E','\xB9',
                                                                 '\xBD'};
static const char MODULE_VERSION_X6X[] =                        {'\xD3'};
static const char MODULE_VERSION_X7X[] =                        {'\xCD'};
#define MODULE_HEADER_BSA_OFFSET 28
static const char MODULE_HEADER_BSA[] =                         {'B', 'S', 'A', '_'};
#define MODULE_HEADER_LENGTH 36

static const char RMAC_HEADER_X6X[] =                           {'\x0B','\x01','\x0D','\x00'};
static const char RMAC_HEADER_X7X[] =                           {'\x0B','\x01','\x00','\x80','\x09',
                                                                 '\x0D','\x00','\x2D','\x00'};
static const char AMAC_HEADER_X6X[] =                           {'\x0B','\x00','\x0D','\x00'};
static const char AMAC_HEADER_X7X[] =                           {'\x0B','\x01','\x00','\x80','\x09',
                                                                 '\x0D','\x00','\x2A','\x00'};
#define MAC_LENGTH 6
#define MAC_ASCII_LENGTH 13

#define DTS_KEY_LENGTH 8
static const char DTS_SHORT_HEADER_X6X[] =                      {'\x8B','\x04','\x26','\x00'};
static const char DTS_SHORT_HEADER_X7X[] =                      {'\x8B','\x04','\xAA','\xBB'};
static const char DTS_SHORT_PART2[] =                           {'\x04','\x04','\x32','\x55','\xF8',
                                                                 '\x00','\xA2','\x02','\xA1','\x00',
                                                                 '\x40','\x63','\x43','\x10','\xFE',
                                                                 '\x81','\x03','\xDF','\x40','\xB2',
                                                                 '\x00','\x20','\x00','\x73','\x3C',
                                                                 '\x10','\x08','\x00','\x00','\x00'};

static const char DTS_LONG_HEADER_X6X[] =                       {'\x8B','\x04','\x4E','\x00'};
static const char DTS_LONG_HEADER_X7X[] =                       {'\x8B','\x04','\x00','\x00', '\x00',
                                                                 '\x4E','\x00','\x00','\x02'};
static const char DTS_LONG_PART2[] =                            {'\x04','\x04','\x32','\x55','\xF8',
                                                                 '\x00','\xA2','\x02','\xA1','\x00',
                                                                 '\x40','\x63','\x43','\x10','\x84',
                                                                 '\x83','\x03','\xDF','\x40','\x80',
                                                                 '\x00','\x20','\x00','\x73','\x3C',
                                                                 '\x10','\x08','\x00','\x60','\x0F'};

static const char DTS_LONG_MAGIC_V1[] =                         {'\x43','\x10','\x15','\x04','\x20',
                                                                 '\x00','\x3C','\x10','\x00','\x00',
                                                                 '\x00','\x43','\x10'};
static const char DTS_LONG_MAGIC_V2[] =                         {'\x06','\x11','\x15','\x04','\x20',
                                                                 '\x00','\x3C','\x10','\x00','\x00',
                                                                 '\x00','\x00','\x00'}; //P67-M PRO
static const char DTS_LONG_MAGIC_V3[] =                         {'\x43','\x10','\x84','\x83','\x20',
                                                                 '\x00','\x3C','\x10','\x00','\x00',
                                                                 '\x00','\x43','\x10'}; //P67 WS

static const char DTS_LONG_PART3[] =                            {'\x00','\x00','\x00','\x00','\x00',
                                                                 '\x00','\x00','\x00','\x00','\x00',
                                                                 '\x00','\x00','\x00'};
static const char DTS_LONG_MASK[] =                             {'\x00','\x00','\x00','\xFF','\xFF',
                                                                 '\x00','\x00','\x00'};
static const char DTS_LONG_PART4[] =                            {'\x04','\x00','\x00','\x23','\x33',
                                                                 '\x00'};

static const char UUID_HEADER_X6X[] =                           {'\x01','\x08','\x10','\x00'};
static const char UUID_HEADER_X7X[] =                           {'\x01','\x08','\x00','\x80','\x09',
                                                                 '\x10','\x00','\x01','\x00'};
#define UUID_LENGTH 16

static const char MBSN_HEADER_X6X[] =                           {'\x02','\x07','\x10','\x00'};
static const char MBSN_HEADER_X7X[] =                           {'\x02','\x07','\x00','\x80','\x09',
                                                                 '\x10','\x00','\x02','\x00'};
#define MBSN_BODY_LENGTH 16

#define MODULE_LENGTH 552

// BIOS data structures
typedef struct {
    QByteArray motherboard_name;
    QByteArray bios_version;
    QByteArray bios_date;
} bootefi_t;

typedef struct {
    QByteArray me_version;
    //QByteArray me_image;
} me_t;

typedef struct {
    QByteArray mac;
    QByteArray gbe_version;
} gbe_t;

enum fd44_version_e
{
    x6x,
    x7x
};

typedef struct {
    fd44_version_e version;
    QByteArray mac;
    QByteArray dts_key;
    QByteArray dts_long_magic;
    QByteArray uuid;
    QByteArray mbsn;
} fd44_t;

enum bios_state_e
{
    ParseError,
    Empty,
    Valid
};

typedef struct {
    bootefi_t be;
    me_t me;
    gbe_t gbe;
    fd44_t fd44;
    dts_e dts_type;
    mac_storage_e mac_storage;
    bios_state_e state;
} bios_t;

#endif // BIOS_H
