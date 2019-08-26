/*
 * Super Metroid SRAM Editor
 * Copyright (C) 2005 emuWorks
 * http://games.technoplaza.net/
 *
 * This file is part of Super Metroid SRAM Editor.
 *
 * Super Metroid SRAM Editor is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Super Metroid SRAM Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Super Metroid SRAM Editor; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
 
// $Id: AppConstants.hh,v 1.3 2005/10/15 18:40:24 technoplaza Exp $

#ifndef _SMSE_APPCONSTANTS_HH_
#define _SMSE_APPCONSTANTS_HH_

/// number of save points in Brinstar
#define BS_SAVEPOINTS 5

/// number of save points in Crateria
#define C_SAVEPOINTS 2

/// offset of the checksums
#define CHECKSUM_OFFSET 0

/// offset of the redundant checksums
#define CHECKSUM_OFFSET2 0x1FF0

/// time (ms) to wait before clearing the status bar
#define CLEAR_DELAY 4000

/// offset of the checksum compliments
#define COMPLIMENT_OFFSET 0x8

/// offset of the redundant checksum compliments
#define COMPLIMENT_OFFSET2 0x1FF8

/// number of energy tanks in the game
#define ETANKS 14

/// number of eye doors in the game
#define EYEDOORS 5

/// offset to the beginning of the game data
#define GAME_OFFSET 0x10

/// size of each individual save game
#define GAME_SIZE 0x65C

/// number of save games in the SRAM data
#define GAMES 3

/// number of green doors in the game
#define GREENDOORS 23

/// idenfitier for the status bar clear timer
#define ID_SBTIMER wxID_HIGHEST + 1

/// number of save points in Maridia
#define M_SAVEPOINTS 4

/// number of metal doors in the game
#define METALDOORS 40

/// number of missile packs in the game
#define MISSILE_PACKS 46

/// number of save points in Norfair
#define NF_SAVEPOINTS 6

/// number of power bomb packs in the game
#define PBOMB_PACKS 10

/// number of reserve tanks in the game
#define RTANKS 4

/// number of red doors in the game
#define REDDOORS 30

/// number of super missile packs in the game
#define SMISSILE_PACKS 10

/// size of the SRAM file
#define SRAM_SIZE 0x2000

/// number of save points in Tourian
#define T_SAVEPOINTS 2

/// number of save points in the Wrecked Ship
#define WS_SAVEPOINTS 1

/// number of yellow doors in the game
#define YELLOWDOORS 13

namespace smse {
    /// The application name
    extern const wxString APP_NAME;
    
    /// The current version
    extern const wxString APP_VERSION;
    
    /// The copyright notice
    extern const wxString APP_COPYRIGHT;
    
    /// The website URL
    extern const wxString APP_URL;
}

#endif

