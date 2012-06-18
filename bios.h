#ifndef BIOS_H
#define BIOS_H

#include <QByteArray>
#include "motherboards.h"

// BIOS markers to detect available features
static const char BOOTEFI_HEADER[] =                {'$','B','O','O','T','E','F','I','$'};
static const char BOOTEFI_MAGIC_LENGTH = 3;
static const char BOOTEFI_BIOS_VERSION_LENGTH = 2;

// GbE header
static const char GBE_HEADER[] =                                {'\x00', '\x08', '\xFF', '\xFF', '\xD4',
                                                                 '\x00', '\xFF', '\xFF', '\xFF', '\xFF',
                                                                 '\xFF', '\xFF', '\xFF', '\xFF', '\xC3',
                                                                 '\x10'};

// FD44 module structure
static const char MODULE_HEADER_PART1[]	=                       {'\x0B','\x82','\x44','\xFD','\xAB',
                                                                 '\xF1','\xC0','\x41','\xAE','\x4E',
                                                                 '\x0C','\x55','\x55','\x6E','\xB9',
                                                                 '\xBD','\xD3'};
static const unsigned int MODULE_HEADER_ME_VERSION_LENGTH = 1;
static const char MODULE_HEADER_PART2[] =                       {'\x02','\x00','\x28','\x02','\x00',
                                                                 '\xF8','\x10','\x02','\x00','\x19',
                                                                 '\x42','\x53','\x41','\x5F','\x00',
                                                                 '\x02','\x00','\x00'};

static const char MAC_HEADER[] =                                {'\x0B','\x01','\x0D','\x00'};
static const unsigned int MAC_LENGTH = 6;
static const unsigned int MAC_ASCII_LENGTH = 2*MAC_LENGTH + 1;


static const unsigned int DTS_KEY_LENGTH = 8;
static const char DTS_SHORT_HEADER[] =                          {'\x8B','\x04','\x26','\x00'};
static const char DTS_SHORT_PART2[] =                           {'\x04','\x04','\x32','\x55','\xF8',
                                                                 '\x00','\xA2','\x02','\xA1','\x00',
                                                                 '\x40','\x63','\x43','\x10','\xFE',
                                                                 '\x81','\x03','\xDF','\x40','\xB2',
                                                                 '\x00','\x20','\x00','\x73','\x3C',
                                                                 '\x10','\x08','\x00','\x00','\x00'};

static const char DTS_LONG_HEADER[] =                           {'\x8B','\x04','\x4E','\x00'};
static const char DTS_LONG_PART2[] =                            {'\x04','\x04','\x32','\x55','\xF8',
                                                                 '\x00','\xA2','\x02','\xA1','\x00',
                                                                 '\x40','\x63','\x43','\x10','\x84',
                                                                 '\x83','\x03','\xDF','\x40','\x80',
                                                                 '\x00','\x20','\x00','\x73','\x3C',
                                                                 '\x10','\x08','\x00','\x60','\x0F',
                                                                 '\x43','\x10','\x15','\x04','\x20',
                                                                 '\x00','\x3C','\x10','\x00','\x00',
                                                                 '\x00','\x43','\x10','\x00','\x00',
                                                                 '\x00','\x00','\x00','\x00','\x00',
                                                                 '\x00','\x00','\x00','\x00','\x00',
                                                                 '\x00'};
static const char DTS_LONG_MASK[] =                             {'\x00','\x00','\x00','\xFF','\xFF',
                                                                '\x00','\x00','\x00'};
static const char DTS_LONG_PART3[] =                            {'\x04','\x00','\x00','\x23','\x33',
                                                                 '\x00'};

static const char UUID_HEADER[] =                               {'\x01','\x08','\x10','\x00'};
static const unsigned int UUID_LENGTH = 16;

static const char MBSN_HEADER[] =                               {'\x02','\x07','\x10','\x00'};
static const char MBSN_BODY_LENGTH = 16;
static const char MBSN_OLD_FORMAT_SIGN[] =		        {'M','T','7'};

static const unsigned int MODULE_LENGTH = 552;

// BIOS data structures
typedef struct {
    QByteArray motherboard_name;
    QByteArray bios_version;
} bootefi_t;

typedef struct {
    lan_e lan;
    QByteArray mac;
    QByteArray mac2;
} gbe_t;


typedef struct {
    QByteArray mac;
    dts_e dts_type;
    QByteArray dts_key;
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
    gbe_t gbe;
    fd44_t fd44;
    bios_state_e state;
} bios_t;

#endif // BIOS_H
