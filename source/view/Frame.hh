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
 
// $Id: Frame.hh,v 1.46 2008/12/17 04:14:15 jdratlif Exp $

#ifndef SMSE_FRAME_HH_
#define SMSE_FRAME_HH_

#include <wx/spinbutt.h>

/// idenfitier for the status bar clear timer
#define ID_SBTIMER wxID_HIGHEST + 1

namespace smse {
    class SRAMFile;

    /// The main application frame
    class Frame : public wxFrame {
        DECLARE_CLASS(Frame)
        DECLARE_EVENT_TABLE()
        
        friend class SRAMFileTarget;
        
    private:
        /// time (ms) to wait before clearing the status bar
        static const int CLEAR_DELAY = 4000;
        
        /// number of save points in Crateria
        static const int C_SAVEPOINTS = 2;
        
        /// number of save points in Brinstar
        static const int BS_SAVEPOINTS = 5;
        
        /// number of save points in Norfair
        static const int NF_SAVEPOINTS = 6;
        
        /// number of save points in the Wrecked Ship
        static const int WS_SAVEPOINTS = 1;
        
        /// number of save points in Maridia
        static const int M_SAVEPOINTS = 4;
        
        /// number of save points in Tourian
        static const int T_SAVEPOINTS = 2;
        
        /// number of missile packs in the game
        static const int MISSILE_PACKS = 46;
        
        /// number of super missile packs in the game
        static const int SMISSILE_PACKS = 10;
        
        /// number of power bomb packs in the game
        static const int PBOMB_PACKS = 10;
        
        /// number of energy tanks in the game
        static const int ETANKS = 14;
        
        /// number of reserve tanks in the game
        static const int RTANKS = 4;
        
        /// number of red doors in the game
        static const int REDDOORS = 30;
        
        /// number of green doors in the game
        static const int GREENDOORS = 23;
        
        /// number of yellow doors in the game
        static const int YELLOWDOORS = 13;
        
        /// number of metal doors in the game
        static const int METALDOORS = 40;
        
        /// number of eye doors in the game
        static const int EYEDOORS = 5;

        wxTimer timer;
        SRAMFile *sram;
        int game;
        bool ignoreTextEvents;
    
        /**
         * Creates the controls for this Frame.
         */
        void CreateControls();
        
        /**
         * Closes the current SRAMFile.
         *
         * @param save true to save before closing; false otherwise.
         *
         * @return true if the file was closed; false otherwise.
         */
        bool close(bool save = false);
        
        /**
         * Enables or disables the menus according to the SRAMFile status.
         */
        void enableMenus();
        
        /**
         * Loads save game data into the Frame controls.
         *
         * @param game The game to load.
         */
        void load(int game = 0);
        
        /**
         * Loads boss data into the controls.
         */
        void loadBosses();
        
        /**
         * Loads the game data into the controls on the Color Doors tab.
         */
        void loadColorDoorsTab();
        
        /**
         * Loads the controller configuration into the controls.
         */
        void loadControllerConfig();
        
        /**
         * Loads Samus' energy data into the controls.
         */
        void loadEnergy();
        
        /**
         * Loads energy tank data into the controls.
         */
        void loadEnergyTanks();
        
        /**
         * Loads eye door data into the controls.
         */
        void loadEyeDoors();
        
        /**
         * Loads the misc game configuration into the controls.
         */
        void loadGameConfig();
        
        /**
         * Loads the misc game status into the controls.
         */
        void loadGameStatus();
        
        /**
         * Loads the game data into the controls on the Game tab.
         */
        void loadGameTab();
        
        /**
         * Loads the game time into the controls.
         */
        void loadGameTime();
        
        /**
         * Loads green door data into the controls.
         */
        void loadGreenDoors();
        
        /**
         * Loads Samus' inventory data into the controls.
         */
        void loadInventory();
        
        /**
         * Loads Samus' item data into the controls.
         */
        void loadItems();
        
        /**
         * Loads the game data into the controls on the Items tab.
         */
        void loadItemsTab();
        
        /**
         * Loads area map data into the controls.
         */
        void loadMaps();
        
        /**
         * Loads metal door data into the controls.
         */
        void loadMetalDoors();
        
        /**
         * Loads the metroid room data into the controls.
         */
        void loadMetroidRooms();
        
        /**
         * Loads the miniboss data into the controls.
         */
        void loadMiniBosses();
        
        /**
         * Loads the game data into the controls on the Misc tab.
         */
        void loadMiscTab();
        
        /**
         * Loads missile pack data into the controls.
         */
        void loadMissilePacks();
        
        /**
         * Loads power bomb pack data into the contols.
         */
        void loadPowerBombPacks();
        
        /**
         * Loads red door data into the controls.
         */
        void loadRedDoors();
        
        /**
         * Loads reserve tank data into the controls.
         */
        void loadReserveTanks();
        
        /**
         * Loads the save point data into the controls.
         */
        void loadSavePoint();
        
        /**
         * Loads the game data into the controls on the Special Doors tab.
         */
        void loadSpecialDoorsTab();
        
        /**
         * Loads super missile pack data into the controls.
         */
        void loadSuperMissilePacks();
        
        /**
         * Loads yellow door data into the controls.
         */
        void loadYellowDoors();
        
        /**
         * Loads zebetite data into the controls.
         */
        void loadZebetites();
        
        /**
         * Saves the SRAMFile to disk.
         *
         * @param filename The filename to save as.
         *
         * @return true if the SRAMFile was saved; false otherwise.
         */
        bool save(const wxString &filename = wxEmptyString);
        
        /**
         * Called when a boss control is changed.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onBossChange(wxCommandEvent &event);
        
        /**
         * Called when one of the controller configuration controls is changed.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onControllerChange(wxCommandEvent &event);
        
        /**
         * Called when an energy control is changed.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onEnergyChange(wxCommandEvent &event);
        
        /**
         * Called when an energy tank control is changed.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onEnergyTankChange(wxCommandEvent &event);
        
        /**
         * Called when an eye door control is changed.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onEyeDoorChange(wxCommandEvent &event);
        
        /**
         * Called when close from the file menu is selected.
         *
         * @param event The triggering wxCommandEvent (unused).
         */
        void onFileClose(wxCommandEvent &event);
        
        /**
         * Called in idle time to update the close file menu item.
         *
         * @param event The triggering wxUpdateUIEvent.
         */
        void onFileCloseUpdate(wxUpdateUIEvent &event);
        
        /**
         * Called when exit from the file menu is selected.
         *
         * @param event The triggering wxCommandEvent (unused).
         */
        void onFileExit(wxCommandEvent &event);
        
        /**
         * Called when open from the file menu is selected.
         *
         * @param event The triggering wxCommandEvent (unused).
         */
        void onFileOpen(wxCommandEvent &event);
        
        /**
         * Called when save from the file menu is selected.
         *
         * @param event The triggering wxCommandEvent (unused).
         */
        void onFileSave(wxCommandEvent &event);
        
        /**
         * Called when save as from the file menu is selected.
         *
         * @param event The triggering wxCommandEvent (unused).
         */
        void onFileSaveAs(wxCommandEvent &event);
        
        /**
         * Called in idle time to update the save and save as file menu items.
         *
         * @param event The triggering wxUpdateUIEvent (unused).
         */
        void onFileSaveUpdate(wxUpdateUIEvent &event);
        
        /**
         * Called when a game is selected from the game menu.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onGame(wxCommandEvent &event);
        
        /**
         * Called when one of the clear submenu items is selected.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onGameClear(wxCommandEvent &event);
        
        /**
         * Called in idle time to update the clear submenu.
         *
         * @param event The triggering wxUpdateUIEvent.
         */
        void onGameClearUpdate(wxUpdateUIEvent &event);
        
        /**
         * Called when one of the copy submenu items is selected.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onGameCopy(wxCommandEvent &event);
        
        /**
         * Called in idle time to update the copy submenu.
         *
         * @param event The triggering wxUpdateUIEvent.
         */
        void onGameCopyUpdate(wxUpdateUIEvent &event);
        
        /**
         * Called when an ending from the game menu is selected.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onGameEnding(wxCommandEvent &event);
        
        /**
         * Called in idle time to update the ending submenu.
         *
         * @param event The triggering wxUpdateUIEvent.
         */
        void onGameEndingUpdate(wxUpdateUIEvent &event);
        
        /**
         * Called when bosses is selected from the give submenu.
         *
         * @param event The triggering wxCommandEvent (unused).
         */
        void onGameGiveBosses(wxCommandEvent &event);
        
        /**
         * Called when energy is selected from the give submenu.
         *
         * @param event The triggering wxCommandEvent (unused).
         */
        void onGameGiveEnergy(wxCommandEvent &event);
        
        /**
         * Called when energy tanks is selected from the give submenu.
         *
         * @param event The triggering wxCommandEvent (unused).
         */
        void onGameGiveEnergyTanks(wxCommandEvent &event);
        
        /**
         * Called when everything is selected from the give submenu.
         *
         * @param event The triggering wxCommandEvent (unused).
         */
        void onGameGiveEverything(wxCommandEvent &event);
        
        /**
         * Called when eye doors is selected from the give submenu.
         *
         * @param event The triggering wxCommandEvent (unused).
         */
        void onGameGiveEyeDoors(wxCommandEvent &event);
        
        /**
         * Called when green doors is selected from the give submenu.
         *
         * @param event The triggering wxCommandEvent (unused).
         */
        void onGameGiveGreenDoors(wxCommandEvent &event);
        
        /**
         * Called when inventory is selected from the give submenu.
         *
         * @param event The triggering wxCommandEvent (unused).
         */
        void onGameGiveInventory(wxCommandEvent &event);
        
        /**
         * Called when items is selected from the give submenu.
         *
         * @param event The triggering wxCommandEvent (unused).
         */
        void onGameGiveItems(wxCommandEvent &event);
        
        /**
         * Called when maps is selected from the give submenu.
         *
         * @param event The triggering wxCommandEvent (unused).
         */
        void onGameGiveMaps(wxCommandEvent &event);
        
        /**
         * Called when metal doors is selected from the give submenu.
         *
         * @param event The triggering wxCommandEvent (unused).
         */
        void onGameGiveMetalDoors(wxCommandEvent &event);
        
        /**
         * Called when metroid rooms is selected from the give submenu.
         *
         * @param event The triggering wxCommandEvent (unused).
         */
        void onGameGiveMetroidRooms(wxCommandEvent &event);
        
        /**
         * Called when mini-bosses is selected from the give submenu.
         *
         * @param event The triggering wxCommandEvent (unused).
         */
        void onGameGiveMiniBosses(wxCommandEvent &event);
        
        /**
         * Called when missile packs is selected from the give submenu.
         *
         * @param event The triggering wxCommandEvent (unused).
         */
        void onGameGiveMissilePacks(wxCommandEvent &event);
        
        /**
         * Called when power bombs is selected from the give submenu.
         *
         * @param event The triggering wxCommandEvent (unused).
         */
        void onGameGivePowerBombPacks(wxCommandEvent &event);
        
        /**
         * Called when red doors is selected from the give submenu.
         *
         * @param event The triggering wxCommandEvent (unused).
         */
        void onGameGiveRedDoors(wxCommandEvent &event);
        
        /**
         * Called when reserve tanks is selected from the give submenu.
         *
         * @param event The triggering wxCommandEvent (unused).
         */
        void onGameGiveReserveTanks(wxCommandEvent &event);
        
        /**
         * Called when super missile packs is selected from the give submenu.
         *
         * @param event The triggering wxCommandEvent (unused).
         */
        void onGameGiveSuperMissilePacks(wxCommandEvent &event);
        
        /**
         * Called in idle time to update the give submenu.
         *
         * @param event The triggering wxUpdateUIEvent.
         */
        void onGameGiveUpdate(wxUpdateUIEvent &event);
        
        /**
         * Called when yellow doors is selected from the give submenu.
         *
         * @param event The triggering wxCommandEvent (unused).
         */
        void onGameGiveYellowDoors(wxCommandEvent &event);
        
        /**
         * Called when zebetites is selected rom the give submenu.
         *
         * @param event The triggering wxCommandEvent (unused).
         */
        void onGameGiveZebetites(wxCommandEvent &event);
        
        /**
         * Called when the game time control is changed.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onGameTimeChange(wxCommandEvent &event);
        
        /**
         * Called in idle time to update the game menu items.
         *
         * @param event The triggering wxUpdateUIEvent.
         */
        void onGameUpdate(wxUpdateUIEvent &event);
        
        /**
         * Called when a green door control is changed.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onGreenDoorChange(wxCommandEvent &event);
        
        /**
         * Called when help from the about menu is selected.
         *
         * @param event The triggering wxCommandEvent (unused).
         */
        void onHelpAbout(wxCommandEvent &event);
        
        /**
         * Called when an item control is changed.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onItemChange(wxCommandEvent &event);
        
        /**
         * Called when an item equipped control is changed.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onItemEquippedChange(wxCommandEvent &event);
        
        /**
         * Called in idle time to update the equipped item controls.
         *
         * @param event The triggering wxUpdateUIEvent.
         */
        void onItemEquippedUpdate(wxUpdateUIEvent &event);
        
        /**
         * Called when a map control is changed.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onMapChange(wxCommandEvent &event);
        
        /**
         * Called when a metal door control is changed.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onMetalDoorChange(wxCommandEvent &event);
        
        /**
         * Called when a metroid room control is changed.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onMetroidRoomChange(wxCommandEvent &event);
        
        /**
         * Called when a mini boss control is changed.
         */
        void onMiniBossChange(wxCommandEvent &event);
        
        /**
         * Called when one of the misc game config controls is changed.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onMiscConfigChange(wxCommandEvent &event);
        
        /**
         * Called when one of the misc game status controls is changed.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onMiscStatChange(wxCommandEvent &event);
        
        /**
         * Called when a missile pack control is changed.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onMissilePackChange(wxCommandEvent &event);
        
        /**
         * Called when the missiles spin button control is changed.
         *
         * @param event The triggering wxSpinEvent.
         */
        void onMissilesSpinner(wxSpinEvent &event);
        
        /**
         * Called when the missiles text control is changed.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onMissilesChange(wxCommandEvent &event);
        
        /**
         * Called in idle time to update the notebook control.
         *
         * @param event The triggering wxUpdateUIEvent.
         */
        void onNotebookUpdate(wxUpdateUIEvent &event);
        
        /**
         * Called when a power bomb pack control is changed.
         *
         * @param event The triggerging wxCommandEvent.
         */
        void onPowerBombPackChange(wxCommandEvent &event);
        
        /**
         * Called when the power bombs spin button control is changed.
         *
         * @param event The triggering wxSpinEvent.
         */
        void onPowerBombsSpinner(wxSpinEvent &event);
        
        /**
         * Called when the power bombs text control is changed.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onPowerBombsChange(wxCommandEvent &event);
        
        /**
         * Called when a red door control is changed.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onRedDoorChange(wxCommandEvent &event);
        
        /**
         * Called when a reserve tank control is changed.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onReserveTankChange(wxCommandEvent &event);
        
        /**
         * Called when the save area control is changed.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onSaveAreaChange(wxCommandEvent &event);
        
        /**
         * Called when a save point control is changed.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onSavePointChange(wxCommandEvent &event);
        
        /**
         * Called in idle time to update the save point controls.
         *
         * @param event The triggering wxUpdateUIEvent.
         */
        void onSavePointUpdate(wxUpdateUIEvent &event);
        
        /**
         * Called when a statue control is changed.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onStatueChange(wxCommandEvent &event);
        
        /**
         * Called in idle time to update the statue controls.
         *
         * @param event The triggering wxUpdateUIEvent.
         */
        void onStatueUpdate(wxUpdateUIEvent &event);
        
        /**
         * Called when the status bar timer goes off.
         *
         * @param event The triggering wxTimerEvent (unused).
         */
        void onStatusBarTimer(wxTimerEvent &event);
        
        /**
         * Called when a super missile pack control is changed.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onSuperMissilePackChange(wxCommandEvent &event);
        
        /**
         * Called when the super missiles spin button control is changed.
         *
         * @param event The triggering wxSpinEvent.
         */
        void onSuperMissilesSpinner(wxSpinEvent &event);
        
        /**
         * Called when the super missiles text control is changed.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onSuperMissilesChange(wxCommandEvent &event);
        
        /**
         * Called when this Frame is being closed.
         *
         * @param event The triggering wxCloseEvent.
         */
        void onWindowClosing(wxCloseEvent &event);
        
        /**
         * Called when a yellow door control is changed.
         *
         * @param event The triggering wxCommandEvent.
         */
        void onYellowDoorChange(wxCommandEvent &event);
        
        /**
         * Called when the zebetites control is changed.
         */
        void onZebetitesChange(wxCommandEvent &event);
        
    public:
        /**
         * Creates a new Frame.
         */
        Frame();
        
        /**
         * Opens an SRAM file.
         *
         * @param filename The filename to open.
         */
        void open(const wxString &filename);
    };
    
    inline void Frame::onFileCloseUpdate(wxUpdateUIEvent &event)
        { event.Enable(sram); }
        
    inline void Frame::onFileExit(wxCommandEvent &) { Close(); }
    
    inline void Frame::onGameEndingUpdate(wxUpdateUIEvent &event)
        { event.Enable(sram); }
        
    inline void Frame::onGameGiveUpdate(wxUpdateUIEvent &event)
        { event.Enable(sram); }
}

#endif

