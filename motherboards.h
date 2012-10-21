#ifndef MOTHERBOARDS_H
#define MOTHERBOARDS_H

#include "bios.h"

const motherboard_t SUPPORTED_MOTHERBOARDS_LIST[] = 
{
    // H61
    {
        "P8H61-M", 
        MODULE_VERSION_6_SERIES,
        ASCII, ASCII_MAC_HEADER_6_SERIES, NULL,
        None, NULL, NULL,
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES,
    },{
        "P8H61-M-LE", 
        MODULE_VERSION_6_SERIES,
        ASCII, ASCII_MAC_HEADER_6_SERIES, NULL,
        None, NULL, NULL,
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES,
    },{
        "P8H61-M-LX", 
        MODULE_VERSION_6_SERIES,
        ASCII, ASCII_MAC_HEADER_6_SERIES, NULL,
        None, NULL, NULL,
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES,
    },


    // P67
    {
        "MaximusIV-Extreme",
        MODULE_VERSION_6_SERIES,
        GbE, NULL, NULL,
        None, NULL, NULL,
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES,
    },{
        "P8P67", 
        MODULE_VERSION_6_SERIES,
        UUID, NULL, NULL,
        Short, DTS_SHORT_HEADER_6_SERIES, NULL,
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES,
    },{
        "P8P67-REV31", 
        MODULE_VERSION_6_SERIES, 
        UUID, NULL, NULL,
        Short, DTS_SHORT_HEADER_6_SERIES, NULL,
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES,
    },{
        "P8P67-DELUXE", 
        MODULE_VERSION_6_SERIES,
        GbE, NULL, NULL,
        Long, DTS_LONG_HEADER_6_SERIES, DTS_LONG_MAGIC_V1,
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES,
    },{
        "P8P67-EVO",
        MODULE_VERSION_6_SERIES,
        GbE, NULL, NULL,
        Short, DTS_SHORT_HEADER_6_SERIES, NULL,
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES,
    },{
        "P8P67-LE", 
        MODULE_VERSION_6_SERIES,
        ASCII, ASCII_MAC_HEADER_6_SERIES, NULL,
        Short, DTS_SHORT_HEADER_6_SERIES, NULL,
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES,
    },{
        "P8P67-PRO", 
        MODULE_VERSION_6_SERIES, 
        GbE, NULL, NULL,
        Short, DTS_SHORT_HEADER_6_SERIES, NULL,
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES,
    },{
        "P8P67-PRO-REV31", 
        MODULE_VERSION_6_SERIES, 
        GbE, NULL, NULL,
        Short, DTS_SHORT_HEADER_6_SERIES, NULL,
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES,
    },{
        "P8P67-WS-REVOLUTION", 
        MODULE_VERSION_6_SERIES, 
        GbE, NULL, NULL,
        Long, DTS_LONG_HEADER_6_SERIES, DTS_LONG_MAGIC_V3,
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES,
    },{
        "P8P67-M", 
        MODULE_VERSION_6_SERIES, 
        ASCII, ASCII_MAC_HEADER_6_SERIES, NULL,
        Short, DTS_SHORT_HEADER_6_SERIES, NULL,
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES,
    },{
        "P8P67-M-PRO", 
        MODULE_VERSION_6_SERIES,
        ASCII, ASCII_MAC_HEADER_6_SERIES, NULL,
        Long, DTS_LONG_HEADER_6_SERIES, DTS_LONG_MAGIC_V2,
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES,
    },{
        "SABERTOOTH-P67", 
        MODULE_VERSION_6_SERIES, 
        GbE, NULL, NULL,
        Short, DTS_SHORT_HEADER_6_SERIES, NULL,
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES,
    },
    
    // Z68
    {
        "Maximus-IV-Extreme-Z",
        MODULE_VERSION_6_SERIES, 
        GbE, NULL, NULL,
        None, NULL, NULL,
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES,
    },{
        "MaximusIV-GENE-Z", 
        MODULE_VERSION_6_SERIES, 
        GbE, NULL, NULL,
        None, NULL, NULL,
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES,
    },{
        "MAXIMUS-IV-GENE-Z-GEN3",
        MODULE_VERSION_6_SERIES, 
        GbE, NULL, NULL,
        None, NULL, NULL,
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES,
    },{
        "P8Z68-DELUXE", 
        MODULE_VERSION_6_SERIES, 
        GbE, NULL, NULL,
        Long, DTS_LONG_HEADER_6_SERIES, DTS_LONG_MAGIC_V1,
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES,
    },{
        "P8Z68-DELUXE-GEN3",
        MODULE_VERSION_6_SERIES, 
        GbE, NULL, NULL,
        Long, DTS_LONG_HEADER_6_SERIES, DTS_LONG_MAGIC_V1,
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES,
    },{
        "P8Z68-V", 
        MODULE_VERSION_6_SERIES, 
        GbE, NULL, NULL,
        None, NULL, NULL,
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES,
    },{
        "P8Z68-V-GEN3",
        MODULE_VERSION_6_SERIES, 
        GbE, NULL, NULL,
        None, NULL, NULL,
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES,    
    },
        {"P8Z68-V-LE", 
        MODULE_VERSION_6_SERIES, 
        ASCII, ASCII_MAC_HEADER_6_SERIES, NULL, 
        None, NULL, NULL, 
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES, 
    },{
        "P8Z68-V-LX",
        MODULE_VERSION_6_SERIES, 
        ASCII, ASCII_MAC_HEADER_6_SERIES, NULL, 
        None, NULL, NULL, 
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES, 
    },{
        "P8Z68-V-PRO", 
        MODULE_VERSION_6_SERIES, 
        GbE, NULL, NULL,
        Short, DTS_SHORT_HEADER_6_SERIES, NULL,
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES,
    },{
        "P8Z68-V-PRO-GEN3",
        MODULE_VERSION_6_SERIES, 
        GbE, NULL, NULL,
        Short, DTS_SHORT_HEADER_6_SERIES, NULL,
        UuidPresent, UUID_HEADER_6_SERIES,
        MbsnPresent, MBSN_HEADER_6_SERIES,
    },

    // B75
    {
        "P8B75-M-LE", 
        MODULE_VERSION_7_SERIES, 
        ASCII, ASCII_MAC_HEADER_7_SERIES, '\x25',
        None, NULL, NULL,
        UuidPresent, UUID_HEADER_7_SERIES,
        MbsnPresent, MBSN_HEADER_7_SERIES,
    },{
        "P8B75-M-LX", 
        MODULE_VERSION_7_SERIES, 
        ASCII, ASCII_MAC_HEADER_7_SERIES, '\x24',
        None, NULL, NULL,
        UuidPresent, UUID_HEADER_7_SERIES,
        MbsnPresent, MBSN_HEADER_7_SERIES,
    },

    // H77
    {
        "P8H77-I",
        MODULE_VERSION_7_SERIES, 
        ASCII, ASCII_MAC_HEADER_7_SERIES, '\x22',
        None, NULL, NULL,
        UuidPresent, UUID_HEADER_7_SERIES,
        MbsnPresent, MBSN_HEADER_7_SERIES, 
    },{
        "P8H77-V",
        MODULE_VERSION_7_SERIES, 
        ASCII, ASCII_MAC_HEADER_7_SERIES, '\x2A',
        None, NULL, NULL,
        UuidPresent, UUID_HEADER_7_SERIES,
        MbsnPresent, MBSN_HEADER_7_SERIES, 
    },{
        "P8H77-V-LE",
        MODULE_VERSION_7_SERIES, 
        ASCII, ASCII_MAC_HEADER_7_SERIES, '\x26',
        None, NULL, NULL,
        UuidPresent, UUID_HEADER_7_SERIES,
        MbsnPresent, MBSN_HEADER_7_SERIES, 
    },{
        "P8H77-M",
        MODULE_VERSION_7_SERIES, 
        ASCII, ASCII_MAC_HEADER_7_SERIES, '\x25',
        None, NULL, NULL,
        UuidPresent, UUID_HEADER_7_SERIES,
        MbsnPresent, MBSN_HEADER_7_SERIES, 
    },{
        "P8H77-M-PRO",
        MODULE_VERSION_7_SERIES, 
        ASCII, ASCII_MAC_HEADER_7_SERIES, '\x28',
        None, NULL, NULL,
        UuidPresent, UUID_HEADER_7_SERIES,
        MbsnPresent, MBSN_HEADER_7_SERIES, 
    },

    // Z77
    {
        "MAXIMUS-V-FORMULA", 
        MODULE_VERSION_7_SERIES, 
        GbE, NULL, NULL, 
        None, NULL, NULL, 
        UuidPresent, UUID_HEADER_7_SERIES,
        MbsnPresent, MBSN_HEADER_7_SERIES, 
    },{
        "MAXIMUS-V-EXTREME", 
        MODULE_VERSION_7_SERIES, 
        GbE, NULL, NULL, 
        None, NULL, NULL, 
        UuidPresent, UUID_HEADER_7_SERIES,
        MbsnPresent, MBSN_HEADER_7_SERIES, 
    },{
        "MAXIMUS-V-GENE", 
        MODULE_VERSION_7_SERIES, 
        GbE, NULL, NULL, 
        None, NULL, NULL, 
        UuidPresent, UUID_HEADER_7_SERIES,
        MbsnPresent, MBSN_HEADER_7_SERIES, 
    },{
        "P8Z77-M", 
        MODULE_VERSION_7_SERIES, 
        UUID, NULL, NULL, 
        None, NULL, NULL, 
        UuidPresent, UUID_HEADER_7_SERIES,
        MbsnPresent, MBSN_HEADER_7_SERIES,
    },{
        "P8Z77-M-PRO", 
        MODULE_VERSION_7_SERIES, 
        UUID, NULL, NULL, 
        None, NULL, NULL, 
        UuidPresent, UUID_HEADER_7_SERIES,
        MbsnPresent, MBSN_HEADER_7_SERIES,
    },{
        "P8Z77-V", 
        MODULE_VERSION_7_SERIES,
        GbE, NULL, NULL, 
        None, NULL, NULL, 
        UuidPresent, UUID_HEADER_7_SERIES,
        MbsnPresent, MBSN_HEADER_7_SERIES, 
    },{
        "P8Z77-V-DELUXE", 
        MODULE_VERSION_7_SERIES, 
        GbE, NULL, NULL, 
        None, NULL, NULL, 
        UuidPresent, UUID_HEADER_7_SERIES,
        MbsnPresent, MBSN_HEADER_7_SERIES,
    },{
        "P8Z77-V-LE", 
        MODULE_VERSION_7_SERIES, 
        ASCII, ASCII_MAC_HEADER_7_SERIES, '\x2D', 
        None, NULL, NULL, 
        UuidPresent, UUID_HEADER_7_SERIES,
        MbsnPresent, MBSN_HEADER_7_SERIES, 
    },{
        "P8Z77-V-LX", 
        MODULE_VERSION_7_SERIES, 
        ASCII, ASCII_MAC_HEADER_7_SERIES, '\x2D', 
        None, NULL, NULL, 
        UuidPresent, UUID_HEADER_7_SERIES,
        MbsnPresent, MBSN_HEADER_7_SERIES, 
    },{
        "P8Z77-V-PRO", 
        MODULE_VERSION_7_SERIES, 
        GbE, NULL, NULL, 
        None,  NULL, NULL, 
        UuidPresent, UUID_HEADER_7_SERIES,
        MbsnPresent, MBSN_HEADER_7_SERIES, 
    },{
        "P8Z77-WS", 
        MODULE_VERSION_7_SERIES, 
        UUID, NULL, NULL, 
        Long, DTS_LONG_HEADER_7_SERIES, DTS_LONG_MAGIC_V1, 
        UuidPresent, UUID_HEADER_7_SERIES,
        MbsnPresent, MBSN_HEADER_7_SERIES, 
    },{
        "SABERTOOTH-Z77", 
        MODULE_VERSION_7_SERIES, 
        GbE, NULL, NULL, 
        None,  NULL,  NULL, 
        UuidPresent, UUID_HEADER_7_SERIES,
        MbsnPresent, MBSN_HEADER_7_SERIES, 
    },

    // X79
    {
        "P9X79", 
        MODULE_VERSION_6_SERIES, 
        GbE, NULL, NULL, 
        Long,  DTS_LONG_HEADER_X79,  DTS_LONG_MAGIC_V1, 
        UuidPresent, UUID_HEADER_X79,
        MbsnPresent, MBSN_HEADER_X79, 
    },{
        "SABERTOOTH-X79", 
        MODULE_VERSION_6_SERIES, 
        GbE, NULL, NULL, 
        Long,  DTS_LONG_HEADER_X79,  DTS_LONG_MAGIC_V1, 
        UuidPresent, UUID_HEADER_X79,
        MbsnPresent, MBSN_HEADER_X79, 
    },

    //C602
    {
        "Z9PE-D16",
        MODULE_VERSION_C602,
        UUID, NULL, NULL, 
        None, NULL, NULL,
        UuidPresent, UUID_HEADER_7_SERIES,
        MbsnPresent, MBSN_HEADER_7_SERIES, 
    },
};

const unsigned int SUPPORTED_MOTHERBOARDS_LIST_LENGTH = sizeof(SUPPORTED_MOTHERBOARDS_LIST) / sizeof(SUPPORTED_MOTHERBOARDS_LIST[0]);

#endif // MOTHERBOARDS_H
