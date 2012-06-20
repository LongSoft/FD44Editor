#ifndef MOTHERBOARDS_H
#define MOTHERBOARDS_H

#include <QByteArray>

enum lan_e {UnknownLan, Realtek, Intel};
enum dts_e {UnknownDts, None, Short, Long};
static const unsigned int BOOTEFI_MOTHERBOARD_NAME_LENGTH = 60;

typedef struct {
    char name[BOOTEFI_MOTHERBOARD_NAME_LENGTH];
    lan_e lan_type;
    dts_e dts_type;
} motherboard_t;

static const motherboard_t MB_FEATURE_LIST[] = {
    // P67
    {"MaximusIV-Extreme", Intel, None},
    {"P8P67", Realtek, Short},
    {"P8P67-DELUXE", Intel, Long},
    {"P8P67-EVO", Intel, Short},
    //{"P8P67-LE", Realtek, None},
    {"P8P67-PRO", Intel, Short},
    {"P8P67-WS-REVOLUTION", Intel, None},
    {"P8P67-M", Realtek, Short},
    //{"P8P67-M-PRO", Realtek, None},
    {"SABERTOOTH-P67", Intel, Short},
    
    // Z68
    {"Maximus-IV-Extreme-Z", Intel, None},
    {"MaximusIV-GENE-Z", Intel, None},
    {"MAXIMUS-IV-GENE-Z-GEN3", Intel, None},
    {"P8Z68-DELUXE", Intel, Long},
    {"P8Z68-DELUXE-GEN3", Intel, Long},
    //{"P8Z68-M-PRO", Realtek, None},
    {"P8Z68-V", Intel, None},
    {"P8Z68-V-GEN3", Intel, None},
    {"P8Z68-V-LE", Realtek, None},
    {"P8Z68-V-LX", Realtek, None},
    {"P8Z68-V-PRO", Intel, Short},
    {"P8Z68-V-PRO-GEN3", Intel, Short},

	// Z77
	//{"MAXIMUS-V-FORMULA", Intel, None},
	//{"MAXIMUS-V-GENE", Intel, None},
	//{"P8Z77-I-DELUXE", Intel, Long},
	//{"P8Z77-M", Realtek, Short},
	//{"P8Z77-M-PRO", Realtek, Long},
	//{"P8Z77-V", Intel, Short},
	//{"P8Z77-V-DELUXE", Intel, Long},
	//{"P8Z77-V-LE", Realtek, None},
	//{"P8Z77-V-LE-PLUS", Realtek, None},
	//{"P8Z77-V-LK", Realtek, None},
	//{"P8Z77-V-PREMIUM", Intel, Long},
	//{"P8Z77-V-PRO", Intel, Short},
	//{"P8Z77-V-PRO-THUNDERBOLT", Intel, Short},
	//{"P8Z77-WS", Intel, None},
	//{"SABERTOOTH-P67", Intel, Short},


};

static const unsigned int MB_FEATURE_LIST_LENGTH = sizeof(MB_FEATURE_LIST) / sizeof(MB_FEATURE_LIST[0]);

#endif // MOTHERBOARDS_H
