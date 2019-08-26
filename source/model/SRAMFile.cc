/*
 * Super Metroid SRAM Editor
 * Copyright (C) 2005,2007-2008 emuWorks
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
 
// $Id: SRAMFile.cc,v 1.36 2008/12/17 04:14:15 jdratlif Exp $

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <cstring>
#include <fstream>

#include "model/SRAMFile.hh"

using namespace smse;

const std::pair<int, unsigned char> SRAMFile::SRAM_OFFSET[] =
    {
        // Energy (Current/Max) [0]
        std::pair<int, unsigned char>(0x20, 0),
        std::pair<int, unsigned char>(0x22, 0),
        
        // Reserve Energy (Current/Max) [2]
        std::pair<int, unsigned char>(0x34, 0),
        std::pair<int, unsigned char>(0x32, 0),
        
        // Missiles (Current/Max) [4]
        std::pair<int, unsigned char>(0x24, 0),
        std::pair<int, unsigned char>(0x26, 0),
        
        // Super Missiles (Current/Max) [6]
        std::pair<int, unsigned char>(0x28, 0),
        std::pair<int, unsigned char>(0x2A, 0),
        
        // Power Bombs (Current/Max) [8]
        std::pair<int, unsigned char>(0x2C, 0),
        std::pair<int, unsigned char>(0x2E, 0),
        
        // Morphing Ball [10]
        std::pair<int, unsigned char>(0xB3, 0x4),
        std::pair<int, unsigned char>(0x2, 0x4),
        std::pair<int, unsigned char>(0x0, 0x4),
        
        // Bombs [13]
        std::pair<int, unsigned char>(0xB0, 0x80),
        std::pair<int, unsigned char>(0x3, 0x10),
        std::pair<int, unsigned char>(0x1, 0x10),
        
        // Spring Ball [16]
        std::pair<int, unsigned char>(0xC2, 0x40),
        std::pair<int, unsigned char>(0x2, 0x2),
        std::pair<int, unsigned char>(0x0, 0x2),
        
        // High Jump Boots [19]
        std::pair<int, unsigned char>(0xB6, 0x20),
        std::pair<int, unsigned char>(0x3, 0x1),
        std::pair<int, unsigned char>(0x1, 0x1),
        
        // Varia Suit [22]
        std::pair<int, unsigned char>(0xB6, 0x1),
        std::pair<int, unsigned char>(0x2, 0x1),
        std::pair<int, unsigned char>(0x0, 0x1),
        
        // Gravity Suit [25]
        std::pair<int, unsigned char>(0xC0, 0x80),
        std::pair<int, unsigned char>(0x2, 0x20),
        std::pair<int, unsigned char>(0x0, 0x20),
        
        // Speed Booster [28]
        std::pair<int, unsigned char>(0xB8, 0x4),
        std::pair<int, unsigned char>(0x3, 0x20),
        std::pair<int, unsigned char>(0x1, 0x20),
        
        // Space Jump [31]
        std::pair<int, unsigned char>(0xC3, 0x4),
        std::pair<int, unsigned char>(0x3, 0x2),
        std::pair<int, unsigned char>(0x1, 0x2),
        
        // Screw Attack [34]
        std::pair<int, unsigned char>(0xB9, 0x80),
        std::pair<int, unsigned char>(0x2, 0x8),
        std::pair<int, unsigned char>(0x0, 0x8),
        
        // Charge Beam [37]
        std::pair<int, unsigned char>(0xB2, 0x80),
        std::pair<int, unsigned char>(0x7, 0x10),
        std::pair<int, unsigned char>(0x5, 0x10),
        
        // Ice Beam [40]
        std::pair<int, unsigned char>(0xB6, 0x4),
        std::pair<int, unsigned char>(0x6, 0x2),
        std::pair<int, unsigned char>(0x4, 0x2),
        
        // Wave Beam [43]
        std::pair<int, unsigned char>(0xB8, 0x10),
        std::pair<int, unsigned char>(0x6, 0x1),
        std::pair<int, unsigned char>(0x4, 0x1),
        
        // Spazer [46]
        std::pair<int, unsigned char>(0xB5, 0x4),
        std::pair<int, unsigned char>(0x6, 0x4),
        std::pair<int, unsigned char>(0x4, 0x4),
        
        // Plasma Beam [49]
        std::pair<int, unsigned char>(0xC1, 0x80),
        std::pair<int, unsigned char>(0x6, 0x8),
        std::pair<int, unsigned char>(0x4, 0x8),
        
        // Grappling Beam [52]
        std::pair<int, unsigned char>(0xB7, 0x10),
        std::pair<int, unsigned char>(0x3, 0x40),
        std::pair<int, unsigned char>(0x1, 0x40),
        
        // X-Ray Scope [55]
        std::pair<int, unsigned char>(0xB4, 0x40),
        std::pair<int, unsigned char>(0x3, 0x80),
        std::pair<int, unsigned char>(0x1, 0x80),
        
        // Silver Torizo [58]
        std::pair<int, unsigned char>(0x68, 0x4),
        
        // Spore Spawn [59]
        std::pair<int, unsigned char>(0x69, 0x2),
        
        // Kraid [60]
        std::pair<int, unsigned char>(0x69, 0x1),
        std::pair<int, unsigned char>(0x61, 0x2),
        
        // Phantoon [62]
        std::pair<int, unsigned char>(0x6B, 0x1),
        std::pair<int, unsigned char>(0x60, 0x40),
        
        // Botwoon [64]
        std::pair<int, unsigned char>(0x6C, 0x2),
        
        // Draygon [65]
        std::pair<int, unsigned char>(0x6C, 0x1),
        std::pair<int, unsigned char>(0x61, 0x1),
        
        // Crocomire [67]
        std::pair<int, unsigned char>(0x6A, 0x2),
        
        // Golden Torizo [68]
        std::pair<int, unsigned char>(0x6A, 0x4),
        
        // Ridley [69]
        std::pair<int, unsigned char>(0x6A, 0x1),
        std::pair<int, unsigned char>(0x60, 0x80),
        
        // Metroid Rooms [71]
        std::pair<int, unsigned char>(0x62, 0x1),
        std::pair<int, unsigned char>(0x62, 0x2),
        std::pair<int, unsigned char>(0x62, 0x4),
        std::pair<int, unsigned char>(0x62, 0x8),
        
        // Zebetites [75]
        std::pair<int, unsigned char>(0x60, 0x3),
        
        // Tourian Elevator [76]
        std::pair<int, unsigned char>(0x61, 0x4),
        
        // Maridia Glass Tube Broken [77]
        std::pair<int, unsigned char>(0x61, 0x8),
        
        // Rescued Animals [78]
        std::pair<int, unsigned char>(0x61, 0x80),
        
        // Crateria Missile Packs (8) [79]
        std::pair<int, unsigned char>(0xB0, 0x2),
        std::pair<int, unsigned char>(0xB0, 0x4),
        std::pair<int, unsigned char>(0xB0, 0x8),
        std::pair<int, unsigned char>(0xB0, 0x10),
        std::pair<int, unsigned char>(0xB0, 0x40),
        std::pair<int, unsigned char>(0xB1, 0x2),
        std::pair<int, unsigned char>(0xB1, 0x4),
        std::pair<int, unsigned char>(0xB1, 0x10),
        
        // Brinstar Missile Packs (12) [87]
        std::pair<int, unsigned char>(0xB1, 0x80),
        std::pair<int, unsigned char>(0xB2, 0x4),
        std::pair<int, unsigned char>(0xB2, 0x8),
        std::pair<int, unsigned char>(0xB2, 0x20),
        std::pair<int, unsigned char>(0xB2, 0x40),
        std::pair<int, unsigned char>(0xB3, 0x2),
        std::pair<int, unsigned char>(0xB3, 0x10),
        std::pair<int, unsigned char>(0xB4, 0x4),
        std::pair<int, unsigned char>(0xB4, 0x10),
        std::pair<int, unsigned char>(0xB4, 0x20),
        std::pair<int, unsigned char>(0xB5, 0x2),
        std::pair<int, unsigned char>(0xB5, 0x10),
        
        // Norfair Missile Packs (15) [99]
        std::pair<int, unsigned char>(0xB6, 0x2),
        std::pair<int, unsigned char>(0xB6, 0x8),
        std::pair<int, unsigned char>(0xB6, 0x40),
        std::pair<int, unsigned char>(0xB6, 0x80),
        std::pair<int, unsigned char>(0xB7, 0x4),
        std::pair<int, unsigned char>(0xB7, 0x8),
        std::pair<int, unsigned char>(0xB7, 0x40),
        std::pair<int, unsigned char>(0xB7, 0x80),
        std::pair<int, unsigned char>(0xB8, 0x1),
        std::pair<int, unsigned char>(0xB8, 0x2),
        std::pair<int, unsigned char>(0xB8, 0x8),
        std::pair<int, unsigned char>(0xB8, 0x40),
        std::pair<int, unsigned char>(0xB9, 0x2),
        std::pair<int, unsigned char>(0xB9, 0x4),
        std::pair<int, unsigned char>(0xB9, 0x20),
        
        // Wrecked Ship Missile Packs (3) [114]
        std::pair<int, unsigned char>(0xC0, 0x1),
        std::pair<int, unsigned char>(0xC0, 0x4),
        std::pair<int, unsigned char>(0xC0, 0x8),
        
        // Maridia Missile Packs (8) [117]
        std::pair<int, unsigned char>(0xC1, 0x1),
        std::pair<int, unsigned char>(0xC1, 0x8),
        std::pair<int, unsigned char>(0xC1, 0x20),
        std::pair<int, unsigned char>(0xC1, 0x40),
        std::pair<int, unsigned char>(0xC2, 0x1),
        std::pair<int, unsigned char>(0xC2, 0x4),
        std::pair<int, unsigned char>(0xC2, 0x10),
        std::pair<int, unsigned char>(0xC2, 0x80),
        
        // Crateria Super Missile Packs (1) [125]
        std::pair<int, unsigned char>(0xB1, 0x8),
        
        // Brinstar Super Missile Packs (3) [126]
        std::pair<int, unsigned char>(0xB1, 0x40),
        std::pair<int, unsigned char>(0xB2, 0x1),
        std::pair<int, unsigned char>(0xB3, 0x80),
        
        // Norfair Super Missile Packs (1) [129]
        std::pair<int, unsigned char>(0xB8, 0x80),
        
        // Wrecked Ship Super Missile Packs (2) [130]
        std::pair<int, unsigned char>(0xC0, 0x20),
        std::pair<int, unsigned char>(0xC0, 0x40),
        
        // Maridia Super Missile Packs (3) [132]
        std::pair<int, unsigned char>(0xC1, 0x2),
        std::pair<int, unsigned char>(0xC1, 0x10),
        std::pair<int, unsigned char>(0xC2, 0x20),
        
        // Crateria Power Bomb Packs (1) [135]
        std::pair<int, unsigned char>(0xB0, 0x1),
        
        // Brinstar Power Bomb Packs (5) [136]
        std::pair<int, unsigned char>(0xB1, 0x20),
        std::pair<int, unsigned char>(0xB3, 0x1),
        std::pair<int, unsigned char>(0xB3, 0x8),
        std::pair<int, unsigned char>(0xB4, 0x80),
        std::pair<int, unsigned char>(0xB5, 0x1),
        
        // Norfair Power Bomb Packs (3) [141]
        std::pair<int, unsigned char>(0xB7, 0x2),
        std::pair<int, unsigned char>(0xB9, 0x8),
        std::pair<int, unsigned char>(0xB9, 0x10),
        
        // Maridia Power Bomb Packs (1) [144]
        std::pair<int, unsigned char>(0xC2, 0x8),
        
        // Crateria Energy Tanks (2) [145]
        std::pair<int, unsigned char>(0xB0, 0x20),
        std::pair<int, unsigned char>(0xB1, 0x1),
        
        // Brinstar Energy Tanks (5) [147]
        std::pair<int, unsigned char>(0xB3, 0x20),
        std::pair<int, unsigned char>(0xB3, 0x40),
        std::pair<int, unsigned char>(0xB4, 0x2),
        std::pair<int, unsigned char>(0xB4, 0x8),
        std::pair<int, unsigned char>(0xB5, 0x8),
        
        // Norfair Energy Tanks (4) [152]
        std::pair<int, unsigned char>(0xB6, 0x10),
        std::pair<int, unsigned char>(0xB7, 0x1),
        std::pair<int, unsigned char>(0xB9, 0x40),
        std::pair<int, unsigned char>(0xBA, 0x1),
        
        // Wrecked Ship Energy Tanks (1)  [156]
        std::pair<int, unsigned char>(0xC0, 0x10),
        
        // Maridia Energy Tanks (2) [157]
        std::pair<int, unsigned char>(0xC1, 0x4),
        std::pair<int, unsigned char>(0xC3, 0x1),
        
        // Brinstar Reserve Tank [159]
        std::pair<int, unsigned char>(0xB2, 0x2),
        
        // Norfair Reserve Tank [160]
        std::pair<int, unsigned char>(0xB7, 0x20),
        
        // Wrecked Ship Reserve Tank [161]
        std::pair<int, unsigned char>(0xC0, 0x2),
        
        // Maridia Reserve Tank [162]
        std::pair<int, unsigned char>(0xC2, 0x2),
        
        // Crateria Red Doors (3) [163]
        std::pair<int, unsigned char>(0xF0, 0x20),
        std::pair<int, unsigned char>(0xF3, 0x20),
        std::pair<int, unsigned char>(0xF3, 0x40),
        
        // Brinstar Red Doors (10) [166]
        std::pair<int, unsigned char>(0xF3, 0x80),
        std::pair<int, unsigned char>(0xF4, 0x1),
        std::pair<int, unsigned char>(0xF4, 0x2),
        std::pair<int, unsigned char>(0xF4, 0x4),
        std::pair<int, unsigned char>(0xF4, 0x8),
        std::pair<int, unsigned char>(0xF4, 0x40),
        std::pair<int, unsigned char>(0xF5, 0x4),
        std::pair<int, unsigned char>(0xF5, 0x8),
        std::pair<int, unsigned char>(0xF6, 0x4),
        std::pair<int, unsigned char>(0xF7, 0x4),
        
        // Norfair Red Doors (7) [176]
        std::pair<int, unsigned char>(0xF9, 0x4),
        std::pair<int, unsigned char>(0xF9, 0x20),
        std::pair<int, unsigned char>(0xFA, 0x2),
        std::pair<int, unsigned char>(0xFA, 0x4),
        std::pair<int, unsigned char>(0xFA, 0x20),
        std::pair<int, unsigned char>(0xFA, 0x40),
        std::pair<int, unsigned char>(0xFA, 0x80),
        
        // Wrecked Ship Red Doors (1) [183]
        std::pair<int, unsigned char>(0x101, 0x8),
        
        // Maridia Red Doors (8) [184]
        std::pair<int, unsigned char>(0x101, 0x10),
        std::pair<int, unsigned char>(0x101, 0x20),
        std::pair<int, unsigned char>(0x101, 0x40),
        std::pair<int, unsigned char>(0x102, 0x1),
        std::pair<int, unsigned char>(0x102, 0x4),
        std::pair<int, unsigned char>(0x102, 0x40),
        std::pair<int, unsigned char>(0x103, 0x1),
        
        // Tourian Red Doors (2) [191]
        std::pair<int, unsigned char>(0x104, 0x80),
        std::pair<int, unsigned char>(0x105, 0x2),
        
        // Crateria Green Doors (2) [193]
        std::pair<int, unsigned char>(0xF0, 0x1),
        std::pair<int, unsigned char>(0xF1, 0x10),
        
        // Brinstar Green Doors (10) [195]
        std::pair<int, unsigned char>(0xF5, 0x2),
        std::pair<int, unsigned char>(0xF5, 0x40),
        std::pair<int, unsigned char>(0xF6, 0x8),
        std::pair<int, unsigned char>(0xF6, 0x10),
        std::pair<int, unsigned char>(0xF6, 0x20),
        std::pair<int, unsigned char>(0xF7, 0x1),
        std::pair<int, unsigned char>(0xF7, 0x8),
        std::pair<int, unsigned char>(0xF7, 0x20),
        std::pair<int, unsigned char>(0xF7, 0x80),
        std::pair<int, unsigned char>(0xF8, 0x10),
        
        // Norfair Green Doors (6) [205]
        std::pair<int, unsigned char>(0xF9, 0x2),
        std::pair<int, unsigned char>(0xF9, 0x8),
        std::pair<int, unsigned char>(0xF9, 0x40),
        std::pair<int, unsigned char>(0xFA, 0x8),
        std::pair<int, unsigned char>(0xFA, 0x10),
        std::pair<int, unsigned char>(0xFB, 0x80),
        
        // Wrecked Ship Green Doors (1) [211]
        std::pair<int, unsigned char>(0x100, 0x10),
        
        // Maridia Green Doors (4) [212]
        std::pair<int, unsigned char>(0x101, 0x80),
        std::pair<int, unsigned char>(0x102, 0x10),
        std::pair<int, unsigned char>(0x102, 0x20),
        std::pair<int, unsigned char>(0x103, 0x4),

        // Crateria Yellow Doors (6) [216]
        std::pair<int, unsigned char>(0xF0, 0x2),
        std::pair<int, unsigned char>(0xF1, 0x20),
        std::pair<int, unsigned char>(0xF1, 0x40),
        std::pair<int, unsigned char>(0xF1, 0x80),
        std::pair<int, unsigned char>(0xF2, 0x1),
        std::pair<int, unsigned char>(0xF2, 0x8),
        
        // Brinstar Yellow Doors (4) [222]
        std::pair<int, unsigned char>(0xF5, 0x1),
        std::pair<int, unsigned char>(0xF6, 0x1),
        std::pair<int, unsigned char>(0xF7, 0x2),
        std::pair<int, unsigned char>(0xF7, 0x10),
        
        // Norfair Yellow Doors (3) [226]
        std::pair<int, unsigned char>(0xF9, 0x10),
        std::pair<int, unsigned char>(0xFB, 0x1),
        std::pair<int, unsigned char>(0xFB, 0x40),
        
        // Crateria Metal Doors (1) [229]
        std::pair<int, unsigned char>(0xF3, 0x8),
        
        // Brinstar Metal Doors (16) [230]
        std::pair<int, unsigned char>(0xF3, 0x2),
        std::pair<int, unsigned char>(0xF3, 0x4),
        std::pair<int, unsigned char>(0xF4, 0x20),
        std::pair<int, unsigned char>(0xF5, 0x10),
        std::pair<int, unsigned char>(0xF5, 0x20),
        std::pair<int, unsigned char>(0xF5, 0x80),
        std::pair<int, unsigned char>(0xF6, 0x2),
        std::pair<int, unsigned char>(0xF6, 0x40),
        std::pair<int, unsigned char>(0xF6, 0x80),
        std::pair<int, unsigned char>(0xF7, 0x40),
        std::pair<int, unsigned char>(0xF8, 0x1),
        std::pair<int, unsigned char>(0xF8, 0x2),
        std::pair<int, unsigned char>(0xF8, 0x4),
        std::pair<int, unsigned char>(0xF8, 0x8),
        std::pair<int, unsigned char>(0xF8, 0x40),
        std::pair<int, unsigned char>(0xF8, 0x80),
        
        // Norfair Metal Doors (6) [246]
        std::pair<int, unsigned char>(0xF9, 0x80),
        std::pair<int, unsigned char>(0xFA, 0x1),
        std::pair<int, unsigned char>(0xFB, 0x2),
        std::pair<int, unsigned char>(0xFB, 0x4),
        std::pair<int, unsigned char>(0xFB, 0x8),
        std::pair<int, unsigned char>(0xFC, 0x1),
        
        // Wrecked Ship Metal Doors (5) [252]
        std::pair<int, unsigned char>(0x100, 0x8),
        std::pair<int, unsigned char>(0x100, 0x40),
        std::pair<int, unsigned char>(0x101, 0x1),
        std::pair<int, unsigned char>(0x101, 0x2),
        std::pair<int, unsigned char>(0x101, 0x4),
        
        // Maridia Metal Doors (7) [257]
        std::pair<int, unsigned char>(0x102, 0x2),
        std::pair<int, unsigned char>(0x102, 0x8),
        std::pair<int, unsigned char>(0x102, 0x80),
        std::pair<int, unsigned char>(0x103, 0x10),
        std::pair<int, unsigned char>(0x103, 0x20),
        std::pair<int, unsigned char>(0x103, 0x40),
        std::pair<int, unsigned char>(0x103, 0x80),
        
        // Tourian Metal Doors (5) [264]
        std::pair<int, unsigned char>(0x104, 0x1),
        std::pair<int, unsigned char>(0x104, 0x2),
        std::pair<int, unsigned char>(0x104, 0x4),
        std::pair<int, unsigned char>(0x104, 0x8),
        std::pair<int, unsigned char>(0x104, 0x20),
        
        // Brinstar Eye Door (Kraid) [269]
        std::pair<int, unsigned char>(0xF8, 0x20),
        
        // Norfair Eye Door (Ridley) [270]
        std::pair<int, unsigned char>(0xFB, 0x10),
        
        // Wrecked Ship Eye Door (Phantoon) [271]
        std::pair<int, unsigned char>(0x100, 0x20),
        
        // Maridia Eye Door (Draygon) [272]
        std::pair<int, unsigned char>(0x103, 0x8),
        
        // Tourian Eye Door [273]
        std::pair<int, unsigned char>(0x105, 0x1),
        
        // Crateria Map [274]
        std::pair<int, unsigned char>(0x148, 0xFF),
        
        // Brinstar Map [275]
        std::pair<int, unsigned char>(0x149, 0xFF),
        
        // Norfair Map [276]
        std::pair<int, unsigned char>(0x14A, 0xFF),
        
        // Wrecked Ship Map [277]
        std::pair<int, unsigned char>(0x14B, 0xFF),
        
        // Maridia Map [278]
        std::pair<int, unsigned char>(0x14C, 0xFF),
        
        // Tourian Map [279]
        std::pair<int, unsigned char>(0x14D, 0xFF),
        
        // Save Spot (Area, Point) [280]
        std::pair<int, unsigned char>(0x158, 0),
        std::pair<int, unsigned char>(0x156, 0),
        
        // Game Hours [282]
        std::pair<int, unsigned char>(0x3E, 0xFF),
        
        // Game Minutes [283]
        std::pair<int, unsigned char>(0x3C, 0xFF),
        
        // Shot Button [284]
        std::pair<int, unsigned char>(0x10, 0),
        
        // Jump Button [285]
        std::pair<int, unsigned char>(0x12, 0),
        
        // Dash Button [286]
        std::pair<int, unsigned char>(0x14, 0),
        
        // Item Cancel Button [287]
        std::pair<int, unsigned char>(0x16, 0),
        
        // Item Select Button [288]
        std::pair<int, unsigned char>(0x18, 0),
        
        // Angle Down Button [289]
        std::pair<int, unsigned char>(0x1A, 0),
        
        // Angle Up Button [290]
        std::pair<int, unsigned char>(0x1C, 0),
        
        // Language (English or Japanese) [291]
        std::pair<int, unsigned char>(0x40, 0x1),
        
        // Moon Walk (Off or On) [292]
        std::pair<int, unsigned char>(0x42, 0x1),
        
        // Icon Cancel (Manual or Automatic) [293]
        std::pair<int, unsigned char>(0x48, 0x1)
    };
    
const std::pair<unsigned char, unsigned char> SRAMFile::BUTTON_VALUE[] =
    {
        std::pair<unsigned char, unsigned char>(0x80, 0x0),     // A
        std::pair<unsigned char, unsigned char>(0x0, 0x80),     // B
        std::pair<unsigned char, unsigned char>(0x40, 0x0),     // X
        std::pair<unsigned char, unsigned char>(0x0, 0x40),     // Y
        std::pair<unsigned char, unsigned char>(0x20, 0x0),     // L
        std::pair<unsigned char, unsigned char>(0x10, 0x0),     // R
        std::pair<unsigned char, unsigned char>(0x0, 0x20)     // Select
    };

SRAMFile::SRAMFile(const wxString &filename) throw (InvalidSRAMFileException) :
    game(sram + GAME_OFFSET), modified(false) {
    this->filename = filename;
    
    try {
        read();
    } catch (FileIOException &e) {
        throw InvalidSRAMFileException(e.what());
    }
    
    if (!hasValidGame()) {
        throw InvalidSRAMFileException("No save games found");
    }
}

void SRAMFile::setBit(const std::pair<int, unsigned char> &bit,
                      bool on) {
    if (on) {
        game[bit.first] |= bit.second;
    } else {
        game[bit.first] &= ~bit.second;
    }
    
    modified = true;
}

enum Button SRAMFile::getButton(enum Action action) const
    throw(InvalidButtonException){
    const std::pair<int, unsigned char> &offset =
        SRAM_OFFSET[action + SO_SHOTBUTTON];
        
    unsigned char high = game[offset.first];
    unsigned char low = game[offset.first + 1];
    
    for (int i = BUTTON_A; i <= BUTTON_SELECT; ++i) {
        enum Button button = static_cast<enum Button>(i);
        
        if ((high == BUTTON_VALUE[button].first) &&
            (low == BUTTON_VALUE[button].second)) {
            return button;
        }
    }
    
    // we should NEVER reach here
    throw InvalidButtonException();
}

void SRAMFile::setButton(enum Action action, enum Button button) {
    const std::pair<int, unsigned char> &offset =
        SRAM_OFFSET[action + SO_SHOTBUTTON];
        
    game[offset.first] = BUTTON_VALUE[button].first;
    game[offset.first + 1] = BUTTON_VALUE[button].second;
    
    modified = true;
}

std::pair<unsigned char,
          unsigned char> SRAMFile::getChecksum(int game, bool redundant) const {
    // ensure we have a valid game
    wxASSERT((game >= 0) && (game <= 2));
              
    int offset = CHECKSUM_OFFSET;
    
    if (redundant) {
        offset = CHECKSUM_OFFSET2;
    }
    
    return std::pair<unsigned char,
                     unsigned char>(sram[offset + (game * 2)],
                                    sram[offset + (game * 2) + 1]);
}

std::pair<unsigned char,
          unsigned char> SRAMFile::getCompliment(int game,
                                                 bool redundant) const {
    // ensure we have a valid game
    wxASSERT((game >= 0) && (game <= 2));
    
    int offset = COMPLIMENT_OFFSET;
    
    if (redundant) {
        offset = COMPLIMENT_OFFSET2;
    }
    
    return std::pair<unsigned char,
                     unsigned char>(sram[offset + (game * 2)],
                                    sram[offset + (game * 2) + 1]);
}

bool SRAMFile::getDoor(int door) const {
    // ensure we have a valid door
    wxASSERT((door >= SO_CRDOORS) && (door <= SO_TEDOOR));
    
    return getBit(SRAM_OFFSET[door]);
}

void SRAMFile::setDoor(int door, bool open) {
    // ensure we have a valid door
    wxASSERT((door >= SO_CRDOORS) && (door <= SO_TEDOOR));
    
    setBit(SRAM_OFFSET[door], open);
}

wxUint16 SRAMFile::getEnergy() const {
    const std::pair<int, unsigned char> &offset =
        SRAM_OFFSET[SO_CURRENTENERGY];
    wxUint16 *ptr = reinterpret_cast<wxUint16 *>(game + offset.first);
    
    return wxUINT16_SWAP_ON_BE(*ptr);
}

void SRAMFile::setEnergy(wxUint16 energy) {
    const std::pair<int, unsigned char> &offset =
        SRAM_OFFSET[SO_CURRENTENERGY];
    wxUint16 *ptr = reinterpret_cast<wxUint16 *>(game + offset.first);
    wxUint16 max = getMaxEnergy();
    
    if (energy > max) {
        energy = max;
    }
    
    *ptr = wxUINT16_SWAP_ON_BE(energy);
    
    modified = true;
}

bool SRAMFile::hasEnergyTank(int tank) const {
    // ensure we have a valid energy tank
    wxASSERT((tank >= SO_CETANKS) && (tank < SO_BSRTANK));
    
    return getBit(SRAM_OFFSET[tank]);
}

void SRAMFile::setEnergyTank(int tank, bool give) {
    // ensure we have a valid energy tank
    wxASSERT((tank >= SO_CETANKS) && (tank < SO_BSRTANK));
    
    if (!hasEnergyTank(tank) && give) {
        setMaxEnergy(getMaxEnergy() + 100);
    } else if (hasEnergyTank(tank) && !give) {
        setMaxEnergy(getMaxEnergy() - 100);
        setEnergy(getEnergy());
    }
    
    setBit(SRAM_OFFSET[tank], give);
}

void SRAMFile::setGame(int game) {
    // ensure we have a valid game
    wxASSERT((game >= 0) && (game <= 2));
    
    // ensure we are choosing a valid game
    wxASSERT(valid[game]);
    
    this->game = (sram + GAME_OFFSET + (GAME_SIZE * game));
}

unsigned char SRAMFile::getGameHours() const {
    std::pair<int, unsigned char> offset = SRAM_OFFSET[SO_GAMEHOURS];
    
    return game[offset.first];
}

void SRAMFile::setGameHours(unsigned char hours) {
    // ensure we have a valid number of hours
    wxASSERT(hours < 100);
    
    const std::pair<int, unsigned char> &offset = SRAM_OFFSET[SO_GAMEHOURS];
    
    game[offset.first] = hours;
    
    modified = true;
}

unsigned char SRAMFile::getGameMinutes() const {
    std::pair<int, unsigned char> offset = SRAM_OFFSET[SO_GAMEMINUTES];
    
    return game[offset.first];
}

void SRAMFile::setGameMinutes(unsigned char minutes) {
    // ensure we have a valid number of minutes
    wxASSERT(minutes < 60);
    
    const std::pair<int, unsigned char> &offset = SRAM_OFFSET[SO_GAMEMINUTES];
    
    game[offset.first] = minutes;
    
    modified = true;
}

bool SRAMFile::hasItem(enum Item item) const {
    return (getBit(SRAM_OFFSET[item]) && getBit(SRAM_OFFSET[item + 1]));
}

void SRAMFile::setItem(enum Item item, bool have) {
    setBit(SRAM_OFFSET[item], have);
    setBit(SRAM_OFFSET[item + 1], have);
}

wxUint16 SRAMFile::getMaxEnergy() const {
    const std::pair<int, unsigned char> &offset =
        SRAM_OFFSET[SO_MAXENERGY];
    wxUint16 *ptr = reinterpret_cast<wxUint16 *>(game + offset.first);
    
    return wxUINT16_SWAP_ON_BE(*ptr);
}

void SRAMFile::setMaxEnergy(wxUint16 energy) {
    const std::pair<int, unsigned char> &offset =
        SRAM_OFFSET[SO_MAXENERGY];
    wxUint16 *ptr = reinterpret_cast<wxUint16 *>(game + offset.first);
    
    *ptr = wxUINT16_SWAP_ON_BE(energy);
    
    modified = true;
}

unsigned char SRAMFile::getMaxMissiles() const{
    const std::pair<int, unsigned char> &offset =
        SRAM_OFFSET[SO_MAXMISSILES];
    
    return game[offset.first];
}

void SRAMFile::setMaxMissiles(unsigned char missiles) {
    const std::pair<int, unsigned char> &offset =
        SRAM_OFFSET[SO_MAXMISSILES];
        
    game[offset.first] = missiles;
    
    modified = true;
}

unsigned char SRAMFile::getMaxPowerBombs() const {
    const std::pair<int, unsigned char> &offset =
        SRAM_OFFSET[SO_MAXPBOMBS];
    
    return game[offset.first];
}

void SRAMFile::setMaxPowerBombs(unsigned char bombs) {
    const std::pair<int, unsigned char> &offset =
        SRAM_OFFSET[SO_MAXPBOMBS];
    
    game[offset.first] = bombs;
    
    modified = true;
}

wxUint16 SRAMFile::getMaxReserveEnergy() const {
    const std::pair<int, unsigned char> &offset =
        SRAM_OFFSET[SO_MAXRESERVE];
    wxUint16 *ptr = reinterpret_cast<wxUint16 *>(game + offset.first);
    
    return wxUINT16_SWAP_ON_BE(*ptr);
}

void SRAMFile::setMaxReserveEnergy(wxUint16 energy) {
    const std::pair<int, unsigned char> &offset =
        SRAM_OFFSET[SO_MAXRESERVE];
    wxUint16 *ptr = reinterpret_cast<wxUint16 *>(game + offset.first);
    
    *ptr = wxUINT16_SWAP_ON_BE(energy);
    
    modified = true;
}

unsigned char SRAMFile::getMaxSuperMissiles() const {
    const std::pair<int, unsigned char> &offset =
        SRAM_OFFSET[SO_MAXSMISSILES];
    
    return game[offset.first];
}

void SRAMFile::setMaxSuperMissiles(unsigned char missiles) {
    const std::pair<int, unsigned char> &offset =
        SRAM_OFFSET[SO_MAXSMISSILES];
    
    game[offset.first] = missiles;
    
    modified = true;
}

bool SRAMFile::hasMissilePack(int pack) const {
    // ensure we have a valid missile pack number
    wxASSERT((pack >= SO_CMISSILEPACKS) && (pack < SO_CSMISSILEPACKS));
    
    return getBit(SRAM_OFFSET[pack]);
}

void SRAMFile::setMissilePack(int pack, bool give) {
    // ensure we have a valid missile pack number
    wxASSERT((pack >= SO_CMISSILEPACKS) && (pack < SO_CSMISSILEPACKS));
    
    if (!hasMissilePack(pack) && give) {
        setMaxMissiles(getMaxMissiles() + 5);
    } else if (hasMissilePack(pack) && !give) {
        setMaxMissiles(getMaxMissiles() - 5);
        setMissiles(getMissiles());
    }
    
    setBit(SRAM_OFFSET[pack], give);
}

unsigned char SRAMFile::getMissiles() const {
    const std::pair<int, unsigned char> &offset =
        SRAM_OFFSET[SO_CURRENTMISSILES];
    
    return game[offset.first];
}

void SRAMFile::setMissiles(unsigned char missiles) {
    const std::pair<int, unsigned char> &offset =
        SRAM_OFFSET[SO_CURRENTMISSILES];
    unsigned char max = getMaxMissiles();
    
    if (missiles > max) {
        missiles = max;
    }
    
    game[offset.first] = missiles;
    
    modified = true;
}

bool SRAMFile::hasPowerBombPack(int pack) const {
    // ensure we have a valid power bomb pack
    wxASSERT((pack >= SO_CPBOMBPACKS) && (pack < SO_CETANKS));
    
    return getBit(SRAM_OFFSET[pack]);
}

void SRAMFile::setPowerBombPack(int pack, bool give) {
    // ensure we have a valid power bomb pack
    wxASSERT((pack >= SO_CPBOMBPACKS) && (pack < SO_CETANKS));
    
    if (!hasPowerBombPack(pack) && give) {
        setMaxPowerBombs(getMaxPowerBombs() + 5);
    } else if (hasPowerBombPack(pack) && !give) {
        setMaxPowerBombs(getMaxPowerBombs() - 5);
        setPowerBombs(getPowerBombs());
    }
    
    setBit(SRAM_OFFSET[pack], give);
}

unsigned char SRAMFile::getPowerBombs() const {
    const std::pair<int, unsigned char> &offset =
        SRAM_OFFSET[SO_CURRENTPBOMBS];
        
    return game[offset.first];
}

void SRAMFile::setPowerBombs(unsigned char bombs) {
    const std::pair<int, unsigned char> &offset =
        SRAM_OFFSET[SO_CURRENTPBOMBS];
    unsigned char max = getMaxPowerBombs();
    
    if (bombs > max) {
        bombs = max;
    }
    
    game[offset.first] = bombs;
    
    modified = true;
}

wxUint16 SRAMFile::getReserveEnergy() const {
    const std::pair<int, unsigned char> &offset =
        SRAM_OFFSET[SO_CURRENTRESERVE];
    wxUint16 *ptr = reinterpret_cast<wxUint16 *>(game + offset.first);
    
    return wxUINT16_SWAP_ON_BE(*ptr);
}

void SRAMFile::setReserveEnergy(wxUint16 energy) {
    const std::pair<int, unsigned char> &offset =
        SRAM_OFFSET[SO_CURRENTRESERVE];
    wxUint16 *ptr = reinterpret_cast<wxUint16 *>(game + offset.first);
    wxUint16 max = getMaxReserveEnergy();
    
    if (energy > max) {
        energy = max;
    }
    
    *ptr = wxUINT16_SWAP_ON_BE(energy);
    
    modified = true;
}

bool SRAMFile::hasReserveTank(int tank) const {
    // ensure we have a valid reserve tank
    wxASSERT((tank >= SO_BSRTANK) && (tank <= SO_MRTANK));
    
    return getBit(SRAM_OFFSET[tank]);
}

void SRAMFile::setReserveTank(int tank, bool give) {
    // ensure we have a valid reserve tank
    wxASSERT((tank >= SO_BSRTANK) && (tank <= SO_MRTANK));
    
    if (!hasReserveTank(tank) && give) {
        setMaxReserveEnergy(getMaxReserveEnergy() + 100);
    } else if (hasReserveTank(tank) && !give) {
        setMaxReserveEnergy(getMaxReserveEnergy() - 100);
        setReserveEnergy(getReserveEnergy());
    }
    
    setBit(SRAM_OFFSET[tank], give);
}

std::pair<enum Area, int> SRAMFile::getSavePoint() const {
    const std::pair<int, unsigned char> &area = SRAM_OFFSET[SO_SAVEAREA];
    const std::pair<int, unsigned char> &point = SRAM_OFFSET[SO_SAVEPOINT];
    
    return std::pair<enum Area, int>(static_cast<enum Area>(game[area.first]),
                                     game[point.first]);
}

void SRAMFile::setSavePoint(enum Area area, int point) {
    // ensure we have a valid save point
    wxASSERT((point >= 0) &&
             (((area == AREA_C) && (point < C_SAVEPOINTS)) ||
              ((area == AREA_BS) && (point < BS_SAVEPOINTS)) ||
              ((area == AREA_NF) && (point < NF_SAVEPOINTS)) ||
              ((area == AREA_WS) && (point < WS_SAVEPOINTS)) ||
              ((area == AREA_M) && (point < M_SAVEPOINTS)) ||
              ((area == AREA_T) && (point < T_SAVEPOINTS))));
    
    const std::pair<int, unsigned char> &high = SRAM_OFFSET[SO_SAVEAREA];
    const std::pair<int, unsigned char> &low = SRAM_OFFSET[SO_SAVEPOINT];
    
    game[high.first] = area;
    game[low.first] = point;
    
    modified = true;
}

bool SRAMFile::hasSuperMissilePack(int pack) const {
    // ensure we have a valid super missile pack
    wxASSERT((pack >= SO_CSMISSILEPACKS) && (pack < SO_CPBOMBPACKS));
    
    return getBit(SRAM_OFFSET[pack]);
}

void SRAMFile::setSuperMissilePack(int pack, bool give) {
    // ensure we have a valid super missile pack
    wxASSERT((pack >= SO_CSMISSILEPACKS) && (pack < SO_CPBOMBPACKS));
    
    if (!hasSuperMissilePack(pack) && give) {
        setMaxSuperMissiles(getMaxSuperMissiles() + 5);
    } else if (hasSuperMissilePack(pack) && !give) {
        setMaxSuperMissiles(getMaxSuperMissiles() - 5);
        setSuperMissiles(getSuperMissiles());
    }
    
    setBit(SRAM_OFFSET[pack], give);
}

unsigned char SRAMFile::getSuperMissiles() const {
    const std::pair<int, unsigned char> &offset =
        SRAM_OFFSET[SO_CURRENTSMISSILES];
        
    return game[offset.first];
}

void SRAMFile::setSuperMissiles(unsigned char missiles) {
    const std::pair<int, unsigned char> &offset =
        SRAM_OFFSET[SO_CURRENTSMISSILES];
    unsigned char max = getMaxSuperMissiles();
    
    if (missiles > max) {
        missiles = max;
    }
    
    game[offset.first] = missiles;
    
    modified = true;
}

bool SRAMFile::isValidGame(int game) const {
    // ensure we have a valid game
    wxASSERT((game >= 0) && (game <= 2));
    
    return valid[game];
}

int SRAMFile::getZebetites() const {
    const std::pair<int, unsigned char> &offset =
        SRAM_OFFSET[SO_ZEBETITES];
    
    return ((game[offset.first] & 0x38) >> offset.second);
}

void SRAMFile::setZebetites(int count) {
    // ensure a valid number of dead zebetites
    wxASSERT((count >= 0) && (count <= 4));
    
    const std::pair<int, unsigned char> &offset =
        SRAM_OFFSET[SO_ZEBETITES];
    count <<= offset.second;
    
    game[offset.first] &= ~0x38;
    game[offset.first] |= count;
    
    modified = true;
}

std::pair<unsigned char,
          unsigned char> SRAMFile::checksum(int game) const {
    // ensure we have a valid game
    wxASSERT((game >= 0) && (game <= 2));
    
    const char *ptr = (sram + GAME_OFFSET + (GAME_SIZE * game));
    short high = 0, low = 0;
    unsigned char value;
    
    for (unsigned int i = 0; i < GAME_SIZE; ++i) {
        value = ptr[i];
        
        if ((high += value) > 255) {
            high &= 0xFF;
            ++low;
        }
        
        value = ptr[++i];
        
        if ((low += value) > 255) {
            low &= 0xFF;
        }
    }
    
    return std::pair<unsigned char,
                     unsigned char>(static_cast<unsigned char>(high),
                                    static_cast<unsigned char>(low));
}

void SRAMFile::checksum() {
    for (unsigned int game = 0; game < GAMES; ++game) {
        if (valid[game]) {
            std::pair<unsigned char, unsigned char> pair = checksum(game);
            std::pair<unsigned char, unsigned char> comp = compliment(pair);
            
            sram[CHECKSUM_OFFSET + (game * 2)] = pair.first;
            sram[CHECKSUM_OFFSET + (game * 2) + 1] = pair.second;
            
            sram[COMPLIMENT_OFFSET + (game * 2)] = comp.first;
            sram[COMPLIMENT_OFFSET + (game * 2) + 1] = comp.second;
            
            sram[CHECKSUM_OFFSET2 + (game * 2)] = pair.first;
            sram[CHECKSUM_OFFSET2 + (game * 2) + 1] = pair.second;
            
            sram[COMPLIMENT_OFFSET2 + (game * 2)] = comp.first;
            sram[COMPLIMENT_OFFSET2 + (game * 2) + 1] = comp.second;
        }
    }
}

void SRAMFile::clear(int game) {
    // ensure we have a valid game
    wxASSERT((game >= 0) && (game < GAMES));
    
    if (valid[game]) {
        // clear the game data
        std::memset((sram + GAME_OFFSET + (game * GAME_SIZE)), 0, GAME_SIZE);
        
        // invalidate the checksum
        sram[CHECKSUM_OFFSET + (game * 2)] = 0xFF;
        sram[CHECKSUM_OFFSET2 + (game * 2)] = 0xFF;
        
        // the game is no longer valid
        valid[game] = false;
        
        modified = true;
    }
}

void SRAMFile::copy(int src, int dest) {
    // ensure we have valid games
    wxASSERT((src >= 0) && (src < GAMES));
    wxASSERT((dest >= 0) && (dest < GAMES));
    wxASSERT(src != dest);
    
    // ensure our source is a valid game to duplicate
    wxASSERT(valid[src]);
    
    // copy the game data
    std::memcpy((sram + GAME_OFFSET + (dest * GAME_SIZE)),
                (sram + GAME_OFFSET + (src * GAME_SIZE)),
                GAME_SIZE);
                
    // the destination is now a valid game
    valid[dest] = true;
    
    modified = true;
}

std::pair<unsigned char, unsigned char>
    SRAMFile::compliment(std::pair<unsigned char,
                                   unsigned char> checksum) const {
    return std::pair<unsigned char, unsigned char>((checksum.first ^ 0xFF),
                                                   (checksum.second ^ 0xFF));
}

bool SRAMFile::hasValidGame() {
    bool valid = false;
    
    for (unsigned int game = 0; game < GAMES; ++game) {
        std::pair<unsigned char, unsigned char> schecksum = getChecksum(game);
        std::pair<unsigned char,
                  unsigned char> scompliment = getCompliment(game);
                  
        std::pair<unsigned char, unsigned char> rchecksum =
            getChecksum(game, true);
        std::pair<unsigned char, unsigned char> rcompliment =
            getCompliment(game, true);
        
        std::pair<unsigned char, unsigned char> lchecksum = checksum(game);
        std::pair<unsigned char,
                  unsigned char> lcompliment = compliment(lchecksum);
                  
        if ((schecksum == lchecksum) && (scompliment == lcompliment) &&
            (rchecksum == lchecksum) && (rcompliment == lcompliment)) {
            this->valid[game] = true;
            valid = true;
        } else {
            this->valid[game] = false;
        }
    }
    
    return valid;
}

void SRAMFile::read() throw(FileIOException, InvalidSRAMFileException) {
    std::fstream in(filename.mb_str(),
                    std::ios_base::in | std::ios_base::binary);
    
    if (!in) {
        throw FileIOException("Unable to open file for reading!");
    }
    
    in.read(sram, SRAM_SIZE);
    
    if (in.gcount() != static_cast<std::streamsize>(SRAM_SIZE)) {
        throw FileIOException("Unable to read SRAM data!");
    }
    
    char ch;
    
    if (in.get(ch)) {
        throw InvalidSRAMFileException("Not a valid SRAM file");
    }
    
    in.close();
}

void SRAMFile::save(const wxString &filename) throw(FileIOException) {
    if (filename != wxEmptyString) {
        this->filename = filename;
    }
    
    checksum();
    write();
    
    modified = false;
}

void SRAMFile::write() throw(FileIOException) {
    std::fstream out(filename.mb_str(),
                     std::ios_base::out | std::ios_base::binary);
    
    if (!out) {
        throw FileIOException("Unable to open file for writing!");
    }
    
    out.write(sram, SRAM_SIZE);
    
    if (out.tellp() != static_cast<std::streampos>(SRAM_SIZE)) {
        throw FileIOException("Unable to write SRAM data!");
    }
    
    out.close();
}

