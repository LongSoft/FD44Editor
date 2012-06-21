#ifndef MOTHERBOARDS_H
#define MOTHERBOARDS_H

#include <QByteArray>

enum mac_storage_e {UnknownStorage, GbE, MAC, UUID};
enum dts_e {UnknownDts, None, Short, Long};
static const unsigned int BOOTEFI_MOTHERBOARD_NAME_LENGTH = 60;

typedef struct {
    char name[BOOTEFI_MOTHERBOARD_NAME_LENGTH];
    mac_storage_e mac_storage;
    dts_e dts_type;
} motherboard_t;

static const motherboard_t MB_FEATURE_LIST[] = {
    // P67
    {"MaximusIV-Extreme", GbE, None},
    {"P8P67", UUID, Short},
    {"P8P67-DELUXE", GbE, Long},
    {"P8P67-EVO", GbE, Short},
    //{"P8P67-LE", MAC, None},
    {"P8P67-PRO", GbE, Short},
    {"P8P67-WS-REVOLUTION", GbE, None},
    {"P8P67-M", MAC, Short},
    {"P8P67-M-PRO", MAC, None},
    {"SABERTOOTH-P67", GbE, Short},
    
    // Z68
    {"Maximus-IV-Extreme-Z", GbE, None},
    {"MaximusIV-GENE-Z", GbE, None},
    {"MAXIMUS-IV-GENE-Z-GEN3", GbE, None},
    {"P8Z68-DELUXE", GbE, Long},
    {"P8Z68-DELUXE-GEN3", GbE, Long},
    //{"P8Z68-M-PRO", MAC, None},
    {"P8Z68-V", GbE, None},
    {"P8Z68-V-GEN3", GbE, None},
    {"P8Z68-V-LE", MAC, None},
    {"P8Z68-V-LX", MAC, None},
    {"P8Z68-V-PRO", GbE, Short},
    {"P8Z68-V-PRO-GEN3", GbE, Short},

	// Z77
    {"MAXIMUS-V-FORMULA", GbE, None},
    {"MAXIMUS-V-GENE", GbE, None},
    //{"P8Z77-I-DELUXE", GbE, Long},
    //{"P8Z77-M", MAC, Short},
    {"P8Z77-M-PRO", UUID, None},
    {"P8Z77-V", GbE, None},
    {"P8Z77-V-DELUXE", GbE, None},
    //{"P8Z77-V-LE", MAC, None},
    //{"P8Z77-V-LE-PLUS", MAC, None},
    //{"P8Z77-V-LK", MAC, None},
    //{"P8Z77-V-PREMIUM", GbE, Long},
    {"P8Z77-V-PRO", GbE, None},
    //{"P8Z77-V-PRO-THUNDERBOLT", GbE, Short},
    {"P8Z77-WS", UUID, None},
    //{"SABERTOOTH-P67", GbE, Short},
};

static const unsigned int MB_FEATURE_LIST_LENGTH = sizeof(MB_FEATURE_LIST) / sizeof(MB_FEATURE_LIST[0]);

#endif // MOTHERBOARDS_H
