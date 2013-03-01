/* motherboards.h

  Copyright (c) 2012, Nikolaj Schlej. All rights reserved.
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

*/

#ifndef MOTHERBOARDS_H
#define MOTHERBOARDS_H

#include "bios.h"

typedef struct {
    char name[BOOTEFI_MOTHERBOARD_NAME_LENGTH];
    mac_e mac_type;
    QByteArray mac_magic;
    dts_e dts_type;
    QByteArray dts_magic;
} motherboard_t;

const motherboard_t SUPPORTED_MOTHERBOARDS_LIST[] = 
{
    // H61
    {
        "P8H61-MX-R2",
        ASCII,
        QByteArray(),
        None,
        QByteArray(),
    },{
        "P8H61-M", 
        ASCII,
        QByteArray(),
        None,
        QByteArray(),
    },{
        "P8H61-M-LE", 
        ASCII,
        QByteArray(),
        None,
        QByteArray(),
    },{
        "P8H61-M-LE-R2", 
        ASCII,
        QByteArray(),
        None,
        QByteArray(),
    },{
        "P8H61-M-LX", 
        ASCII,
        QByteArray(),
        None,
        QByteArray(),
    },

    // H67
    {
        "P8H67",
        ASCII,
        QByteArray(),
        None,
        QByteArray(),
    },{
        "P8H67-I",
        ASCII,
        QByteArray(),
        None,
        QByteArray(),
    },{
        "P8H67-V",
        ASCII,
        QByteArray(),
        None,
        QByteArray(),
    },

    // P67
    {
        "MaximusIV-Extreme",
        GbE,
        QByteArray(),
        None,
        QByteArray(),
    },{
        "P8P67", 
        UUID,
        QByteArray(),
        Short,
        QByteArray(),
    },{
        "P8P67-REV31", 
        UUID,
        QByteArray(),
        Short,
        QByteArray(),
    },{
        "P8P67-DELUXE", 
        GbE,
        QByteArray(),
        Long,
        DTS_LONG_MAGIC_V1,
    },{
        "P8P67-EVO",
        GbE,
        QByteArray(),
        Short,
        QByteArray(),
    },{
        "P8P67-LE", 
        ASCII,
        QByteArray(),
        Short,
        QByteArray(),
    },{
        "P8P67-PRO", 
        GbE,
        QByteArray(),
        Short,
        QByteArray(),
    },{
        "P8P67-PRO-REV31", 
        GbE,
        QByteArray(),
        Short,
        QByteArray(),
    },{
        "P8P67-WS-REVOLUTION", 
        GbE,
        QByteArray(),
        Long,
        DTS_LONG_MAGIC_V3,
    },{
        "P8P67-M", 
        ASCII,
        QByteArray(),
        Short,
        QByteArray(),
    },{
        "P8P67-M-PRO", 
        ASCII,
        QByteArray(),
        Long,
        DTS_LONG_MAGIC_V2,
    },{
        "SABERTOOTH-P67", 
        GbE,
        QByteArray(),
        Short,
        QByteArray(),
    },
    
    // Z68
    {
        "Maximus-IV-Extreme-Z",
        GbE,
        QByteArray(),
        None,
        QByteArray(),
    },{
        "MaximusIV-GENE-Z", 
        GbE,
        QByteArray(),
        None,
        QByteArray(),
    },{
        "MAXIMUS-IV-GENE-Z-GEN3",
        GbE,
        QByteArray(),
        None,
        QByteArray(),
    },{
        "P8Z68-DELUXE", 
        GbE,
        QByteArray(),
        Long,
        DTS_LONG_MAGIC_V1,
    },{
        "P8Z68-DELUXE-GEN3",
        GbE,
        QByteArray(),
        Long,
        DTS_LONG_MAGIC_V1,
    },{
        "P8Z68-V", 
        GbE,
        QByteArray(),
        None,
        QByteArray(),
    },{
        "P8Z68-V-GEN3",
        GbE,
        QByteArray(),
        None,
        QByteArray(),
    },{
        "P8Z68-V-LE",
        ASCII,
        QByteArray(),
        None,
        QByteArray(),
    },{
        "P8Z68-V-LX",
        ASCII,
        QByteArray(),
        None,
        QByteArray(),
    },{
        "P8Z68-V-PRO", 
        GbE,
        QByteArray(),
        Short,
        QByteArray(),
    },{
        "P8Z68-V-PRO-GEN3",
        GbE,
        QByteArray(),
        Short,
        QByteArray(),
    },

    // B75
    {
        "P8B75-M", 
        ASCII,
        QByteArray("\x26", 1),
        None,
        QByteArray(),
    },{
        "P8B75-M-LE", 
        ASCII,
        QByteArray("\x25", 1),
        None,
        QByteArray(),
    },{
        "P8B75-M-LX", 
        ASCII,
        QByteArray("\x24", 1),
        None,
        QByteArray(),
    },{
        "P8B75-M-LX-PLUS", 
        ASCII,
        QByteArray("\x22", 1),
        None,
        QByteArray(),
    },

    // H77
    {
        "P8H77-I",
        ASCII,
        QByteArray("\x22", 1),
        None,
        QByteArray(),
    },{
        "P8H77-V",
        ASCII,
        QByteArray("\x2A", 1),
        None,
        QByteArray(),
    },{
        "P8H77-V-LE",
        ASCII,
        QByteArray("\x26", 1),
        None,
        QByteArray(),
    },{
        "P8H77-M",
        ASCII,
        QByteArray("\x25", 1),
        None,
        QByteArray(),
    },{
        "P8H77-M-PRO",
        ASCII,
        QByteArray("\x28", 1),
        None,
        QByteArray(),
    },

    // Z77
    {
        "MAXIMUS-V-FORMULA", 
        GbE,
        QByteArray(),
        None,
        QByteArray(),
    },{
        "MAXIMUS-V-EXTREME", 
        GbE,
        QByteArray(),
        None,
        QByteArray(),
    },{
        "MAXIMUS-V-GENE", 
        GbE,
        QByteArray(),
        None,
        QByteArray(),
    },{
        "P8Z77-I-DELUXE", 
        GbE,
        QByteArray(),
        None,
        QByteArray(),
    },{
        "P8Z77-M", 
        UUID,
        QByteArray(),
        None,
        QByteArray(),
    },{
        "P8Z77-M-PRO", 
        UUID,
        QByteArray(),
        None,
        QByteArray(),
    },{
        "P8Z77-V", 
        GbE,
        QByteArray(),
        None,
        QByteArray(),
    },{
        "P8Z77-V-DELUXE", 
        GbE,
        QByteArray(),
        None,
        QByteArray(),
    },{
        "P8Z77-V-LE", 
        ASCII,
        QByteArray("\x2D", 1),
        None,
        QByteArray(),
    },{
        "P8Z77-V-LE-PLUS", 
        ASCII,
        QByteArray("\x2D", 1),
        None,
        QByteArray(),
    },{
        "P8Z77-V-LX", 
        ASCII,
        QByteArray("\x2D", 1),
        None,
        QByteArray(),
    },{
        "P8Z77-V-PRO", 
        GbE,
        QByteArray(),
        None,
        QByteArray(),
    },{
        "P8Z77-WS", 
        UUID,
        QByteArray(),
        Long,
        DTS_LONG_MAGIC_V1,
    },{
        "SABERTOOTH-Z77", 
        GbE,
        QByteArray(),
        None,
        QByteArray(),
    },

    // X79
    {
        "P9X79", 
        GbE,
        QByteArray(),
        Long,
        DTS_LONG_MAGIC_V1,
    },{
        "Rampage-IV-Extreme", 
        GbE,
        QByteArray(),
        None,
        QByteArray(),
    },{
        "SABERTOOTH-X79", 
        GbE,
        QByteArray(),
        Long,
        DTS_LONG_MAGIC_V1,
    },

	//C204
    {
        "P8B-M",
        GbE,
        QByteArray(),
        None,
        QByteArray(),
    },{
        "P8B-E-4L",
        GbE,
        QByteArray(),
        None,
        QByteArray(),
    },

    //C602
    {
        "Z9PE-D16",
        UUID,
        QByteArray(),
        None,
        QByteArray(),
    },
};

const unsigned int SUPPORTED_MOTHERBOARDS_LIST_LENGTH = sizeof(SUPPORTED_MOTHERBOARDS_LIST) / sizeof(SUPPORTED_MOTHERBOARDS_LIST[0]);

#endif // MOTHERBOARDS_H
