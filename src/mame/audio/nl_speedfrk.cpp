// license:CC0
// copyright-holders:Aaron Giles,Couriersud

//
// Netlist for Speed Freak
//
// Derived from the schematics and parts list in the Speed Freak
// manual.
//
// Known problems/issues:
//
//    * Compiles but not yet really tested.
//
//    * Schematics have some seriously confusing issues, including
//       many unlabelled parts and pins, and some nonsensical
//       connections. I have attempted to make sense of them the
//       best I could, but needs some careful looking to be sure
//       it's realistic.
//
//    * It's super slow, with the 2MHz clock really bringing things
//       down. Will probably need some clever approaches to get it
//       in the realm of playable.
//

#include "netlist/devices/net_lib.h"
#include "nl_cinemat_common.h"


//
// The final amplifier is documented but not emulated.
//
#define EMULATE_FINAL_AMP	0


static NETLIST_START(SpeedFreak_schematics)

	ANALOG_INPUT(I_V5, 5)
	ANALOG_INPUT(I_VM5, -5)
	ANALOG_INPUT(I_V15, 15)
	ANALOG_INPUT(I_VM15, -15)
	ANALOG_INPUT(I_V25, 25)
	ANALOG_INPUT(I_VM25, -25)

//	RES(R1, 2.7)
//	RES(R2, 2.7)
//	RES(R3, 2.7)
//	RES(R4, 2.7)
//	RES(R5, 150)
//	RES(R6, 150)
//	RES(R7, RES_K(10))
//	RES(R8, RES_K(68))
//	RES(R9, RES_K(2.2))
//	RES(R10, 820)
//	RES(R11, RES_K(47))
	RES(R12, RES_K(1))
	RES(R13, RES_K(2.2))
	RES(R14, RES_K(3.9))
	RES(R15, RES_K(8.7))
	RES(R16, RES_K(22))
	RES(R17, RES_K(6.8))
	RES(R18, RES_K(10))
	RES(R19, 330)
	RES(R20, 330)
	RES(R21, RES_K(3.3))
	RES(R22, RES_K(30))
	RES(R23, RES_K(4.7))
	RES(R24, RES_K(10))
	RES(R25, RES_K(4.7))
	RES(R26, RES_K(4.7))
	RES(R27, RES_K(10))
	RES(R28, RES_K(4.7))
	RES(R29, RES_K(10))
	POT(R30, RES_K(10))
	PARAM(R30.DIAL, 0.500000)
	RES(R31, 150)
	RES(R32, RES_K(2.2))
	RES(R33, RES_K(10))
	RES(R34, RES_K(3.3))	// unknown

//	CAP(C1, CAP_U(50))
//	CAP(C2, CAP_U(50))
//	CAP(C3, CAP_U(4.7))
//	CAP(C4, CAP_U(4.7))
//	CAP(C5, CAP_U(4.7))
//	CAP(C6, CAP_U(0.002))
//	CAP(C7, CAP_U(0.002))
//	CAP(C8, CAP_U(0.01))
	CAP(C9, CAP_U(0.1))
	CAP(C10, CAP_U(0.1))
	CAP(C11, CAP_U(0.02))
	CAP(C12, CAP_U(0.001))
	CAP(C13, CAP_U(0.001))
	CAP(C20, CAP_U(0.1))

//  D_1N914B(CR1)	// OK
//  D_1N914B(CR2)	// OK
    D_1N914B(CR3)	// OK

//	Q_2N6292(Q1)	// NPN
//	Q_2N6107(Q2)	// PNP
//	Q_2N3904(Q3)	// NPN -- unknown type
	Q_2N3904(Q4)	// NPN

	TTL_74LS04_DIP(U2)		// Hex Inverting Gates
	NET_C(U2.7, GND)
	NET_C(U2.14, I_V5)

	TL081_DIP(U3)			// Op. Amp.
	NET_C(U3.4, I_VM5)		// not documented on schematics
	NET_C(U3.7, I_V5)

	TTL_74LS163_DIP(U4)		// Synchronous 4-Bit Counters
	NET_C(U4.8, GND)
	NET_C(U4.16, I_V5)

	TTL_74LS107_DIP(U5)		// DUAL J-K FLIP-FLOPS WITH CLEAR
	NET_C(U5.7, GND)
	NET_C(U5.14, I_V5)

	TTL_74LS08_DIP(U6)		// Quad 2-Input AND Gates
	NET_C(U6.7, GND)
	NET_C(U6.14, I_V5)

	TTL_74LS163_DIP(U7)		// Synchronous 4-Bit Counters
	NET_C(U7.8, GND)
	NET_C(U7.16, I_V5)

	TTL_74LS163_DIP(U8)		// Synchronous 4-Bit Counters
	NET_C(U8.8, GND)
	NET_C(U8.16, I_V5)

	TTL_74LS163_DIP(U9)		// Synchronous 4-Bit Counters
	NET_C(U9.8, GND)
	NET_C(U9.16, I_V5)

//	TTL_7915_DIP(U8)		// -15V Regulator -- not needed
//	TTL_7815_DIP(U9)		// +15V Regulator -- not needed

	TTL_74LS04_DIP(U10)		// Hex Inverting Gates
	NET_C(U10.7, GND)
	NET_C(U10.14, I_V5)

	TTL_74LS08_DIP(U11)		// Quad 2-Input AND Gates
	NET_C(U11.7, GND)
	NET_C(U11.14, I_V5)

	TTL_74LS75_DIP(U12)		// 4-Bit Bistable Latches with Complementary Outputs
	NET_C(U12.12, GND)
	NET_C(U12.5, I_V5)

	TTL_74LS164_DIP(U13)	// 8-bit parallel-out serial shift registers
	NET_C(U13.7, GND)
	NET_C(U13.14, I_V5)

	TTL_74LS164_DIP(U14)	// 8-bit parallel-out serial shift registers
	NET_C(U14.7, GND)
	NET_C(U14.14, I_V5)

	TTL_74LS163_DIP(U15)	// Synchronous 4-Bit Counters
	NET_C(U15.8, GND)
	NET_C(U15.16, I_V5)

	TTL_74LS107_DIP(U17)	// DUAL J-K FLIP-FLOPS WITH CLEAR
	NET_C(U17.7, GND)
	NET_C(U17.14, I_V5)

	TTL_74LS393_DIP(U18)	// Dual 4-Stage Binary Counter
	NET_C(U18.7, GND)
	NET_C(U18.14, I_V5)

	TTL_74LS86_DIP(U19)		// Quad 2-Input XOR Gates
	NET_C(U19.7, GND)
	NET_C(U19.14, I_V5)

	TTL_74LS164_DIP(U20)	// 8-bit parallel-out serial shift registers
	NET_C(U20.7, GND)
	NET_C(U20.14, I_V5)

	LM555_DIP(U22)			// 5-5-5 Timer

	TTL_74LS163_DIP(U23)	// Dual 4-Stage Binary Counter
	NET_C(U23.8, GND)
	NET_C(U23.16, I_V5)

	TTL_74LS164_DIP(U24)	// 8-bit parallel-out serial shift registers
	NET_C(U24.7, GND)
	NET_C(U24.14, I_V5)

	//
	// 78kHz coming from the logic PCB
	//
	CLOCK(J4_2, 78000)
	NET_C(J4_2.GND, GND)
	NET_C(J4_2.VCC, I_V5)

	//
	// Skip the clock generator and just do it directly
	//
#if 1
	CLOCK(C2MHZ, 2000000)
	NET_C(C2MHZ.GND, GND)
	NET_C(C2MHZ.VCC, I_V5)
	NET_C(GND, R19.1, R19.2, R20.1, R20.2, C12.1, C12.2, C13.1, C13.2, U10.1, U10.3, U10.5)
#else
	NET_C(R19.1, U10.1, C13.1)
	NET_C(R19.2, U10.2, C12.1)
	NET_C(C12.2, U10.3, R20.1)
	NET_C(R20.2, U10.4, C13.2, U10.5)
	ALIAS(C2MHZ, U10.6)
#endif

	NET_C(J4_2.Q, U3.3)
	NET_C(R17.1, GND)
	NET_C(R17.2, U3.2, R18.1)
	NET_C(R18.2, I_V5)
	NET_C(U3.6, R16.1)

	NET_C(R16.2, CR3.K, U5.12)
	NET_C(CR3.A, GND)
	NET_C(U5.1, I_V5)
	NET_C(U5.4, GND)
	NET_C(U5.13, U5.10, U10.9, U4.15)
	NET_C(U5.3, U5.8)
	NET_C(U5.2, U5.11)
	NET_C(U5.6, U4.9)
	NET_C(U5.9, U4.2, C2MHZ.Q)

	NET_C(U10.8, U4.7)
	NET_C(U4.3, U4.1, U4.6, U4.10, I_V5)	// .10 (CET) not drawn on schems
	NET_C(U4.4, U4.5, GND)
	NET_C(U4.14, U18.1, U6.13, U7.2)

	NET_C(U18.6, U18.13)
	NET_C(U18.2, U18.12, GND)
	NET_C(U18.8, U20.8, U24.8)
	NET_C(U18.10, U23.2)

	NET_C(I_OUT_0, U2.13, U6.4)
	NET_C(U2.12, U2.1)
	ALIAS(STEERING, U2.12)
	NET_C(I_OUT_1, U6.5)
	NET_C(U6.6, U19.13, U19.4)
	NET_C(U19.12, I_V5)
	NET_C(U19.11, U20.1)
	NET_C(U19.1, U20.12)
	NET_C(U19.2, U24.12)		// guessing here -- the schematic makes no sense otherwise
	NET_C(U20.13, U24.1, U19.5) // guessing here -- the schematic makes no sense otherwise
	NET_C(U19.6, U24.2)			// guessing here -- the schematic makes no sense otherwise
	NET_C(U19.3, U20.2)
	NET_C(U20.9, U24.9, I_V5)

	NET_C(U24.13, R22.1)
	NET_C(R22.2, R23.1, R24.2, U22.7)
	NET_C(I_V5, R23.2, U22.4, U22.8)
	NET_C(R24.1, C10.2, U22.6, U22.2)
	NET_C(GND, C10.1, U22.1, C11.1)
	NET_C(C11.2, U22.5)

	NET_C(I_OUT_4, U6.2)
	NET_C(U22.3, U6.1)
	NET_C(U6.3, R33.1)

	NET_C(I_V5, U19.9)
	NET_C(U23.15, U19.10)
	NET_C(U19.8, U23.9, U17.9)
	NET_C(U23.3, U23.4, U23.5, U23.6, GND)	// P0-P3 not defined on schematics
	NET_C(U23.1, U23.7, U23.10, I_V5)		// /SR, CEP, CET not defined on schematics
	NET_C(I_V5, U17.8, U17.11)
	NET_C(I_OUT_7, U2.3)
	NET_C(U2.4, U17.10)
	NET_C(U17.5, R25.1)
	NET_C(U17.6, U17.13)					// U17 pin is not documented

	NET_C(I_OUT_1, U6.10, R32.2, U2.6, U12.4, U12.13)
	NET_C(U6.9, I_V5)
	NET_C(U6.8, U2.5, U6.12)
	NET_C(R32.1, Q4.B)
	NET_C(Q4.E, GND)
	NET_C(Q4.C, R31.1)
	ALIAS(O_START_LIGHT, R31.2)
	NET_C(R31.2, GND)
	NET_C(U6.11, U15.2, U9.2, U8.2)
	NET_C(U15.15, U10.11, U7.10)
	NET_C(U15.9, U9.9, U8.9, U10.10)
	NET_C(U15.1, U15.7, U9.1, U9.7, U8.1, U8.7, I_V5)
	NET_C(U15.10, U9.15)
	NET_C(U9.10, U8.15)
	NET_C(U8.10, I_V5)

	NET_C(I_OUT_2, U2.9)
	NET_C(U2.8, U14.1, U14.2)
	NET_C(I_OUT_3, U2.11)
	NET_C(U2.10, U14.8, U13.8)
	NET_C(U14.3, U15.6)
	NET_C(U14.4, U15.5)
	NET_C(U14.5, U15.4)
	NET_C(U14.6, U15.3)
	NET_C(U14.10, U9.6)
	NET_C(U14.11, U9.5)
	NET_C(U14.12, U9.4)
	NET_C(U14.13, U9.3, U13.1, U13.2)
	NET_C(U14.9, U13.9, I_V5)
	NET_C(U13.3, U8.6)
	NET_C(U13.4, U8.5)
	NET_C(U13.5, U8.4)
	NET_C(U13.6, U8.3)
	NET_C(U13.10, U12.7)
	NET_C(U13.11, U12.6)
	NET_C(U13.12, U12.3)
	NET_C(U13.13, U12.2)
	NET_C(U24.13, U11.12, U11.1, U11.4, U11.9)
	NET_C(U12.9, U11.13)
	NET_C(U12.10, U11.2)
	NET_C(U12.15, U11.5)
	NET_C(U12.16, U11.10)

	NET_C(U17.12, U18.11)
	NET_C(U17.1, U17.4, I_V5)
	NET_C(U17.3, R26.1)
	NET_C(U7.1, U7.5, I_V5)
	NET_C(U7.3, U7.4, U7.6, GND)
	NET_C(U7.7, STEERING)
	NET_C(U7.15, U10.13)
	NET_C(U10.12, U7.9)
	NET_C(U7.11, R27.1)
	NET_C(U7.12, R28.1)
	NET_C(U7.13, R29.1)
	NET_C(R33.2, R25.2, R26.2, R27.2, R28.2, R29.2, R21.2)

	NET_C(C20.1, GND)
	NET_C(C20.2, R21.1, R34.2)
	NET_C(U11.11, R12.1)
	NET_C(U11.3, R13.1)
	NET_C(U11.6, R14.1)
	NET_C(U11.8, R15.1)
	NET_C(R15.2, R14.2, R13.2, R12.2, R34.1)

	//
	// Amplifier
	//

	NET_C(R21.2, R30.3)
	NET_C(R30.1, GND)
	NET_C(R30.2, C9.1)
#if EMULATE_FINAL_AMP
#else
	ALIAS(OUTPUT, C9.2)
	NET_C(C9.2, GND)
#endif

	//
	// Unconnected pins
	//

//	NET_C(GND, )

NETLIST_END()


NETLIST_START(speedfrk)

	SOLVER(Solver, 48000)

	TTL_INPUT(I_OUT_0, 1)		// active low
	TTL_INPUT(I_OUT_1, 1)		// active low
	TTL_INPUT(I_OUT_2, 1)		// active low
	TTL_INPUT(I_OUT_3, 1)		// active low
	TTL_INPUT(I_OUT_4, 1)		// active low
	TTL_INPUT(I_OUT_7, 1)		// active low

	NET_C(GND, I_OUT_0.GND, I_OUT_1.GND, I_OUT_2.GND, I_OUT_3.GND, I_OUT_4.GND, I_OUT_7.GND)
	NET_C(I_V5, I_OUT_0.VCC, I_OUT_1.VCC, I_OUT_2.VCC, I_OUT_3.VCC, I_OUT_4.VCC, I_OUT_7.VCC)

	CINEMAT_LOCAL_MODELS

	LOCAL_SOURCE(SpeedFreak_schematics)
	INCLUDE(SpeedFreak_schematics)

NETLIST_END()
