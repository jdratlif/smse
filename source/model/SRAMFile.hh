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
 
// $Id: SRAMFile.hh,v 1.36 2005/10/13 02:06:26 technoplaza Exp $

#ifndef _SMSE_SRAMFILE_HH_
#define _SMSE_SRAMFILE_HH_

#include <utility>

#include <wx/string.h>

#include "exceptions/FileIOException.hh"
#include "exceptions/InvalidButtonException.hh"
#include "exceptions/InvalidSRAMFileException.hh"

/// size of the SRAM file
#define SRAM_SIZE 0x2000

/// The number of save games in the SRAM data
#define GAMES 3

/// size of each individual save game
#define GAME_SIZE 0x65C

/// offset to the beginning of the game data
#define GAME_OFFSET 0x10

/// offset of the checksums
#define CHECKSUM_OFFSET 0

/// offset of the checksum compliments
#define COMPLIMENT_OFFSET 0x8

/// offset of the redundant checksums
#define CHECKSUM_OFFSET2 0x1FF0

/// offset of the redundant checksum compliments
#define COMPLIMENT_OFFSET2 0x1FF8

/// number of save points in Crateria
#define C_SAVEPOINTS 2

/// number of save points in Brinstar
#define BS_SAVEPOINTS 5

/// number of save points in Norfair
#define NF_SAVEPOINTS 6

/// number of save points in the Wrecked Ship
#define WS_SAVEPOINTS 1

/// number of save points in Maridia
#define M_SAVEPOINTS 4

/// number of save points in Tourian
#define T_SAVEPOINTS 2

namespace smse {
    /// names for the SRAM offsets
    enum {
        // Samus status
        SO_CURRENTENERGY, SO_MAXENERGY, SO_CURRENTRESERVE, SO_MAXRESERVE,
        SO_CURRENTMISSILES, SO_MAXMISSILES, SO_CURRENTSMISSILES,
        SO_MAXSMISSILES, SO_CURRENTPBOMBS, SO_MAXPBOMBS,
        
        // Samus' Items 
        SO_MORPHINGBALL, SO_BOMBS = 13, SO_SPRINGBALL = 16,
        SO_HIGHJUMPBOOTS = 19, SO_VARIASUIT = 22, SO_GRAVITYSUIT = 25,
        SO_SPEEDBOOSTER = 28, SO_SPACEJUMP = 31, SO_SCREWATTACK = 34,
        SO_CHARGEBEAM = 37, SO_ICEBEAM = 40, SO_WAVEBEAM = 43, SO_SPAZER = 46,
        SO_PLASMABEAM = 49, SO_GRAPPLINGBEAM = 52, SO_XRAYSCOPE = 55,
        
        // Bosses
        SO_SILVERTORIZO = 58, SO_SPORESPAWN, SO_KRAID, SO_PHANTOON = 62,
        SO_BOTWOON = 64, SO_DRAYGON, SO_CROCOMIRE = 67, SO_GOLDENTORIZO,
        SO_RIDLEY, SO_ZEBETITES = 71,
        
        // Misc Game Progress
        SO_TOURIANELEVATOR, SO_MARIDIATUBEBROKEN, SO_RESCUEDANIMALS,
        
        // Missile Packs
        SO_CMISSILEPACKS, SO_BSMISSILEPACKS = 83, SO_NFMISSILEPACKS = 95,
        SO_WSMISSILEPACKS = 110, SO_MMISSILEPACKS = 113,
        
        // Super Missile Packs
        SO_CSMISSILEPACKS = 121, SO_BSSMISSILEPACKS, SO_NFSMISSILEPACKS = 125,
        SO_WSSMISSILEPACKS, SO_MSMISSILEPACKS = 128,
        
        // Power Bomb Packs
        SO_CPBOMBPACKS = 131, SO_BSPBOMBPACKS, SO_NFPBOMBPACKS = 137,
        SO_MPBOMBPACKS = 140,
        
        // Energy and Reserve Tanks
        SO_CETANKS, SO_BSETANKS = 143,  SO_NFETANKS = 148, SO_WSETANKS = 152,
        SO_METANKS, SO_BSRTANK = 155, SO_NFRTANK, SO_WSRTANK, SO_MRTANK,
        
        // Red Doors
        SO_CRDOORS, SO_BSRDOORS = 162, SO_NFRDOORS = 172, SO_WSRDOORS = 179,
        SO_MRDOORS, SO_TRDOORS = 187,
        
        // Green Doors
        SO_CGDOORS = 189, SO_BSGDOORS = 191, SO_NFGDOORS = 201,
        SO_WSGDOORS = 207, SO_MGDOORS,
        
        // Yellow Doors
        SO_CYDOORS = 212, SO_BSYDOORS = 218, SO_NFYDOORS = 222,
        
        // Metal Doors
        SO_CMDOORS = 225, SO_BSMDOORS, SO_NFMDOORS = 242,
        SO_WSMDOORS = 248, SO_MMDOORS = 253, SO_TMDOORS = 260,
        
        // Eye Doors
        SO_BSEDOOR = 264, SO_NFEDOOR, SO_WSEDOOR, SO_MEDOOR, SO_TEDOOR,
        
        // Maps
        SO_CMAP, SO_BSMAP, SO_NFMAP, SO_WSMAP, SO_MMAP, SO_TMAP,
        
        // Misc Game Information
        SO_SAVEAREA, SO_SAVEPOINT, SO_GAMEHOURS, SO_GAMEMINUTES,
        
        // Game Configuration
        SO_SHOTBUTTON, SO_JUMPBUTTON, SO_DASHBUTTON, SO_ITEMCANCELBUTTON,
        SO_ITEMSELECTBUTTON, SO_ANGLEDOWNBUTTON, SO_ANGLEUPBUTTON, SO_LANGUAGE,
        SO_MOONWALK, SO_ICONCANCEL
    };
    
    /// semi-typesafe names for Samus' actions
    enum Action {
        ACTION_SHOT, ACTION_JUMP, ACTION_DASH, ACTION_ITEMCANCEL,
        ACTION_ITEMSELECT, ACTION_ANGLEDOWN, ACTION_ANGLEUP
    };
    
    /// semi-typesafe names for the areas on Zebes
    enum Area {
        AREA_C, AREA_BS, AREA_NF, AREA_WS, AREA_M, AREA_T
    };
    
    /// semi-typesafe names for the bosses
    enum Boss {
        BOSS_SILVERTORIZO = 58, BOSS_SPORESPAWN, BOSS_KRAID, BOSS_PHANTOON = 62,
        BOSS_BOTWOON = 64, BOSS_DRAYGON, BOSS_CROCOMIRE = 67, BOSS_GOLDENTORIZO,
        BOSS_RIDLEY
    };
    
    /// semi-typesafe names for the controller buttons
    enum Button {
        BUTTON_A, BUTTON_B, BUTTON_X, BUTTON_Y,
        BUTTON_L, BUTTON_R, BUTTON_SELECT
    };
    
    /// names for the doors
    enum Door {
        // Crateria Red Doors
        RD_C_MAP = 159, RD_C_BOMBS, RD_C_TELEVATOR,
        
        // Brinstar Red Doors
        RD_BS1, RD_BS_MAP, RD_BS2, RD_BS3, RD_BS4, RD_BS_RTANK,
        RD_BS_SPORESPAWN, RD_BS5, RD_BS6, RD_BS_XRAYSCOPE,
        
        // Norfair Red Doors
        RD_NF1, RD_NF_HJBOOTS, RD_NF2, RD_NF3,
        RD_NF_SPEEDBOOSTER, RD_NF4, RD_NF_WAVEBEAM,
        
        // Wrecked Ship Red Doors
        RD_WS_RTANK,
        
        // Maridia Red Doors
        RD_M1, RD_M2, RD_M3, RD_M_MAP, RD_M4, RD_M5, RD_M6,
        
        // Tourian Red Doors
        RD_T1, RD_T_MB,
        
        // Crateria Green Doors
        GD_C1, GD_C_WS,
        
        // Brinstar Green Doors
        GD_BS1, GD_BS_SPORESPAWNEXIT, GD_BS2, GD_BS3, GD_BS4,
        GD_BS5, GD_BS6, GD_BS7, GD_BS_SPAZER, GD_BS8,
        
        // Norfair Green Doors
        GD_NF1, GD_NF_ICEBEAM, GD_NF2, GD_NF3, GD_NF_SPEEDBOOSTER, GD_NF4,
        
        // Wrecked Ship Green Doors
        GD_WS,
        
        // Maridia Green Doors
        GD_M1, GD_M2, GD_M3, GD_M_DRAYGON,
        
        // Crateria Yellow Doors
        YD_C1, YD_C2, YD_C3, YD_C4, YD_C5, YD_C6,
        
        // Brinstar Yellow Doors
        YD_BS1, YD_BS2, YD_BS_XRAYSCOPE, YD_BS3,
        
        // Norfair Yellow Doors
        YD_NF_MAP, YD_NF1, YD_NF2,
        
        // Crateria Metal Doors
        MD_C_BOMBSEXIT,
        
        // Brinstar Metal Doors
        MD_BS_OLDTRIGHT, MD_BS_OLDTLEFT, MD_BS_MAPEXIT, MD_BS1, MD_BS2, MD_BS3,
        MD_BS4, MD_BS5, MD_BS6, MD_BS7, MD_BS8, MD_BS9, MD_BS_MINIKRAIDRIGHT,
        MD_BS_MINIKRAIDLEFT, MD_BS_VARIASUIT, MD_BS_KRAIDEXIT,
        
        // Norfair Metal Doors
        MD_NF_CROCOMIREEXIT, MD_NF_HJBOOTSEXIT, MD_NF_SCREWATTACK,
        MD_NF_RIDLEYEXIT, MD_NF_RIDLEYLEFT, MD_NF_GOLDSPACEPIRATES,
        
        // Wrecked Ship Metal Doors
        MD_WS1, MD_WS_PHANTOONEXIT, MD_WS2, MD_WS3, MD_WS4,
        
        // Maridia Metal Doors
        MD_M_PLASMAEXIT, MD_M_PLASMA, MD_M1, MD_M2,
        MD_M_BOTWOONEXIT, MD_M_DRAYGONEXIT, MD_M_SPACEJUMP,
        
        // Tourian Metal Doors
        MD_T1, MD_T2, MD_T3, MD_T4,
        
        // Eye Doors
        ED_KRAID, ED_RIDLEY, ED_PHANTOON, ED_DRAYGON, ED_T
    };
    
    /// semi-typesafe names for Samus' equipment
    enum Item {
        ITEM_MORPHINGBALL = 10, ITEM_BOMBS = 13, ITEM_SPRINGBALL = 16,
        ITEM_HJBOOTS = 19, ITEM_VARIASUIT = 22, ITEM_GRAVITYSUIT = 25,
        ITEM_SPEEDBOOSTER = 28, ITEM_SPACEJUMP = 31, ITEM_SCREWATTACK = 34,
        ITEM_CHARGEBEAM = 37, ITEM_ICEBEAM = 40, ITEM_WAVEBEAM = 43,
        ITEM_SPAZER = 46, ITEM_PLASMA = 49, ITEM_GRAPPLINGBEAM = 52,
        ITEM_XRAYSCOPE = 55
    };
    
    /// semi-typesafe names for the miscellaneous bits
    enum MiscBit {
        MB_TELEVATOR = 72, MB_MTUBEBROKEN, MB_RESCUEDANIMALS,
        MB_LANGUAGE = 286, MB_MOONWALK, MB_ICONCANCEL
    };
    
    /// semi-typesafe names for the boss statues
    enum Statue {
        STATUE_KRAID = 61, STATUE_PHANTOON = 63,
        STATUE_DRAYGON = 66, STATUE_RIDLEY = 70
    };
    
    /// Class to encapsulate a Super Metroid SRAM File
    class SRAMFile {
    private:
        /// all the SRAM offsets
        static const std::pair<int, unsigned char> SRAM_OFFSET[];
        
        /// the controller button values
        static const std::pair<unsigned char, unsigned char> BUTTON_VALUE[];
        
        wxString filename;
        char sram[SRAM_SIZE];
        char *game;
        bool valid[3], modified;
        
        /**
         * Checks if a bit is set in the SRAM.
         *
         * @param bit The bit to check.
         *
         * @return true if the bit is set; false otherwise.
         */
        bool getBit(const std::pair<int, unsigned char> &bit) const;
        
        /**
         * Sets one of the bits in the SRAM.
         *
         * @param bit The bit to set.
         * @param on true to set the bit; false otherwise.
         */
        void setBit(const std::pair<int, unsigned char> &bit, bool on = true);
        
        /**
         * Gets the checksum for a particular save game.
         *
         * @param game The game whose checksum to get.
         * @param redundant true for the redundant checksum; false otherwise.
         *
         * @return The checksum.
         */
        std::pair<unsigned char,
                  unsigned char> getChecksum(int game,
                                             bool redundant = false) const;
                                  
        /**
         * Gets the checksum compliment for a particular save game.
         *
         * @param game The game whose checksum compliment to get.
         * @param redundant true for the redundant compliment; false otherwise.
         *
         * @return The checksum compliment.
         */
        std::pair<unsigned char,
                  unsigned char> getCompliment(int game,
                                               bool redundant = false) const;
                                               
        /**
         * Sets the maximum energy Samus can have.
         *
         * @param energy The max energy.
         */
        void setMaxEnergy(wxUint16 energy = 0x5DB);
        
        /**
         * Sets the maximum number of missiles Samus can carry.
         *
         * @param missiles The max missiles.
         */
        void setMaxMissiles(unsigned char missiles = 0xE6);
        
        /**
         * Sets the maximum number of power bombs Samus can carry.
         *
         * @param bombs The max power bombs.
         */
        void setMaxPowerBombs(unsigned char bombs = 0x32);
        
        /**
         * Sets the maximum reserve energy Samus can have.
         *
         * @param energy The max reserve energy.
         */
        void setMaxReserveEnergy(wxUint16 energy = 0x190);
        
        /**
         * Sets the maximum number of super missiles Samus can carry.
         *
         * @param missiles The max super missiles.
         */
        void setMaxSuperMissiles(unsigned char missiles = 0x32);
        
        /**
         * Calculates the checksum for one of the save games.
         *
         * @param game The game to checksum (0-2).
         */
        std::pair<unsigned char, 
                  unsigned char> checksum(int game) const;
                  
        /**
         * Recalculates and updates the checksum for this SRAMFile.
         */
        void checksum();
                  
        /**
         * Calculates the compliment for a given checksum.
         *
         * @param checksum The checksum to compliment.
         *
         * @return The checksum compliment.
         */
        std::pair<unsigned char, unsigned char>
            compliment(std::pair<unsigned char, unsigned char> checksum) const;

        /**
         * Checks if the SRAM contains a valid game.
         *
         * @return true if the SRAM has a valid game; false otherwise.
         */
        bool hasValidGame();
        
        /**
         * Reads the SRAM data from disk.
         *
         * @throw FileIOException if the file cannot be read.
         * @throw InvalidSRAMFileException is the file is not a valid SRAM file.
         */
        void read() throw(FileIOException, InvalidSRAMFileException);
        
        /**
         * Writes the SRAM data to the disk.
         *
         * @throw FileIOException if the file cannot be written.
         */
        void write() throw(FileIOException);
        
    public:
        /**
         * Creates a new SRAMFile.
         *
         * @param filename The SRAM file to open.
         *
         * @throw InvalidSRAMFileException if the given file is not a valid
         *                                 SRAM file.
         */
        SRAMFile(const wxString &filename) throw(InvalidSRAMFileException);
        
        /**
         * Gets whether a boss has been killed or not.
         *
         * @param boss The boss to check.
         *
         * @return true if the boss is dead; false otherwise.
         */
        bool getBoss(enum Boss boss) const;
        
        /**
         * Sets whether a boss has been killed or not.
         *
         * @param boss The boss.
         * @param dead true to kill the boss; false otherwise.
         */
        void setBoss(enum Boss boss, bool dead = true);
        
        /**
         * Gets the button associated with an action.
         *
         * @param action The action the button is associated with.
         *
         * @return The button.
         *
         * @throw InvalidButtonException if the action has no valid button
         *                               associated with it.
         */
        enum Button getButton(enum Action action) const
            throw(InvalidButtonException);
        
        /**
         * Sets the button associated with an action.
         *
         * @param action The action the button is to be associated with.
         * @param button The button to associate with the action.
         */
        void setButton(enum Action action, enum Button button);
        
        /**
         * Checks if a door is open.
         *
         * @param door The door to check.
         *
         * @return true if the door is open; false otherwise.
         */
        bool getDoor(int door) const;
        
        /**
         * Sets whether a door is open or not.
         *
         * @param door The door.
         * @param open true to open the door; false otherwise.
         */
        void setDoor(int door, bool open = true);
        
        /**
         * Gets the amount of energy Samus has.
         *
         * @return Samus' energy.
         */
        wxUint16 getEnergy() const;
        
        /**
         * Sets the amount of energy Samus has.
         *
         * @param energy Samus' energy.
         */
        void setEnergy(wxUint16 energy = 0x5DB);
        
        /**
         * Checks if Samus has a particular energy tank.
         *
         * @param tank The tank to check for.
         *
         * @return true if Samus has the energy tank; false otherwise.
         */
        bool hasEnergyTank(int tank) const;
        
        /**
         * Sets whether Samus has a particular energy tank or not.
         *
         * @param tank The tank to set.
         * @param give true to give the energy tank; false otherwise.
         */
        void setEnergyTank(int tank, bool give = true);
        
        /**
         * Gets the current game being edited.
         *
         * @return The current game (0-2).
         */
        int getGame() const;
        
        /**
         * Sets the game to be edited.
         *
         * @param game The game number (0-2).
         */
        void setGame(int game = 0);
        
        /**
         * Gets the number of game hours played.
         *
         * @return The game hours.
         */
        unsigned char getGameHours() const;
        
        /**
         * Sets the number of game hours played.
         *
         * @param hours The game hours.
         */
        void setGameHours(unsigned char hours = 0);
        
        /**
         * Gets the number of game minutes played.
         *
         * @return The game minutes.
         */
        unsigned char getGameMinutes() const;
        
        /**
         * Sets the number of game minutes played.
         *
         * @param minutes The game minutes.
         */
        void setGameMinutes(unsigned char minutes = 0);
        
        /**
         * Checks if Samus has an item in her inventory.
         *
         * @param item The item to check for.
         *
         * @return true if Samus has the item; false otherwise.
         */
        bool hasItem(enum Item item) const;
        
        /**
         * Sets whether Samus has a particular item.
         *
         * @param item The item to set.
         * @param have true for Samus to have the item; false otherwise.
         */
        void setItem(enum Item item, bool have = true);
        
        /**
         * Checks whether an item is equipped.
         *
         * @param item The item to check for.
         *
         * @return true if the item is equipped; false otherwise.
         */
        bool isItemEquipped(enum Item item) const;
        
        /**
         * Sets whether an item is equipped or not.
         *
         * @param item The item to (un)equip.
         * @param equipped true to equip the item; false otherwise.
         */
        void setItemEquipped(enum Item item, bool equipped = true);
        
        /**
         * Checks if Samus has a particular area map.
         *
         * @param area The area whose map to check for.
         *
         * @return true if Samus has the map; false otherwise.
         */
        bool hasMap(enum Area area) const;
        
        /**
         * Sets whether Samus has a particular area map or not.
         *
         * @param area The area for the map.
         * @param give true to give the map; false otherwise.
         */
        void setMap(enum Area area, bool give = true);
        
        /**
         * Gets the maximum energy Samus can have.
         *
         * @return The max energy.
         */
        wxUint16 getMaxEnergy() const;
        
        /**
         * Gets the maximum number of missiles Samus can carry.
         *
         * @return The max missiles.
         */
        unsigned char getMaxMissiles() const;
        
        /**
         * Gets the maximum number of power bombs Samus can carry.
         *
         * @return The max power bombs.
         */
        unsigned char getMaxPowerBombs() const;
        
        /**
         * Gets the maximum reserve energy Samus can have.
         *
         * @return The max reserve energy.
         */
        wxUint16 getMaxReserveEnergy() const;
        
        /**
         * Gets the maximum number of super missiles Samus can carry.
         *
         * @return The max super missiles.
         */
        unsigned char getMaxSuperMissiles() const;
        
        /**
         * Gets one of the miscellaneous bits that don't fit in with any other
         * group of data.
         *
         * @param bit The bit to get.
         *
         * @return true if the bit is active; false otherwise.
         */
        bool getMiscBit(enum MiscBit bit) const;
        
        /**
         * Sets one of the miscellaneous bits that don't fit in with any other
         * group of data.
         *
         * @param bit The bit to set.
         * @param on true to set the bit; false otherwise.
         */
        void setMiscBit(enum MiscBit bit, bool on = true);
        
        /**
         * Checks if Samus has a particular missile pack.
         *
         * @param pack The missile pack to check.
         *
         * @return true if Samus has the missile pack; false otherwise.
         */
        bool hasMissilePack(int pack) const;
        
        /**
         * Sets whether Samus has a particular missile pack or not.
         *
         * @param pack The missile pack.
         * @param give true to give the missile pack; false otherwise.
         */
        void setMissilePack(int pack, bool give = true);
        
        /**
         * Gets the number of missiles in Samus' inventory.
         *
         * @return The number of missiles.
         */
        unsigned char getMissiles() const;
        
        /**
         * Sets the number of missiles in Samus' inventory.
         *
         * @param missiles The number of missiles.
         */
        void setMissiles(unsigned char missiles = 0xE6);
        
        /**
         * Checks if this SRAMFile has been modified.
         *
         * @return true if the SRAMFile has been modified; false otherwise.
         */
        bool isModified() const;
        
        /**
         * Checks if Samus has a particular power bomb pack.
         *
         * @param pack The power bomb pack to check.
         *
         * @return true if Samus has the power bomb pack; false otherwise.
         */
        bool hasPowerBombPack(int pack) const;
        
        /**
         * Sets whether Samus has a particular power bomb pack or not.
         *
         * @param pack The power bomb pack.
         * @param give true to give the power bomb pack; false otherwise.
         */
        void setPowerBombPack(int pack, bool give = true);
        
        /**
         * Gets the number of power bombs in Samus' inventory.
         *
         * @return The number of power bombs.
         */
        unsigned char getPowerBombs() const;
        
        /**
         * Sets the number of power bombs in Samus' inventory.
         *
         * @param bombs The number of power bombs.
         */
        void setPowerBombs(unsigned char bombs = 0x32);
        
        /**
         * Gets the amount of reserve energy Samus has.
         *
         * @return Samus' reserve energy.
         */
        wxUint16 getReserveEnergy() const;
        
        /**
         * Sets the amount of reserve energy Samus has.
         *
         * @param energy Samus' reserve energy.
         */
        void setReserveEnergy(wxUint16 energy = 0x190);
        
        /**
         * Checks if Samus has a particular reserve tank.
         *
         * @param tank The reserve tank to check.
         *
         * @return true if Samus has the reserve tank; false otherwise.
         */
        bool hasReserveTank(int tank) const;
        
        /**
         * Sets whether Samus has a particular reserve tank or not.
         *
         * @param tank The reserve tank.
         * @param give true to give the reserve tank; false otherwise.
         */
        void setReserveTank(int tank, bool give = true);
        
        /**
         * Gets the save point.
         *
         * @return The save point as a std::pair consisting of the area and the
         *         specific save point within the area.
         */
        std::pair<enum Area, int> getSavePoint() const;
        
        /**
         * Sets the save point.
         *
         * @param area The area of the save point.
         * @param point The specific save point within the area.
         */
        void setSavePoint(enum Area area, int point = 0);
        
        /**
         * Gets whether a boss statue has been activated or not.
         *
         * @param statue The statue to check.
         *
         * @return true if the statue has been activated; false otherwise.
         */
        bool getStatue(enum Statue statue) const;
        
        /**
         * Sets whether a boss statue has been activated or not.
         *
         * @param statue The statue.
         * @param active true to activate the statue; false otherwise.
         */
        void setStatue(enum Statue statue, bool active = true);
        
        /**
         * Checks if Samus has a particular super missile pack.
         *
         * @param pack The super missile pack to check.
         *
         * @return true if Samus has the super missile pack; false otherwise.
         */
        bool hasSuperMissilePack(int pack) const;
        
        /**
         * Sets whether Samus has a particular super missile pack or not.
         *
         * @param pack The super missile pack.
         * @param give true to give the super missile pack; false otherwise.
         */
        void setSuperMissilePack(int pack, bool give = true);
        
        /**
         * Gets the number of super missiles in Samus' inventory.
         *
         * @return The number of super missiles.
         */
        unsigned char getSuperMissiles() const;
        
        /**
         * Sets the number of super missiles in Samus' inventory.
         *
         * @param missiles The number of super missiles.
         */
        void setSuperMissiles(unsigned char missiles = 0x32);
        
        /**
         * Checks if a particular game is valid.
         *
         * @param game The game to check.
         *
         * @return true if the game is valid; false otherwise.
         */
        bool isValidGame(int game = 0) const;
        
        /**
         * Gets the number of Zebetites destroyed.
         *
         * @return The number of destroyed Zebetites.
         */
        int getZebetites() const;
        
        /**
         * Sets the number of Zebetites destroyed.
         *
         * @param count The number of destroyed Zebetites.
         */
        void setZebetites(int count = 4);
        
        /**
         * Saves this SRAMFile to disk.
         *
         * @param filename The filename to save as or wxEmptyString to overwrite
         *                 the original file.
         *
         * @throw FileIOException if the SRAMFile cannot be saved.
         */
        void save(const wxString &filename = wxEmptyString)
            throw(FileIOException);
    };
    
    inline bool SRAMFile::getBit(const std::pair<int, unsigned char> &bit) const
        { return (game[bit.first] & bit.second); }
    inline bool SRAMFile::getBoss(enum Boss boss) const
        { return getBit(SRAM_OFFSET[boss]); }
    inline void SRAMFile::setBoss(enum Boss boss, bool dead)
        { setBit(SRAM_OFFSET[boss], dead); }
    inline int SRAMFile::getGame() const
        { return (static_cast<int>(game - sram - GAME_OFFSET) / GAME_SIZE); }
    inline bool SRAMFile::isItemEquipped(enum Item item) const
        { return getBit(SRAM_OFFSET[item + 2]); }
    inline void SRAMFile::setItemEquipped(enum Item item, bool equipped)
        { setBit(SRAM_OFFSET[item + 2], equipped); }
    inline bool SRAMFile::hasMap(enum Area area) const
        { return getBit(SRAM_OFFSET[area + SO_CMAP]); }
    inline void SRAMFile::setMap(enum Area area, bool give)
        { setBit(SRAM_OFFSET[area + SO_CMAP], give); }
    inline bool SRAMFile::getMiscBit(enum MiscBit bit) const
        { return getBit(SRAM_OFFSET[bit]); }
    inline void SRAMFile::setMiscBit(enum MiscBit bit, bool on)
        { setBit(SRAM_OFFSET[bit], on); }
    inline bool SRAMFile::isModified() const { return modified; }
    inline bool SRAMFile::getStatue(enum Statue statue) const
        { return getBit(SRAM_OFFSET[statue]); }
    inline void SRAMFile::setStatue(enum Statue statue, bool active)
        { setBit(SRAM_OFFSET[statue], active); }
}

#endif

