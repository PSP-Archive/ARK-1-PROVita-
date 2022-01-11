/*
 * This file is part of PRO CFW.

 * PRO CFW is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * PRO CFW is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PRO CFW. If not, see <http://www.gnu.org/licenses/ .
 */

#ifndef _GAMEINFO_H_
#define _GAMEINFO_H_

// UNO EUR PSN Game ID
#define NPEH00020 0

// Motorstorm Arctic Edge JPN PSN Game ID
#define UCJS10104 1

// Half Minute Hero EUR PSN Game ID
#define ULES01359 2

// UNO USA PSN Game ID
#define NPUH10027 3

// Motorstorm Arctic Edge EUR PSN Game ID
#define UCES01250 4

// Half Minute Hero JPN PSN Game ID
#define ULJS00195 5

// UNO JPN PSN Game ID
#define NPJH00016 6

// Motorstorm Arctic Edge USA PSN Game ID
#define UCUS98743 7

// Super Collapse 3 EUR PSN Game ID
#define ULES01027 8

// Super Collapse 3 USA PSN Game ID
#define ULUS10287 9

// Half Minute Hero USA PSN Game ID
#define ULUS10491 10

// Urbanix USA PSN Game ID
#define NPUZ00077 11

// Urbanix EUR PSN Game ID
#define NPEZ00176 12

// Urbanix JPN PSN Game ID
#define NPJH00082 13

// Monster Hunter Freedom 2 EUR PSN Game ID
#define ULES00851 14

// Monster Hunter Freedom Unite USA PSN Game ID
#define ULUS10391 15

// Monster Hunter Freedom Unite EUR PSN Game ID
#define ULES01213 16

// Monster Hunter Portable 2ndG JPN PSN Game ID
#define ULJM05500 17

// Monster Hunter Portable 3rd JPN PSN Game ID
#define ULJM05800 18

// Fieldrunners EUR PSN Game ID
#define NPEZ00098 19

// Fieldrunners USA PSN Game ID
#define NPUZ00014 20

// Minna no Tennis Portable JAP Game ID
#define UCJS10101 21

// Pool Hall Pro USA Game ID
#define NPUH10091 22

// Mad Blocker Alpha: Revenge of the Fuzzles (mini) [US]
#define NPUZ00210 23

// Mad Blocker Alpha: Revenge of the Fuzzles (mini) [EUR]
#define NPEZ00327 24

// Gravity Crash Portable EUR
#define NPEG00020 25

// Gravity Crash Portable JAP
#define NPJG00044 26

// Gravity Crash Portable USA
#define NPUG80321 27

// Gravity Crash Portable ASIA
#define NPHG00038 28

// World Of Pool EUR
#define ULES00821 29

// Arcade Darts USA
#define NPUZ00097 30

// Arcade Darts EUR
#define NPEZ00053 31

// Motorstorm Arctic Edge HK PSN Game ID
#define UCKS45124 32

// Everybody's Tennis EU Game ID
#define UCES01420 33

// Everybody's Tennis US Game ID
#define UCUS98701 34

// Everybody's Tennis HK Game ID
#define UCAS40307 35

// Widgets Odyssey 2 EUR
#define NPEZ00149 36

//Arcade Essentials Evolution USA
#define NPUZ00258 37

//King of Pool
#define NPUH10053 38

//Arcade Air Hockey & Bowling
#define NPUZ00103 39

//Qruton
#define NPJG00020 40

//King of Pool JP
#define NPJH00106 41

// UNO EUR PSN Game Data
#if GAME == NPEH00020
#define GAME_STRING "NPEH00020"
#define SAVE_STRING "DATA00"
#endif

// UNO USA PSN Game Data
#if GAME == NPUH10027
#define GAME_STRING "NPUH10027"
#define SAVE_STRING "DATA00"
#endif

// UNO JPN PSN Game Data
#if GAME == NPJH00016
#define GAME_STRING "NPJH00016"
#define SAVE_STRING "DATA00"
#endif

// Motorstorm Arctic Edge JPN PSN Game Data
#if GAME == UCJS10104
#define GAME_STRING "UCJS10104"
#define SAVE_STRING "0000"
#endif

// Half Minute Hero EUR PSN Game Data
#if GAME == ULES01359
#define GAME_STRING "ULES01359"

#if SAVE == 0
#define SAVE_STRING "DATA00"
#else
#if SAVE == 2
#define SAVE_STRING "DATA02"
#else
#if SAVE == 3
#define SAVE_STRING "DATA03"
#else
#if SAVE == 4
#define SAVE_STRING "DATA04"
#else
#define SAVE_STRING "DATA01"
#endif
#endif
#endif
#endif

#endif

// Motorstorm Arctic Edge EUR PSN Game Data
#if GAME == UCES01250
#define GAME_STRING "UCES01250"
#define SAVE_STRING "0000"
#endif

// Half Minute Hero JPN PSN Game Data
#if GAME == ULJS00195
#define GAME_STRING "ULJS00195"
#define SAVE_STRING "DATA00"
#endif

// Motorstorm Arctic Edge USA PSN Game Data
#if GAME == UCUS98743
#define GAME_STRING "UCUS98743"
#define SAVE_STRING "0000"
#endif

// Super Collapse 3 EUR PSN Game Data
#if GAME == ULES01027
#define GAME_STRING "ULES01027"
#define SAVE_STRING "DATA00"
#endif

// Super Collapse 3 USA PSN Game Data
#if GAME == ULUS10287
#define GAME_STRING "ULUS10287"
#define SAVE_STRING "DATA04"
#endif

// Half Minute Hero USA PSN Game Data
#if GAME == ULUS10491
#define GAME_STRING "ULUS10491"
#define SAVE_STRING "DATA01"
#endif

// Urbanix USA PSN Game Data
#if GAME == NPUZ00077
#define GAME_STRING "NPUZ00077"
#define SAVE_STRING ""
#endif

// Urbanix EUR PSN Game Data
#if GAME == NPEZ00176
#define GAME_STRING "NPEZ00176"
#define SAVE_STRING ""
#endif

// Urbanix JPN PSN Game Data
#if GAME == NPJH00082
#define GAME_STRING "NPJH00082"
#define SAVE_STRING ""
#endif

// MHF2 EUR PSN Game Data
#if GAME == ULES00851
#define GAME_STRING "ULES00851"
#define SAVE_STRING ""
#endif

// MHFU USA PSN Game Data
#if GAME == ULUS10391
#define GAME_STRING "ULUS10391"
#define SAVE_STRING ""
#endif

// MHFU EUR PSN Game Data
#if GAME == ULES01213
#define GAME_STRING "ULES01213"
#define SAVE_STRING ""
#endif

// MHP2G JPN PSN Game Data
#if GAME == ULJM05500
#define GAME_STRING "ULJM05500"
#define SAVE_STRING ""
#endif

// MHP3 JPN PSN Game Data
#if GAME == ULJM05800
#define GAME_STRING "ULJM05800"
#define SAVE_STRING "QST"
#endif

// Fieldrunners EUR PSN Game Data
#if GAME == NPEZ00098
#define GAME_STRING "NPEZ00098"
#define SAVE_STRING ""
#define GAME_SAVE_PATH "ms0:/PSP/SAVEDATA/FIELDFIRMWARE/"
#endif

// Fieldrunners USA PSN Game Data
#if GAME == NPUZ00014
#define GAME_STRING "NPUZ00014"
#define SAVE_STRING ""
#define GAME_SAVE_PATH "ms0:/PSP/SAVEDATA/FIELDFIRMWARE/"
#endif

#if GAME == UCJS10101
#define GAME_STRING "UCJS10101"
#define SAVE_STRING ""
#endif

#if GAME == NPUH10091
#define GAME_STRING "NPUH10091"
#define SAVE_STRING "SLOT00"
#endif

#if GAME == NPUZ00210
#define GAME_STRING "NPUZ00210"
#define SAVE_STRING "CPMEM"
#endif

#if GAME == NPEZ00327
#define GAME_STRING "NPEZ00327"
#define SAVE_STRING "CPMEM"
#endif

// Gravity Crash Portable EUR
#if GAME == NPEG00020
#define GAME_STRING "NPEG00020"
#define SAVE_STRING "DATA00"
#endif

// Gravity Crash Portable JAP
#if GAME == NPJG00044
#define GAME_STRING "NPJG00044"
#define SAVE_STRING "DATA00"
#endif

// Gravity Crash Portable USA
#if GAME == NPUG80321
#define GAME_STRING "NPUG80321"
#define SAVE_STRING "DATA00"
#endif

// Gravity Crash Portable ASIA
#if GAME == NPHG00038
#define GAME_STRING "NPEG00020" // Loaded from NPEG00020 because of exploit
#define SAVE_STRING "DATA01"
#endif

// World of Pool EUR
#if GAME == ULES00821
#define GAME_STRING "ULES00821"
#define SAVE_STRING "SLOT00"
#endif

// Pool Hall Pro USA
#if GAME == NPUH10091
#define GAME_STRING "NPUH10091"
#define SAVE_STRING "SLOT00"
#endif

// Arcade Darts USA
#if GAME == NPUZ00097
#define GAME_STRING "NPUZ00097"
#define SAVE_STRING "SLOT00"
#endif

// Arcade Darts EUR
#if GAME == NPEZ00053
#define GAME_STRING "NPEZ00053"
#define SAVE_STRING "SLOT00"
#endif

// Motorstorm Arctic Edge HK PSN Game Data
#if GAME == UCKS45124
#define GAME_STRING "UCKS45124"
#define SAVE_STRING "0000"
#endif

// Everybody's Tennis EU PSN Game Data
#if GAME == UCES01420
#define GAME_STRING "UCES01420"
#define SAVE_STRING ""
#endif

// Everybody's Tennis US PSN Game Data
#if GAME == UCUS98701
#define GAME_STRING "UCUS98701"
#define SAVE_STRING ""
#endif

// Everybody's Tennis HK PSN Game Data
#if GAME == UCAS40307
#define GAME_STRING "UCAS40307"
#define SAVE_STRING ""
#endif

// Widgets Odyssey 2 EUR
#if GAME == NPEZ00149
#define GAME_STRING "NPEZ00149"
#define SAVE_STRING ""
#endif

// Arcade Essential Evolution USA
#if GAME == NPUZ00258
#define GAME_STRING "NPUZ00258"
#define SAVE_STRING ""
#endif

//Pool Hall Pro
#if GAME == NPUH10091
#define GAME_STRING "NPUH10091"
#define SAVE_STRING "SLOT00"
#endif

//King of Pool
#if GAME == NPUH10053
#define GAME_STRING "NPUH10053"
#define SAVE_STRING "_GameData0"
#endif

//Arcade Air Hockey & Bowling USA
#if GAME == NPUZ00103
#define GAME_STRING "NPUZ00103"
#define SAVE_STRING "SLOT00"
#endif

//Qruton
#if GAME == NPJG00020
#define GAME_STRING "NPJG00020"
#define SAVE_STRING "SAVEDATA"
#endif

//King of Pool JP
#if GAME == NPJH00106
#define GAME_STRING "NPJH00106"
#define SAVE_STRING "_GameData0"
#endif

// Default Savedata Folder Path
#ifndef GAME_SAVE_PATH
#define GAME_SAVE_PATH "ms0:/PSP/SAVEDATA/" GAME_STRING SAVE_STRING "/"
#endif

// VSH Replacement Path
#define VSH_PATH GAME_SAVE_PATH "VBOOT.PBP"

#endif

