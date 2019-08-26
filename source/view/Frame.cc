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
 
// $Id: Frame.cc,v 1.52 2005/10/17 09:54:45 technoplaza Exp $

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/regex.h>
#include <wx/xrc/xmlres.h>

#include "exceptions/InvalidSRAMDataException.hh"
#include "res/icon32x32.xpm"
#include "model/SRAMFile.hh"
#include "view/Frame.hh"
#include "view/SRAMFileTarget.hh"

using namespace smse;

Frame::Frame() : timer(this, ID_SBTIMER), sram(NULL), ignoreTextEvents(true) {
    SetParent(NULL);
    
    CreateControls();
    GetSizer()->SetSizeHints(this);
    Centre();
    
    // hide the notebook
    XRCCTRL(*this, "IDN_SMSE", wxNotebook)->Show(false);
    
    // stop ignoring text events
    ignoreTextEvents = false;
    
    // add the file drop target
    SetDropTarget(new SRAMFileTarget(this));
}

void Frame::CreateControls() {
    wxXmlResource::Get()->LoadFrame(this, GetParent(), wxT("IDF_SMSE"));
    SetIcon(wxIcon(icon32x32_xpm));
    
    wxTextValidator nv(wxFILTER_NUMERIC);
    
    XRCCTRL(*this, "IDT_GAME_ENERGY", wxTextCtrl)->SetValidator(nv);
    XRCCTRL(*this, "IDT_GAME_RESERVE", wxTextCtrl)->SetValidator(nv);
    XRCCTRL(*this, "IDT_GAME_MISSILES", wxTextCtrl)->SetValidator(nv);
    XRCCTRL(*this, "IDT_GAME_SMISSILES", wxTextCtrl)->SetValidator(nv);
    XRCCTRL(*this, "IDT_GAME_PBOMBS", wxTextCtrl)->SetValidator(nv);
}

bool Frame::close(bool save) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    if (save && !this->save()) {
        return false;
    }
    
    delete sram;
    sram = NULL;
    
    enableMenus();
    
    return true;
}

void Frame::enableMenus() {
    wxMenuBar *mb = GetMenuBar();
    
    mb->FindItem(wxID_CLOSE)->Enable(sram);
    mb->FindItem(wxID_SAVE)->Enable(sram);
    mb->FindItem(wxID_SAVEAS)->Enable(sram);
    
    mb->FindItem(XRCID("IDM_GAME_GAMEA"))->Enable(sram && sram->isValidGame(0));
    mb->FindItem(XRCID("IDM_GAME_GAMEB"))->Enable(sram && sram->isValidGame(1));
    mb->FindItem(XRCID("IDM_GAME_GAMEC"))->Enable(sram && sram->isValidGame(2));
    
    mb->FindItem(XRCID("IDM_GAME_ENDING"))->Enable(sram);
    mb->FindItem(XRCID("IDM_GAME_GIVE"))->Enable(sram);
}

void Frame::load(int game) {
    // ensure we have a valid game
    wxASSERT((game >= 0) && (game < SRAMFile::GAMES));
    
    // ensure we have an open SRAMFile and valid game
    wxASSERT(sram && sram->isValidGame(game));
    
    // select the proper game in the SRAM
    sram->setGame(game);
    
    this->game = game;
    
    try {
        loadGameTab();
        loadMiscTab();
        loadItemsTab();
        loadColorDoorsTab();
        loadSpecialDoorsTab();
        
        enableMenus();
    } catch (InvalidSRAMDataException &e) {
        wxString message(e.what(), wxConvLibc);
        
        wxMessageBox(message, wxT("Error: Invalid Super Metroid SRAM"),
                     wxOK | wxICON_ERROR);
                     
        close();
    }
}

void Frame::loadBosses() {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    // Kraid + Statue
    XRCCTRL(*this, "IDC_GAME_BOSS_KRAID",
        wxCheckBox)->SetValue(sram->getBoss(BOSS_KRAID));
    XRCCTRL(*this, "IDC_GAME_STATUE_KRAID",
        wxCheckBox)->SetValue(sram->getStatue(STATUE_KRAID));
    
    // Phantoon + Statue
    XRCCTRL(*this, "IDC_GAME_BOSS_PHANTOON",
        wxCheckBox)->SetValue(sram->getBoss(BOSS_PHANTOON));
    XRCCTRL(*this, "IDC_GAME_STATUE_PHANTOON",
        wxCheckBox)->SetValue(sram->getStatue(STATUE_PHANTOON));
    
    // Draygon + Statue
    XRCCTRL(*this, "IDC_GAME_BOSS_DRAYGON",
        wxCheckBox)->SetValue(sram->getBoss(BOSS_DRAYGON));
    XRCCTRL(*this, "IDC_GAME_STATUE_DRAYGON",
        wxCheckBox)->SetValue(sram->getStatue(STATUE_DRAYGON));
    
    // Ridley + Statue
    XRCCTRL(*this, "IDC_GAME_BOSS_RIDLEY",
        wxCheckBox)->SetValue(sram->getBoss(BOSS_RIDLEY));
    XRCCTRL(*this, "IDC_GAME_STATUE_RIDLEY",
        wxCheckBox)->SetValue(sram->getStatue(STATUE_RIDLEY));
}

void Frame::loadColorDoorsTab() {
    loadRedDoors();
    loadGreenDoors();
    loadYellowDoors();
}

void Frame::loadControllerConfig() {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    XRCCTRL(*this, "IDC_MISC_DASH",
        wxChoice)->SetSelection(sram->getButton(ACTION_DASH));
    XRCCTRL(*this, "IDC_MISC_JUMP",
        wxChoice)->SetSelection(sram->getButton(ACTION_JUMP));
    XRCCTRL(*this, "IDC_MISC_ITEMCANCEL",
        wxChoice)->SetSelection(sram->getButton(ACTION_ITEMCANCEL));
    XRCCTRL(*this, "IDC_MISC_SHOT",
        wxChoice)->SetSelection(sram->getButton(ACTION_SHOT));
    XRCCTRL(*this, "IDC_MISC_ANGLEUP",
        wxChoice)->SetSelection(sram->getButton(ACTION_ANGLEUP));
    XRCCTRL(*this, "IDC_MISC_ANGLEDOWN",
        wxChoice)->SetSelection(sram->getButton(ACTION_ANGLEDOWN));
    XRCCTRL(*this, "IDC_MISC_ITEMSELECT",
        wxChoice)->SetSelection(sram->getButton(ACTION_ITEMSELECT));
}

void Frame::loadEnergy() {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    ignoreTextEvents = true;
    
    int energy = sram->getEnergy();
    
    // primary energy
    XRCCTRL(*this, "IDT_GAME_ENERGY",
        wxTextCtrl)->SetValue(wxString::Format(wxT("%d"), energy));
    
    // reserve energy
    energy = sram->getReserveEnergy();
    
    XRCCTRL(*this, "IDT_GAME_RESERVE",
        wxTextCtrl)->SetValue(wxString::Format(wxT("%d"), energy));
        
    ignoreTextEvents = false;
}

void Frame::loadEnergyTanks() {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    XRCCTRL(*this, "IDC_ITEMS_ET_C1",
        wxCheckBox)->SetValue(sram->hasEnergyTank(SO_CETANKS));
    XRCCTRL(*this, "IDC_ITEMS_ET_C2",
        wxCheckBox)->SetValue(sram->hasEnergyTank(SO_CETANKS + 1));
    XRCCTRL(*this, "IDC_ITEMS_ET_BS1",
        wxCheckBox)->SetValue(sram->hasEnergyTank(SO_BSETANKS));
    XRCCTRL(*this, "IDC_ITEMS_ET_BS2",
        wxCheckBox)->SetValue(sram->hasEnergyTank(SO_BSETANKS + 1));
    XRCCTRL(*this, "IDC_ITEMS_ET_BS3",
        wxCheckBox)->SetValue(sram->hasEnergyTank(SO_BSETANKS + 2));
    XRCCTRL(*this, "IDC_ITEMS_ET_BS4",
        wxCheckBox)->SetValue(sram->hasEnergyTank(SO_BSETANKS + 3));
    XRCCTRL(*this, "IDC_ITEMS_ET_BS5",
        wxCheckBox)->SetValue(sram->hasEnergyTank(SO_BSETANKS + 4));
    XRCCTRL(*this, "IDC_ITEMS_ET_NF1",
        wxCheckBox)->SetValue(sram->hasEnergyTank(SO_NFETANKS));
    XRCCTRL(*this, "IDC_ITEMS_ET_NF2",
        wxCheckBox)->SetValue(sram->hasEnergyTank(SO_NFETANKS + 1));
    XRCCTRL(*this, "IDC_ITEMS_ET_NF3",
        wxCheckBox)->SetValue(sram->hasEnergyTank(SO_NFETANKS + 2));
    XRCCTRL(*this, "IDC_ITEMS_ET_NF4",
        wxCheckBox)->SetValue(sram->hasEnergyTank(SO_NFETANKS + 3));
    XRCCTRL(*this, "IDC_ITEMS_ET_WS",
        wxCheckBox)->SetValue(sram->hasEnergyTank(SO_WSETANKS));
    XRCCTRL(*this, "IDC_ITEMS_ET_M1",
        wxCheckBox)->SetValue(sram->hasEnergyTank(SO_METANKS));
    XRCCTRL(*this, "IDC_ITEMS_ET_M2",
        wxCheckBox)->SetValue(sram->hasEnergyTank(SO_METANKS + 1));
}

void Frame::loadEyeDoors() {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    XRCCTRL(*this, "IDC_EYEDOOR_KRAID",
        wxCheckBox)->SetValue(sram->getDoor(ED_KRAID));
    XRCCTRL(*this, "IDC_EYEDOOR_RIDLEY",
        wxCheckBox)->SetValue(sram->getDoor(ED_RIDLEY));
    XRCCTRL(*this, "IDC_EYEDOOR_PHANTOON",
        wxCheckBox)->SetValue(sram->getDoor(ED_PHANTOON));
    XRCCTRL(*this, "IDC_EYEDOOR_DRAYGON",
        wxCheckBox)->SetValue(sram->getDoor(ED_DRAYGON));
    XRCCTRL(*this, "IDC_EYEDOOR_T",
        wxCheckBox)->SetValue(sram->getDoor(ED_T));
}

void Frame::loadGameConfig() {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    XRCCTRL(*this, "IDRB_MISC_LANGUAGE",
        wxRadioBox)->SetSelection(sram->getMiscBit(MB_LANGUAGE));
    XRCCTRL(*this, "IDRB_MISC_MOONWALK",
        wxRadioBox)->SetSelection(sram->getMiscBit(MB_MOONWALK));
    XRCCTRL(*this, "IDRB_MISC_ICONCANCEL",
        wxRadioBox)->SetSelection(sram->getMiscBit(MB_ICONCANCEL));
}

void Frame::loadGameStatus() {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    XRCCTRL(*this, "IDC_MISC_MTUBEBROKEN",
        wxCheckBox)->SetValue(sram->getMiscBit(MB_MTUBEBROKEN));
    XRCCTRL(*this, "IDC_MISC_RESCUEDANIMALS",
        wxCheckBox)->SetValue(sram->getMiscBit(MB_RESCUEDANIMALS));
}

void Frame::loadGameTab() {
    loadEnergy();
    loadInventory();
    
    loadBosses();
    loadMiniBosses();
    loadMetroidRooms();
    loadZebetites();
    
    loadSavePoint();
    loadMaps();
}

void Frame::loadGameTime() {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    ignoreTextEvents = true;
    
    int hours = sram->getGameHours();
    int minutes = sram->getGameMinutes();
    
    XRCCTRL(*this, "IDT_MISC_GAMETIME",
        wxTextCtrl)->SetValue(wxString::Format(wxT("%d:%02d"), hours, minutes));
        
    ignoreTextEvents = false;
}

void Frame::loadGreenDoors() {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    // Crateria Green Doors
    XRCCTRL(*this, "IDC_GREENDOOR_C1",
        wxCheckBox)->SetValue(sram->getDoor(GD_C1));
    XRCCTRL(*this, "IDC_GREENDOOR_C_WS",
        wxCheckBox)->SetValue(sram->getDoor(GD_C_WS));
        
    // Brinstar Green Doors
    XRCCTRL(*this, "IDC_GREENDOOR_BS1",
        wxCheckBox)->SetValue(sram->getDoor(GD_BS1));
    XRCCTRL(*this, "IDC_GREENDOOR_BS2",
        wxCheckBox)->SetValue(sram->getDoor(GD_BS2));
    XRCCTRL(*this, "IDC_GREENDOOR_BS3",
        wxCheckBox)->SetValue(sram->getDoor(GD_BS3));
    XRCCTRL(*this, "IDC_GREENDOOR_BS4",
        wxCheckBox)->SetValue(sram->getDoor(GD_BS4));
    XRCCTRL(*this, "IDC_GREENDOOR_BS5",
        wxCheckBox)->SetValue(sram->getDoor(GD_BS5));
    XRCCTRL(*this, "IDC_GREENDOOR_BS6",
        wxCheckBox)->SetValue(sram->getDoor(GD_BS6));
    XRCCTRL(*this, "IDC_GREENDOOR_BS7",
        wxCheckBox)->SetValue(sram->getDoor(GD_BS7));
    XRCCTRL(*this, "IDC_GREENDOOR_BS8",
        wxCheckBox)->SetValue(sram->getDoor(GD_BS8));
    XRCCTRL(*this, "IDC_GREENDOOR_BS_SPAZER",
        wxCheckBox)->SetValue(sram->getDoor(GD_BS_SPAZER));
    XRCCTRL(*this, "IDC_GREENDOOR_BS_SPORESPAWNEXIT",
        wxCheckBox)->SetValue(sram->getDoor(GD_BS_SPORESPAWNEXIT));
        
    // Norfair Green Doors
    XRCCTRL(*this, "IDC_GREENDOOR_NF1",
        wxCheckBox)->SetValue(sram->getDoor(GD_NF1));
    XRCCTRL(*this, "IDC_GREENDOOR_NF2",
        wxCheckBox)->SetValue(sram->getDoor(GD_NF2));
    XRCCTRL(*this, "IDC_GREENDOOR_NF3",
        wxCheckBox)->SetValue(sram->getDoor(GD_NF3));
    XRCCTRL(*this, "IDC_GREENDOOR_NF4",
        wxCheckBox)->SetValue(sram->getDoor(GD_NF4));
    XRCCTRL(*this, "IDC_GREENDOOR_NF_ICEBEAM",
        wxCheckBox)->SetValue(sram->getDoor(GD_NF_ICEBEAM));
    XRCCTRL(*this, "IDC_GREENDOOR_NF_SPEEDBOOSTER",
        wxCheckBox)->SetValue(sram->getDoor(GD_NF_SPEEDBOOSTER));
        
    // Wrecked Ship Green Door
    XRCCTRL(*this, "IDC_GREENDOOR_WS",
        wxCheckBox)->SetValue(sram->getDoor(GD_WS));
        
    // Maridia Green Doors
    XRCCTRL(*this, "IDC_GREENDOOR_M1",
        wxCheckBox)->SetValue(sram->getDoor(GD_M1));
    XRCCTRL(*this, "IDC_GREENDOOR_M2",
        wxCheckBox)->SetValue(sram->getDoor(GD_M2));
    XRCCTRL(*this, "IDC_GREENDOOR_M3",
        wxCheckBox)->SetValue(sram->getDoor(GD_M3));
    XRCCTRL(*this, "IDC_GREENDOOR_M_DRAYGON",
        wxCheckBox)->SetValue(sram->getDoor(GD_M_DRAYGON));
}

void Frame::loadInventory() {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    ignoreTextEvents = true;
    
    int missiles = sram->getMissiles();
    
    // missiles
    XRCCTRL(*this, "IDT_GAME_MISSILES",
        wxTextCtrl)->SetValue(wxString::Format(wxT("%d"), missiles));
    XRCCTRL(*this, "IDSB_GAME_MISSILES", wxSpinButton)->SetValue(missiles);
    
    // super missiles
    missiles = sram->getSuperMissiles();
    
    XRCCTRL(*this, "IDT_GAME_SMISSILES",
        wxTextCtrl)->SetValue(wxString::Format(wxT("%d"), missiles));
    XRCCTRL(*this, "IDSB_GAME_SMISSILES", wxSpinButton)->SetValue(missiles);
        
    // power bombs
    int bombs = sram->getPowerBombs();
    
    XRCCTRL(*this, "IDT_GAME_PBOMBS",
        wxTextCtrl)->SetValue(wxString::Format(wxT("%d"), bombs));
    XRCCTRL(*this, "IDSB_GAME_PBOMBS", wxSpinButton)->SetValue(bombs);
    
    ignoreTextEvents = false;
}

void Frame::loadItems() {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    XRCCTRL(*this, "IDC_ITEMS_MORPHINGBALL",
        wxCheckBox)->SetValue(sram->hasItem(ITEM_MORPHINGBALL));
    XRCCTRL(*this, "IDC_ITEMS_MORPHINGBALL_EQUIPPED",
        wxCheckBox)->SetValue(sram->isItemEquipped(ITEM_MORPHINGBALL));
    
    XRCCTRL(*this, "IDC_ITEMS_BOMBS",
        wxCheckBox)->SetValue(sram->hasItem(ITEM_BOMBS));
    XRCCTRL(*this, "IDC_ITEMS_BOMBS_EQUIPPED",
        wxCheckBox)->SetValue(sram->isItemEquipped(ITEM_BOMBS));
        
    XRCCTRL(*this, "IDC_ITEMS_SPRINGBALL",
        wxCheckBox)->SetValue(sram->hasItem(ITEM_SPRINGBALL));
    XRCCTRL(*this, "IDC_ITEMS_SPRINGBALL_EQUIPPED",
        wxCheckBox)->SetValue(sram->isItemEquipped(ITEM_SPRINGBALL));
        
    XRCCTRL(*this, "IDC_ITEMS_SCREWATTACK",
        wxCheckBox)->SetValue(sram->hasItem(ITEM_SCREWATTACK));
    XRCCTRL(*this, "IDC_ITEMS_SCREWATTACK_EQUIPPED",
        wxCheckBox)->SetValue(sram->isItemEquipped(ITEM_SCREWATTACK));
        
    XRCCTRL(*this, "IDC_ITEMS_VARIASUIT",
        wxCheckBox)->SetValue(sram->hasItem(ITEM_VARIASUIT));
    XRCCTRL(*this, "IDC_ITEMS_VARIASUIT_EQUIPPED",
        wxCheckBox)->SetValue(sram->isItemEquipped(ITEM_VARIASUIT));
        
    XRCCTRL(*this, "IDC_ITEMS_GRAVITYSUIT",
        wxCheckBox)->SetValue(sram->hasItem(ITEM_GRAVITYSUIT));
    XRCCTRL(*this, "IDC_ITEMS_GRAVITYSUIT_EQUIPPED",
        wxCheckBox)->SetValue(sram->isItemEquipped(ITEM_GRAVITYSUIT));
        
    XRCCTRL(*this, "IDC_ITEMS_HJBOOTS",
        wxCheckBox)->SetValue(sram->hasItem(ITEM_HJBOOTS));
    XRCCTRL(*this, "IDC_ITEMS_HJBOOTS_EQUIPPED",
        wxCheckBox)->SetValue(sram->isItemEquipped(ITEM_HJBOOTS));
        
    XRCCTRL(*this, "IDC_ITEMS_SPACEJUMP",
        wxCheckBox)->SetValue(sram->hasItem(ITEM_SPACEJUMP));
    XRCCTRL(*this, "IDC_ITEMS_SPACEJUMP_EQUIPPED",
        wxCheckBox)->SetValue(sram->isItemEquipped(ITEM_SPACEJUMP));
        
    XRCCTRL(*this, "IDC_ITEMS_SPEEDBOOSTER",
        wxCheckBox)->SetValue(sram->hasItem(ITEM_SPEEDBOOSTER));
    XRCCTRL(*this, "IDC_ITEMS_SPEEDBOOSTER_EQUIPPED",
        wxCheckBox)->SetValue(sram->isItemEquipped(ITEM_SPEEDBOOSTER));
        
    XRCCTRL(*this, "IDC_ITEMS_CHARGEBEAM",
        wxCheckBox)->SetValue(sram->hasItem(ITEM_CHARGEBEAM));
    XRCCTRL(*this, "IDC_ITEMS_CHARGEBEAM_EQUIPPED",
        wxCheckBox)->SetValue(sram->isItemEquipped(ITEM_CHARGEBEAM));
        
    XRCCTRL(*this, "IDC_ITEMS_ICEBEAM",
        wxCheckBox)->SetValue(sram->hasItem(ITEM_ICEBEAM));
    XRCCTRL(*this, "IDC_ITEMS_ICEBEAM_EQUIPPED",
        wxCheckBox)->SetValue(sram->isItemEquipped(ITEM_ICEBEAM));
        
    XRCCTRL(*this, "IDC_ITEMS_WAVEBEAM",
        wxCheckBox)->SetValue(sram->hasItem(ITEM_WAVEBEAM));
    XRCCTRL(*this, "IDC_ITEMS_WAVEBEAM_EQUIPPED",
        wxCheckBox)->SetValue(sram->isItemEquipped(ITEM_WAVEBEAM));
        
    XRCCTRL(*this, "IDC_ITEMS_SPAZER",
        wxCheckBox)->SetValue(sram->hasItem(ITEM_SPAZER));
    XRCCTRL(*this, "IDC_ITEMS_SPAZER_EQUIPPED",
        wxCheckBox)->SetValue(sram->isItemEquipped(ITEM_SPAZER));
        
    XRCCTRL(*this, "IDC_ITEMS_PLASMA",
        wxCheckBox)->SetValue(sram->hasItem(ITEM_PLASMA));
    XRCCTRL(*this, "IDC_ITEMS_PLASMA_EQUIPPED",
        wxCheckBox)->SetValue(sram->isItemEquipped(ITEM_PLASMA));
        
    XRCCTRL(*this, "IDC_ITEMS_GRAPPLINGBEAM",
        wxCheckBox)->SetValue(sram->hasItem(ITEM_GRAPPLINGBEAM));
        
    XRCCTRL(*this, "IDC_ITEMS_XRAYSCOPE",
        wxCheckBox)->SetValue(sram->hasItem(ITEM_XRAYSCOPE));
}

void Frame::loadItemsTab() {
    loadItems();
    
    loadMissilePacks();
    loadSuperMissilePacks();
    loadPowerBombPacks();
    
    loadEnergyTanks();
    loadReserveTanks();
}

void Frame::loadMaps() {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    XRCCTRL(*this, "IDC_GAME_MAP_C",
        wxCheckBox)->SetValue(sram->hasMap(AREA_C));
    XRCCTRL(*this, "IDC_GAME_MAP_BS",
        wxCheckBox)->SetValue(sram->hasMap(AREA_BS));
    XRCCTRL(*this, "IDC_GAME_MAP_NF",
        wxCheckBox)->SetValue(sram->hasMap(AREA_NF));
    XRCCTRL(*this, "IDC_GAME_MAP_WS",
        wxCheckBox)->SetValue(sram->hasMap(AREA_WS));
    XRCCTRL(*this, "IDC_GAME_MAP_M",
        wxCheckBox)->SetValue(sram->hasMap(AREA_M));
    XRCCTRL(*this, "IDC_GAME_MAP_T",
        wxCheckBox)->SetValue(sram->hasMap(AREA_T));
}

void Frame::loadMetalDoors() {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    // Crateria Metal Door
    XRCCTRL(*this, "IDC_METALDOOR_C_BOMBSEXIT",
        wxCheckBox)->SetValue(sram->getDoor(MD_C_BOMBSEXIT));
        
    // Brinstar Metal Doors
    XRCCTRL(*this, "IDC_METALDOOR_BS1",
        wxCheckBox)->SetValue(sram->getDoor(MD_BS1));
    XRCCTRL(*this, "IDC_METALDOOR_BS2",
        wxCheckBox)->SetValue(sram->getDoor(MD_BS2));
    XRCCTRL(*this, "IDC_METALDOOR_BS3",
        wxCheckBox)->SetValue(sram->getDoor(MD_BS3));
    XRCCTRL(*this, "IDC_METALDOOR_BS4",
        wxCheckBox)->SetValue(sram->getDoor(MD_BS4));
    XRCCTRL(*this, "IDC_METALDOOR_BS5",
        wxCheckBox)->SetValue(sram->getDoor(MD_BS5));
    XRCCTRL(*this, "IDC_METALDOOR_BS6",
        wxCheckBox)->SetValue(sram->getDoor(MD_BS6));
    XRCCTRL(*this, "IDC_METALDOOR_BS7",
        wxCheckBox)->SetValue(sram->getDoor(MD_BS7));
    XRCCTRL(*this, "IDC_METALDOOR_BS8",
        wxCheckBox)->SetValue(sram->getDoor(MD_BS8));
    XRCCTRL(*this, "IDC_METALDOOR_BS9",
        wxCheckBox)->SetValue(sram->getDoor(MD_BS9));
    XRCCTRL(*this, "IDC_METALDOOR_BS_MAPEXIT",
        wxCheckBox)->SetValue(sram->getDoor(MD_BS_MAPEXIT));
    XRCCTRL(*this, "IDC_METALDOOR_BS_OLDTLEFT",
        wxCheckBox)->SetValue(sram->getDoor(MD_BS_OLDTLEFT));
    XRCCTRL(*this, "IDC_METALDOOR_BS_OLDTRIGHT",
        wxCheckBox)->SetValue(sram->getDoor(MD_BS_OLDTRIGHT));
    XRCCTRL(*this, "IDC_METALDOOR_BS_MINIKRAIDLEFT",
        wxCheckBox)->SetValue(sram->getDoor(MD_BS_MINIKRAIDLEFT));
    XRCCTRL(*this, "IDC_METALDOOR_BS_MINIKRAIDRIGHT",
        wxCheckBox)->SetValue(sram->getDoor(MD_BS_MINIKRAIDRIGHT));
    XRCCTRL(*this, "IDC_METALDOOR_BS_VARIASUIT",
        wxCheckBox)->SetValue(sram->getDoor(MD_BS_VARIASUIT));
    XRCCTRL(*this, "IDC_METALDOOR_BS_KRAIDEXIT",
        wxCheckBox)->SetValue(sram->getDoor(MD_BS_KRAIDEXIT));
        
    // Norfair Metal Doors
    XRCCTRL(*this, "IDC_METALDOOR_NF_HJBOOTSEXIT",
        wxCheckBox)->SetValue(sram->getDoor(MD_NF_HJBOOTSEXIT));
    XRCCTRL(*this, "IDC_METALDOOR_NF_CROCOMIREEXIT",
        wxCheckBox)->SetValue(sram->getDoor(MD_NF_CROCOMIREEXIT));
    XRCCTRL(*this, "IDC_METALDOOR_NF_SCREWATTACK",
        wxCheckBox)->SetValue(sram->getDoor(MD_NF_SCREWATTACK));
    XRCCTRL(*this, "IDC_METALDOOR_NF_GOLDSPACEPIRATES",
        wxCheckBox)->SetValue(sram->getDoor(MD_NF_GOLDSPACEPIRATES));
    XRCCTRL(*this, "IDC_METALDOOR_NF_RIDLEYLEFT",
        wxCheckBox)->SetValue(sram->getDoor(MD_NF_RIDLEYLEFT));
    XRCCTRL(*this, "IDC_METALDOOR_NF_RIDLEYEXIT",
        wxCheckBox)->SetValue(sram->getDoor(MD_NF_RIDLEYEXIT));
        
    // Wrecked Ship Metal Doors
    XRCCTRL(*this, "IDC_METALDOOR_WS1",
        wxCheckBox)->SetValue(sram->getDoor(MD_WS1));
    XRCCTRL(*this, "IDC_METALDOOR_WS2",
        wxCheckBox)->SetValue(sram->getDoor(MD_WS2));
    XRCCTRL(*this, "IDC_METALDOOR_WS3",
        wxCheckBox)->SetValue(sram->getDoor(MD_WS3));
    XRCCTRL(*this, "IDC_METALDOOR_WS4",
        wxCheckBox)->SetValue(sram->getDoor(MD_WS4));
    XRCCTRL(*this, "IDC_METALDOOR_WS_PHANTOONEXIT",
        wxCheckBox)->SetValue(sram->getDoor(MD_WS_PHANTOONEXIT));
        
    // Maridia Metal Doors
    XRCCTRL(*this, "IDC_METALDOOR_M1",
        wxCheckBox)->SetValue(sram->getDoor(MD_M1));
    XRCCTRL(*this, "IDC_METALDOOR_M2",
        wxCheckBox)->SetValue(sram->getDoor(MD_M2));
    XRCCTRL(*this, "IDC_METALDOOR_M_BOTWOONEXIT",
        wxCheckBox)->SetValue(sram->getDoor(MD_M_BOTWOONEXIT));
    XRCCTRL(*this, "IDC_METALDOOR_M_SPACEJUMP",
        wxCheckBox)->SetValue(sram->getDoor(MD_M_SPACEJUMP));
    XRCCTRL(*this, "IDC_METALDOOR_M_DRAYGONEXIT",
        wxCheckBox)->SetValue(sram->getDoor(MD_M_DRAYGONEXIT));
    XRCCTRL(*this, "IDC_METALDOOR_M_PLASMA",
        wxCheckBox)->SetValue(sram->getDoor(MD_M_PLASMA));
    XRCCTRL(*this, "IDC_METALDOOR_M_PLASMAEXIT",
        wxCheckBox)->SetValue(sram->getDoor(MD_M_PLASMAEXIT));
        
    // Tourian Metal Doors
    XRCCTRL(*this, "IDC_METALDOOR_T1",
        wxCheckBox)->SetValue(sram->getDoor(MD_T1));
    XRCCTRL(*this, "IDC_METALDOOR_T2",
        wxCheckBox)->SetValue(sram->getDoor(MD_T2));
    XRCCTRL(*this, "IDC_METALDOOR_T3",
        wxCheckBox)->SetValue(sram->getDoor(MD_T3));
    XRCCTRL(*this, "IDC_METALDOOR_T4",
        wxCheckBox)->SetValue(sram->getDoor(MD_T4));
    XRCCTRL(*this, "IDC_METALDOOR_T5",
        wxCheckBox)->SetValue(sram->getDoor(MD_T5));
}

void Frame::loadMetroidRooms() {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    XRCCTRL(*this, "IDC_GAME_METROIDROOM1",
        wxCheckBox)->SetValue(sram->getMetroidRoom(MR1));
    XRCCTRL(*this, "IDC_GAME_METROIDROOM2",
        wxCheckBox)->SetValue(sram->getMetroidRoom(MR2));
    XRCCTRL(*this, "IDC_GAME_METROIDROOM3",
        wxCheckBox)->SetValue(sram->getMetroidRoom(MR3));
    XRCCTRL(*this, "IDC_GAME_METROIDROOM4",
        wxCheckBox)->SetValue(sram->getMetroidRoom(MR4));
}

void Frame::loadMiniBosses() {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    // Silver Torizo
    XRCCTRL(*this, "IDC_GAME_MINIBOSS_SILVERTORIZO",
        wxCheckBox)->SetValue(sram->getBoss(BOSS_SILVERTORIZO));
    
    // Spore Spawn
    XRCCTRL(*this, "IDC_GAME_MINIBOSS_SPORESPAWN",
        wxCheckBox)->SetValue(sram->getBoss(BOSS_SPORESPAWN));
    
    // Botwoon
    XRCCTRL(*this, "IDC_GAME_MINIBOSS_BOTWOON",
        wxCheckBox)->SetValue(sram->getBoss(BOSS_BOTWOON));
    
    // Crocomire
    XRCCTRL(*this, "IDC_GAME_MINIBOSS_CROCOMIRE",
        wxCheckBox)->SetValue(sram->getBoss(BOSS_CROCOMIRE));
    
    // Golden Torizo
    XRCCTRL(*this, "IDC_GAME_MINIBOSS_GOLDENTORIZO",
        wxCheckBox)->SetValue(sram->getBoss(BOSS_GOLDENTORIZO));
}

void Frame::loadMiscTab() {
    loadGameTime();
    loadGameStatus();
    loadControllerConfig();
    loadGameConfig();
}

void Frame::loadMissilePacks() {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    // Crateria Missile Packs
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_C1",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_CMISSILEPACKS));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_C2",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_CMISSILEPACKS + 1));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_C3",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_CMISSILEPACKS + 2));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_C4",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_CMISSILEPACKS + 3));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_C5",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_CMISSILEPACKS + 4));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_C6",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_CMISSILEPACKS + 5));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_C7",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_CMISSILEPACKS + 6));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_C8",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_CMISSILEPACKS + 7));
        
    // Brinstar Missile Packs
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_BS1",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_BSMISSILEPACKS ));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_BS2",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_BSMISSILEPACKS + 1));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_BS3",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_BSMISSILEPACKS + 2));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_BS4",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_BSMISSILEPACKS + 3));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_BS5",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_BSMISSILEPACKS + 4));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_BS6",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_BSMISSILEPACKS + 5));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_BS7",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_BSMISSILEPACKS + 6));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_BS8",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_BSMISSILEPACKS + 7));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_BS9",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_BSMISSILEPACKS + 8));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_BS10",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_BSMISSILEPACKS + 9));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_BS11",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_BSMISSILEPACKS + 10));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_BS12",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_BSMISSILEPACKS + 11));
        
    // Norfair Missile Packs
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_NF1",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_NFMISSILEPACKS));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_NF2",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_NFMISSILEPACKS + 1));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_NF3",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_NFMISSILEPACKS + 2));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_NF4",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_NFMISSILEPACKS + 3));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_NF5",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_NFMISSILEPACKS + 4));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_NF6",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_NFMISSILEPACKS + 5));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_NF7",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_NFMISSILEPACKS + 6));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_NF8",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_NFMISSILEPACKS + 7));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_NF9",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_NFMISSILEPACKS + 8));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_NF10",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_NFMISSILEPACKS + 9));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_NF11",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_NFMISSILEPACKS + 10));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_NF12",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_NFMISSILEPACKS + 11));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_NF13",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_NFMISSILEPACKS + 12));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_NF14",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_NFMISSILEPACKS + 13));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_NF15",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_NFMISSILEPACKS + 14));
        
    // Wrecked Ship Missile Packs
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_WS1",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_WSMISSILEPACKS));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_WS2",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_WSMISSILEPACKS + 1));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_WS3",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_WSMISSILEPACKS + 2));
        
    // Maridia Missile Packs
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_M1",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_MMISSILEPACKS));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_M2",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_MMISSILEPACKS + 1));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_M3",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_MMISSILEPACKS + 2));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_M4",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_MMISSILEPACKS + 3));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_M5",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_MMISSILEPACKS + 4));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_M6",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_MMISSILEPACKS + 5));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_M7",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_MMISSILEPACKS + 6));
    XRCCTRL(*this, "IDC_PICKUPS_MISSILE_M8",
        wxCheckBox)->SetValue(sram->hasMissilePack(SO_MMISSILEPACKS + 7));
}

void Frame::loadPowerBombPacks() {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    // Crateria Power Bomb Pack
    XRCCTRL(*this, "IDC_PICKUPS_PBOMB_C",
        wxCheckBox)->SetValue(sram->hasPowerBombPack(SO_CPBOMBPACKS));
        
    // Brinstar Power Bomb Packs
    XRCCTRL(*this, "IDC_PICKUPS_PBOMB_BS1",
        wxCheckBox)->SetValue(sram->hasPowerBombPack(SO_BSPBOMBPACKS));
    XRCCTRL(*this, "IDC_PICKUPS_PBOMB_BS2",
        wxCheckBox)->SetValue(sram->hasPowerBombPack(SO_BSPBOMBPACKS + 1));
    XRCCTRL(*this, "IDC_PICKUPS_PBOMB_BS3",
        wxCheckBox)->SetValue(sram->hasPowerBombPack(SO_BSPBOMBPACKS + 2));
    XRCCTRL(*this, "IDC_PICKUPS_PBOMB_BS4",
        wxCheckBox)->SetValue(sram->hasPowerBombPack(SO_BSPBOMBPACKS + 3));
    XRCCTRL(*this, "IDC_PICKUPS_PBOMB_BS5",
        wxCheckBox)->SetValue(sram->hasPowerBombPack(SO_BSPBOMBPACKS + 4));
        
    // Norfair Power Bombs Packs
    XRCCTRL(*this, "IDC_PICKUPS_PBOMB_NF1",
        wxCheckBox)->SetValue(sram->hasPowerBombPack(SO_NFPBOMBPACKS));
    XRCCTRL(*this, "IDC_PICKUPS_PBOMB_NF2",
        wxCheckBox)->SetValue(sram->hasPowerBombPack(SO_NFPBOMBPACKS + 1));
    XRCCTRL(*this, "IDC_PICKUPS_PBOMB_NF3",
        wxCheckBox)->SetValue(sram->hasPowerBombPack(SO_NFPBOMBPACKS + 2));
        
    // Maridia Power Bomb Pack
    XRCCTRL(*this, "IDC_PICKUPS_PBOMB_M",
        wxCheckBox)->SetValue(sram->hasPowerBombPack(SO_MPBOMBPACKS));
}

void Frame::loadRedDoors() {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    XRCCTRL(*this, "IDC_REDDOOR_C_MAP",
        wxCheckBox)->SetValue(sram->getDoor(RD_C_MAP));
    XRCCTRL(*this, "IDC_REDDOOR_C_BOMBS",
        wxCheckBox)->SetValue(sram->getDoor(RD_C_BOMBS));
    XRCCTRL(*this, "IDC_REDDOOR_C_TELEVATOR",
        wxCheckBox)->SetValue(sram->getDoor(RD_C_TELEVATOR));
        
    XRCCTRL(*this, "IDC_REDDOOR_BS1",
        wxCheckBox)->SetValue(sram->getDoor(RD_BS1));
    XRCCTRL(*this, "IDC_REDDOOR_BS2",
        wxCheckBox)->SetValue(sram->getDoor(RD_BS2));
    XRCCTRL(*this, "IDC_REDDOOR_BS3",
        wxCheckBox)->SetValue(sram->getDoor(RD_BS3));
    XRCCTRL(*this, "IDC_REDDOOR_BS4",
        wxCheckBox)->SetValue(sram->getDoor(RD_BS4));
    XRCCTRL(*this, "IDC_REDDOOR_BS5",
        wxCheckBox)->SetValue(sram->getDoor(RD_BS5));
    XRCCTRL(*this, "IDC_REDDOOR_BS6",
        wxCheckBox)->SetValue(sram->getDoor(RD_BS6));
    XRCCTRL(*this, "IDC_REDDOOR_BS_MAP",
        wxCheckBox)->SetValue(sram->getDoor(RD_BS_MAP));
    XRCCTRL(*this, "IDC_REDDOOR_BS_RTANK",
        wxCheckBox)->SetValue(sram->getDoor(RD_BS_RTANK));
    XRCCTRL(*this, "IDC_REDDOOR_BS_SPORESPAWN",
        wxCheckBox)->SetValue(sram->getDoor(RD_BS_SPORESPAWN));
    XRCCTRL(*this, "IDC_REDDOOR_BS_XRAYSCOPE",
        wxCheckBox)->SetValue(sram->getDoor(RD_BS_XRAYSCOPE));
        
    XRCCTRL(*this, "IDC_REDDOOR_NF1",
        wxCheckBox)->SetValue(sram->getDoor(RD_NF1));
    XRCCTRL(*this, "IDC_REDDOOR_NF2",
        wxCheckBox)->SetValue(sram->getDoor(RD_NF2));
    XRCCTRL(*this, "IDC_REDDOOR_NF3",
        wxCheckBox)->SetValue(sram->getDoor(RD_NF3));
    XRCCTRL(*this, "IDC_REDDOOR_NF4",
        wxCheckBox)->SetValue(sram->getDoor(RD_NF4));
    XRCCTRL(*this, "IDC_REDDOOR_NF_HJBOOTS",
        wxCheckBox)->SetValue(sram->getDoor(RD_NF_HJBOOTS));
    XRCCTRL(*this, "IDC_REDDOOR_NF_SPEEDBOOSTER",
        wxCheckBox)->SetValue(sram->getDoor(RD_NF_SPEEDBOOSTER));
    XRCCTRL(*this, "IDC_REDDOOR_NF_WAVEBEAM",
        wxCheckBox)->SetValue(sram->getDoor(RD_NF_WAVEBEAM));
        
    XRCCTRL(*this, "IDC_REDDOOR_WS_RTANK",
        wxCheckBox)->SetValue(sram->getDoor(RD_WS_RTANK));
        
    XRCCTRL(*this, "IDC_REDDOOR_M1",
        wxCheckBox)->SetValue(sram->getDoor(RD_M1));
    XRCCTRL(*this, "IDC_REDDOOR_M2",
        wxCheckBox)->SetValue(sram->getDoor(RD_M2));
    XRCCTRL(*this, "IDC_REDDOOR_M3",
        wxCheckBox)->SetValue(sram->getDoor(RD_M3));
    XRCCTRL(*this, "IDC_REDDOOR_M4",
        wxCheckBox)->SetValue(sram->getDoor(RD_M4));
    XRCCTRL(*this, "IDC_REDDOOR_M5",
        wxCheckBox)->SetValue(sram->getDoor(RD_M5));
    XRCCTRL(*this, "IDC_REDDOOR_M6",
        wxCheckBox)->SetValue(sram->getDoor(RD_M6));
    XRCCTRL(*this, "IDC_REDDOOR_M_MAP",
        wxCheckBox)->SetValue(sram->getDoor(RD_M_MAP));
        
    XRCCTRL(*this, "IDC_REDDOOR_T1",
        wxCheckBox)->SetValue(sram->getDoor(RD_T1));
    XRCCTRL(*this, "IDC_REDDOOR_T_MB",
        wxCheckBox)->SetValue(sram->getDoor(RD_T_MB));
}

void Frame::loadReserveTanks() {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    XRCCTRL(*this, "IDC_ITEMS_RTANK_BS",
        wxCheckBox)->SetValue(sram->hasReserveTank(SO_BSRTANK));
    XRCCTRL(*this, "IDC_ITEMS_RTANK_NF",
        wxCheckBox)->SetValue(sram->hasReserveTank(SO_NFRTANK));
    XRCCTRL(*this, "IDC_ITEMS_RTANK_WS",
        wxCheckBox)->SetValue(sram->hasReserveTank(SO_WSRTANK));
    XRCCTRL(*this, "IDC_ITEMS_RTANK_M",
        wxCheckBox)->SetValue(sram->hasReserveTank(SO_MRTANK));
}

void Frame::loadSavePoint() {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    std::pair<enum Area, int> save = sram->getSavePoint();
    XRCCTRL(*this, "IDRB_GAME_SAVEAREA", wxRadioBox)->SetSelection(save.first);
    
    switch (save.second) {
        case 0:
            XRCCTRL(*this, "IDR_GAME_SP1", wxRadioButton)->SetValue(true);
            break;
        
        case 1:
            XRCCTRL(*this, "IDR_GAME_SP2", wxRadioButton)->SetValue(true);
            break;
            
        case 2:
            XRCCTRL(*this, "IDR_GAME_SP3", wxRadioButton)->SetValue(true);
            break;
            
        case 3:
            XRCCTRL(*this, "IDR_GAME_SP4", wxRadioButton)->SetValue(true);
            break;
            
        case 4:
            XRCCTRL(*this, "IDR_GAME_SP5", wxRadioButton)->SetValue(true);
            break;
            
        case 5:
            XRCCTRL(*this, "IDR_GAME_SP6", wxRadioButton)->SetValue(true);
            break;
        
        default:
            // this should NEVER happen
            throw InvalidSRAMDataException("Invalid save point");
    }
}

void Frame::loadSpecialDoorsTab() {
    loadEyeDoors();
    loadMetalDoors();
}

void Frame::loadSuperMissilePacks() {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    // Crateria Super Missile Pack
    XRCCTRL(*this, "IDC_PICKUPS_SMISSILE_C", wxCheckBox)->
        SetValue(sram->hasSuperMissilePack(SO_CSMISSILEPACKS));
        
    // Brinstar Super Missile Packs
    XRCCTRL(*this, "IDC_PICKUPS_SMISSILE_BS1", wxCheckBox)->
        SetValue(sram->hasSuperMissilePack(SO_BSSMISSILEPACKS));
    XRCCTRL(*this, "IDC_PICKUPS_SMISSILE_BS2", wxCheckBox)->
        SetValue(sram->hasSuperMissilePack(SO_BSSMISSILEPACKS + 1));
    XRCCTRL(*this, "IDC_PICKUPS_SMISSILE_BS3", wxCheckBox)->
        SetValue(sram->hasSuperMissilePack(SO_BSSMISSILEPACKS + 2));
        
    // Norfair Super Missile Pack
    XRCCTRL(*this, "IDC_PICKUPS_SMISSILE_NF", wxCheckBox)->
        SetValue(sram->hasSuperMissilePack(SO_NFSMISSILEPACKS));
        
    // Wrecked Ship Super Missile Packs
    XRCCTRL(*this, "IDC_PICKUPS_SMISSILE_WS1", wxCheckBox)->
        SetValue(sram->hasSuperMissilePack(SO_WSSMISSILEPACKS));
    XRCCTRL(*this, "IDC_PICKUPS_SMISSILE_WS2", wxCheckBox)->
        SetValue(sram->hasSuperMissilePack(SO_WSSMISSILEPACKS + 1));
        
    // Maridia Super Missile Packs
    XRCCTRL(*this, "IDC_PICKUPS_SMISSILE_M1", wxCheckBox)->
        SetValue(sram->hasSuperMissilePack(SO_MSMISSILEPACKS));
    XRCCTRL(*this, "IDC_PICKUPS_SMISSILE_M2", wxCheckBox)->
        SetValue(sram->hasSuperMissilePack(SO_MSMISSILEPACKS + 1));
    XRCCTRL(*this, "IDC_PICKUPS_SMISSILE_M3", wxCheckBox)->
        SetValue(sram->hasSuperMissilePack(SO_MSMISSILEPACKS + 2));
}

void Frame::loadYellowDoors() {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    // Crateria Yellow Doors
    XRCCTRL(*this, "IDC_YELLOWDOOR_C1",
        wxCheckBox)->SetValue(sram->getDoor(YD_C1));
    XRCCTRL(*this, "IDC_YELLOWDOOR_C2",
        wxCheckBox)->SetValue(sram->getDoor(YD_C2));
    XRCCTRL(*this, "IDC_YELLOWDOOR_C3",
        wxCheckBox)->SetValue(sram->getDoor(YD_C3));
    XRCCTRL(*this, "IDC_YELLOWDOOR_C4",
        wxCheckBox)->SetValue(sram->getDoor(YD_C4));
    XRCCTRL(*this, "IDC_YELLOWDOOR_C5",
        wxCheckBox)->SetValue(sram->getDoor(YD_C5));
    XRCCTRL(*this, "IDC_YELLOWDOOR_C6",
        wxCheckBox)->SetValue(sram->getDoor(YD_C6));
    
    // Brinstar Yellow Doors
    XRCCTRL(*this, "IDC_YELLOWDOOR_BS1",
        wxCheckBox)->SetValue(sram->getDoor(YD_BS1));
    XRCCTRL(*this, "IDC_YELLOWDOOR_BS2",
        wxCheckBox)->SetValue(sram->getDoor(YD_BS2));
    XRCCTRL(*this, "IDC_YELLOWDOOR_BS3",
        wxCheckBox)->SetValue(sram->getDoor(YD_BS3));
    XRCCTRL(*this, "IDC_YELLOWDOOR_BS_XRAYSCOPE",
        wxCheckBox)->SetValue(sram->getDoor(YD_BS_XRAYSCOPE));
    
    // Norfair
    XRCCTRL(*this, "IDC_YELLOWDOOR_NF1",
        wxCheckBox)->SetValue(sram->getDoor(YD_NF1));
    XRCCTRL(*this, "IDC_YELLOWDOOR_NF2",
        wxCheckBox)->SetValue(sram->getDoor(YD_NF2));
    XRCCTRL(*this, "IDC_YELLOWDOOR_NF_MAP",
        wxCheckBox)->SetValue(sram->getDoor(YD_NF_MAP));
}

void Frame::loadZebetites() {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    unsigned int zebetites = sram->getZebetites();
    
    if (zebetites < 5) {
        XRCCTRL(*this, "IDRB_GAME_ZEBETITES",
            wxRadioBox)->SetSelection(zebetites);
    } else {
        // this should NEVER happen
        throw InvalidSRAMDataException("Invalid destroyed zebetite count");
    }
}

void Frame::open(const wxString &filename) {
    try {
        SRAMFile *temp = new SRAMFile(filename);
        
        if (sram) {
            close();
        }
        
        sram = temp;
        
        for (unsigned int i = 0; i < SRAMFile::GAMES; ++i) {
            if (sram->isValidGame(i)) {
                load(i);
                break;
            }
        }
    } catch (InvalidSRAMFileException &e) {
        wxString error(e.what(), wxConvLibc);
        
        wxMessageBox(error,
            wxT("Error: Invalid Super Metroid SRAM file"),
            wxOK | wxICON_ERROR);
    }
}

bool Frame::save(const wxString &filename) {
    try {
        sram->save(filename);
    } catch (FileIOException &e) {
        wxMessageBox(wxT("Unable to write SRAM file"),
                     wxT("Error: Unable to save SRAM"),
                     wxOK | wxICON_ERROR);
        
        return false;
    }
    
    return true;
}

void Frame::onBossChange(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    enum Statue statue;
    
    if (event.GetId() == XRCID("IDC_GAME_BOSS_KRAID")) {
        sram->setBoss(BOSS_KRAID, event.IsChecked());
        statue = STATUE_KRAID;
    } else if (event.GetId() == XRCID("IDC_GAME_BOSS_PHANTOON")) {
        sram->setBoss(BOSS_PHANTOON, event.IsChecked());
        statue = STATUE_PHANTOON;
    } else if (event.GetId() == XRCID("IDC_GAME_BOSS_DRAYGON")) {
        sram->setBoss(BOSS_DRAYGON, event.IsChecked());
        statue = STATUE_DRAYGON;
    } else {
        // ensure we have ridley
        wxASSERT(event.GetId() == XRCID("IDC_GAME_BOSS_RIDLEY"));
        
        sram->setBoss(BOSS_RIDLEY, event.IsChecked());
        statue = STATUE_RIDLEY;
    }
    
    if (!event.IsChecked()) {
        sram->setStatue(statue, false);
        sram->setMiscBit(MB_TELEVATOR, false);
    }
}

void Frame::onControllerChange(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    enum Button button = static_cast<enum Button>(event.GetSelection());
    
    if (event.GetId() == XRCID("IDC_MISC_DASH")) {
        sram->setButton(ACTION_DASH, button);
    } else if (event.GetId() == XRCID("IDC_MISC_JUMP")) {
        sram->setButton(ACTION_JUMP, button);
    } else if (event.GetId() == XRCID("IDC_MISC_ITEMCANCEL")) {
        sram->setButton(ACTION_ITEMCANCEL, button);
    } else if (event.GetId() == XRCID("IDC_MISC_SHOT")) {
        sram->setButton(ACTION_SHOT, button);
    } else if (event.GetId() == XRCID("IDC_MISC_ANGLEUP")) {
        button = static_cast<enum Button>(event.GetSelection() + BUTTON_L);
        sram->setButton(ACTION_ANGLEUP, button);
    } else if (event.GetId() == XRCID("IDC_MISC_ANGLEDOWN")) {
        button = static_cast<enum Button>(event.GetSelection() + BUTTON_L);
        sram->setButton(ACTION_ANGLEDOWN, button);
    } else {
        // ensure item select if we're here
        wxASSERT(event.GetId() == XRCID("IDC_MISC_ITEMSELECT"));
        
        sram->setButton(ACTION_ITEMSELECT, button);
    }
}

void Frame::onEnergyChange(wxCommandEvent &event) {
    if (!ignoreTextEvents) {
        // ensure we have an open SRAMFile
        wxASSERT(sram);
        
        unsigned long value;
        
        if (event.GetString().ToULong(&value)) {
            wxUint16 energy = static_cast<wxUint16>(value);

            if (event.GetId() == XRCID("IDT_GAME_ENERGY")) {
                unsigned int max = sram->getMaxEnergy();
                
                if (value > max) {
                    value = max;
                    
                    GetStatusBar()->SetStatusText(wxString::Format
                        (wxT("Warning: Samus' max energy is %d"), max));
                    timer.Start(CLEAR_DELAY, true);
                }
                
                sram->setEnergy(energy);
            } else {
                // ensure we are dealing with reserve energy
                wxASSERT(event.GetId() == XRCID("IDT_GAME_RESERVE"));
                
                unsigned int max = sram->getMaxReserveEnergy();
                
                if (value > max) {
                    value = max;
                    
                    GetStatusBar()->SetStatusText(wxString::Format
                        (wxT("Warning: Samus' max reserve energy is %d"), max));
                    timer.Start(CLEAR_DELAY, true);
                }
                
                sram->setReserveEnergy(energy);
            }
        }
    }
}

void Frame::onEnergyTankChange(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    if (event.GetId() == XRCID("IDC_ITEMS_ET_C1")) {
        sram->setEnergyTank(SO_CETANKS, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_ET_C2")) {
        sram->setEnergyTank(SO_CETANKS + 1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_ET_BS1")) {
        sram->setEnergyTank(SO_BSETANKS, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_ET_BS2")) {
        sram->setEnergyTank(SO_BSETANKS + 1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_ET_BS3")) {
        sram->setEnergyTank(SO_BSETANKS + 2, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_ET_BS4")) {
        sram->setEnergyTank(SO_BSETANKS + 3, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_ET_BS5")) {
        sram->setEnergyTank(SO_BSETANKS + 4, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_ET_NF1")) {
        sram->setEnergyTank(SO_NFETANKS, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_ET_NF2")) {
        sram->setEnergyTank(SO_NFETANKS + 1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_ET_NF3")) {
        sram->setEnergyTank(SO_NFETANKS + 2, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_ET_NF4")) {
        sram->setEnergyTank(SO_NFETANKS + 3, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_ET_WS")) {
        sram->setEnergyTank(SO_WSETANKS, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_ET_M1")) {
        sram->setEnergyTank(SO_METANKS, event.IsChecked());
    } else {
        // ensure we have maridia energy tank 2 if we're here
        wxASSERT(event.GetId() == XRCID("IDC_ITEMS_ET_M2"));
        
        sram->setEnergyTank(SO_METANKS + 1, event.IsChecked());
    }
    
    wxTextCtrl *ctrl = XRCCTRL(*this, "IDT_GAME_ENERGY", wxTextCtrl);
    unsigned long value;
    
    if (ctrl->GetValue().ToULong(&value)) {
        wxUint16 max = sram->getMaxEnergy();
        
        if (value > max) {
            ignoreTextEvents = true;
            ctrl->SetValue(wxString::Format(wxT("%d"), max));
            ignoreTextEvents = false;
        }
    }
}

void Frame::onEyeDoorChange(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    if (event.GetId() == XRCID("IDC_EYEDOOR_KRAID")) {
        sram->setDoor(ED_KRAID, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_EYEDOOR_PHANTOON")) {
        sram->setDoor(ED_PHANTOON, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_EYEDOOR_DRAYGON")) {
        sram->setDoor(ED_DRAYGON, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_EYEDOOR_RIDLEY")) {
        sram->setDoor(ED_RIDLEY, event.IsChecked());
    } else {
        // make sure we have the Tourian eye door if we're here
        wxASSERT(event.GetId() == XRCID("IDC_EYEDOOR_T"));
        
        sram->setDoor(ED_T, event.IsChecked());
    }
}

void Frame::onFileClose(wxCommandEvent &) {
    // ensure we are not trying to close a file we never opened
    wxASSERT(sram);
    
    if (sram->isModified()) {
        int answer = wxMessageBox(wxT("Save SRAM before closing?"),
                                  wxT("Warning: Unsaved SRAM file"),
                                  wxYES_NO | wxCANCEL | wxICON_QUESTION);
                                  
        if (answer == wxYES) {
            close(true);
            
            return;
        } else if (answer == wxCANCEL) {
            return;
        }
    }
    
    close();
}

void Frame::onFileOpen(wxCommandEvent &) {
    // save current SRAMFile
    if (sram && sram->isModified()) {
        int answer = wxMessageBox(wxT("Save current SRAM?"),
                                  wxT("Warning: Unsaved SRAM file"),
                                  wxYES_NO | wxICON_QUESTION);
                                  
        if (answer == wxYES) {
            if (!save()) {
                return;
            }
        }
    }
    
    wxFileDialog dlg(this, wxT("Select a Super Metroid SRAM file"),
                     wxEmptyString, wxEmptyString,
                     wxT("SRAM Files (*.srm)|*.srm"), wxOPEN);
                     
    if (dlg.ShowModal() == wxID_OK) {
        open(dlg.GetPath());
    }
}

void Frame::onFileSave(wxCommandEvent &) {
    // ensure we have an open modified SRAMFile
    wxASSERT(sram && sram->isModified());
    
    save();
}

void Frame::onFileSaveAs(wxCommandEvent &) {
    // ensure we have an open modified SRAMFile
    wxASSERT(sram && sram->isModified());
    
    wxFileDialog dlg(this, wxT("Select a Super Metroid SRAM file"),
                     wxEmptyString, wxEmptyString,
                     wxT("SRAM Files (*.srm)|*.srm"), wxSAVE);
                     
    if (dlg.ShowModal() == wxID_OK) {
        sram->save(dlg.GetPath());
    }
}

void Frame::onFileSaveUpdate(wxUpdateUIEvent &) {
    bool enable = (sram && sram->isModified());
    
    wxMenuItem *item = GetMenuBar()->FindItem(wxID_SAVE);
    item->Enable(enable);
    
    item = GetMenuBar()->FindItem(wxID_SAVEAS);
    item->Enable(enable);
}

void Frame::onGame(wxCommandEvent &event) {
    if (event.GetId() == XRCID("IDM_GAME_GAMEA")) {
        load(0);
    } else if (event.GetId() == XRCID("IDM_GAME_GAMEB")) {
        load(1);
    } else {
        // ensure we are only getting the third game here
        wxASSERT(event.GetId() == XRCID("IDM_GAME_GAMEC"));
        
        load(2);
    }
}

void Frame::onGameClear(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    int game;
    wxChar ch;
    
    if (event.GetId() == XRCID("IDM_GAME_CLEARA")) {
        game = 0;
        ch = wxT('A');
    } else if (event.GetId() == XRCID("IDM_GAME_CLEARB")) {
        game = 1;
        ch = wxT('B');
    } else {
        // ensure we got game C if we're here
        wxASSERT(event.GetId() == XRCID("IDM_GAME_CLEARC"));
        
        game = 2;
        ch = wxT('C');
    }
    
    int answer = wxMessageBox(wxString::Format
        (wxT("Are you sure you want to erase Game %c?"), ch),
        wxT("Really erase game?"), wxYES_NO | wxICON_QUESTION);
        
    if (answer == wxYES) {
        // make copy of the SRAMFile
        SRAMFile *temp = new SRAMFile(*sram);
        
        // erase the old game
        sram->clear(game);
        
        if (this->game == game) {
            // time to change games
            int game = -1;
            
            for (unsigned int i = 0; i < SRAMFile::GAMES; ++i) {
                if (sram->isValidGame(i)) {
                    game = i;
                    break;
                }
            }
            
            if (game == -1) {
                // we deleted the only valid game
                if (!close(true)) {
                    // we can't save the file, so we'll have to revert
                    delete sram;
                    
                    sram = temp;
                }
            } else {
                // no need for the copy anymore
                delete temp;
                
                load(game);
            }
        }
        
        enableMenus();
    }
}

void Frame::onGameClearUpdate(wxUpdateUIEvent &event) {
    if (event.GetId() == XRCID("IDM_GAME_CLEAR")) {
        event.Enable(sram);
    } else if (event.GetId() == XRCID("IDM_GAME_CLEARA")) {
        event.Enable(sram && sram->isValidGame(0));
    } else if (event.GetId() == XRCID("IDM_GAME_CLEARB")) {
        event.Enable(sram && sram->isValidGame(1));
    } else {
        // ensure we have clear C if we're here
        wxASSERT(event.GetId() == XRCID("IDM_GAME_CLEARC"));
        
        event.Enable(sram && sram->isValidGame(2));
    }
}

void Frame::onGameCopy(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    int src, dest;
    wxChar sch, dch;
    
    if (event.GetId() == XRCID("IDM_GAME_COPYAB")) {
        src = 0;
        dest = 1;
        sch = wxT('A');
        dch = wxT('B');
    } else if (event.GetId() == XRCID("IDM_GAME_COPYAC")) {
        src = 0;
        dest = 2;
        sch = wxT('A');
        dch = wxT('C');
    } else if (event.GetId() == XRCID("IDM_GAME_COPYBA")) {
        src = 1;
        dest = 0;
        sch = wxT('B');
        dch = wxT('A');
    } else if (event.GetId() == XRCID("IDM_GAME_COPYBC")) {
        src = 1;
        dest = 2;
        sch = wxT('B');
        dch = wxT('C');
    } else if (event.GetId() == XRCID("IDM_GAME_COPYCA")) {
        src = 2;
        dest = 0;
        sch = wxT('C');
        dch = wxT('A');
    } else {
        // ensure we have copy C->B if we're here
        wxASSERT(event.GetId() == XRCID("IDM_GAME_COPYCB"));
        
        src = 2;
        dest = 1;
        sch = wxT('C');
        dch = wxT('B');
    }
    
    int answer = wxMessageBox(wxString::Format
        (wxT("Are you sure you want to copy Game %c over Game %c?"), sch, dch),
        wxT("Really copy game?"), wxYES_NO | wxICON_QUESTION);
        
    if (answer == wxYES) {
        sram->copy(src, dest);
        enableMenus();
    }
}

void Frame::onGameCopyUpdate(wxUpdateUIEvent &event) {
    if (event.GetId() == XRCID("IDM_GAME_COPY")) {
        event.Enable(sram);
    } else if (event.GetId() == XRCID("IDM_GAME_COPYA")) {
        event.Enable(sram && sram->isValidGame(0));
    } else if (event.GetId() == XRCID("IDM_GAME_COPYB")) {
        event.Enable(sram && sram->isValidGame(1));
    } else {
        // ensure we have copy C->B if we're here
        wxASSERT(event.GetId() == XRCID("IDM_GAME_COPYC"));
        
        event.Enable(sram && sram->isValidGame(2));
    }
}

void Frame::onGameEnding(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    wxTextCtrl *ctrl = XRCCTRL(*this, "IDT_MISC_GAMETIME", wxTextCtrl);
    
    if (event.GetId() == XRCID("IDM_GAME_ENDING1")) {
        ctrl->SetValue(wxT("0:00"));
    } else if (event.GetId() == XRCID("IDM_GAME_ENDING2")) {
        ctrl->SetValue(wxT("3:00"));
    } else {
        // ensure we have the third ending if we're here
        wxASSERT(event.GetId() == XRCID("IDM_GAME_ENDING3"));
        
        ctrl->SetValue(wxT("10:00"));
    }
}

void Frame::onGameGiveBosses(wxCommandEvent &) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    sram->setBoss(BOSS_KRAID);
    sram->setStatue(STATUE_KRAID);
    
    sram->setBoss(BOSS_RIDLEY);
    sram->setStatue(STATUE_RIDLEY);
    
    sram->setBoss(BOSS_PHANTOON);
    sram->setStatue(STATUE_PHANTOON);
    
    sram->setBoss(BOSS_DRAYGON);
    sram->setStatue(STATUE_DRAYGON);
    
    sram->setMiscBit(MB_TELEVATOR);
    
    loadBosses();
}

void Frame::onGameGiveEnergy(wxCommandEvent &) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    sram->setEnergy();
    sram->setReserveEnergy();
    loadEnergy();
}

void Frame::onGameGiveEnergyTanks(wxCommandEvent &) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    for (int i = 0; i < ETANKS; ++i) {
        sram->setEnergyTank(i + SO_CETANKS);
    }
    
    loadEnergyTanks();
}

void Frame::onGameGiveEverything(wxCommandEvent &) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    wxCommandEvent event1(wxEVT_COMMAND_MENU_SELECTED,
        XRCID("IDM_GAME_GIVE_BOSSES"));
    GetEventHandler()->ProcessEvent(event1);
    
    wxCommandEvent event2(wxEVT_COMMAND_MENU_SELECTED,
        XRCID("IDM_GAME_GIVE_MINIBOSSES"));
    GetEventHandler()->ProcessEvent(event2);
    
    wxCommandEvent event3(wxEVT_COMMAND_MENU_SELECTED,
        XRCID("IDM_GAME_GIVE_METROIDROOMS"));
    GetEventHandler()->ProcessEvent(event3);
    
    wxCommandEvent event4(wxEVT_COMMAND_MENU_SELECTED,
        XRCID("IDM_GAME_GIVE_ZEBETITES"));
    GetEventHandler()->ProcessEvent(event4);
    
    wxCommandEvent event5(wxEVT_COMMAND_MENU_SELECTED,
        XRCID("IDM_GAME_GIVE_MAPS"));
    GetEventHandler()->ProcessEvent(event5);
    
    wxCommandEvent event6(wxEVT_COMMAND_MENU_SELECTED,
        XRCID("IDM_GAME_GIVE_ITEMS"));
    GetEventHandler()->ProcessEvent(event6);
    
    wxCommandEvent event7(wxEVT_COMMAND_MENU_SELECTED,
        XRCID("IDM_GAME_GIVE_MISSILES"));
    GetEventHandler()->ProcessEvent(event7);
    
    wxCommandEvent event8(wxEVT_COMMAND_MENU_SELECTED,
        XRCID("IDM_GAME_GIVE_SMISSILES"));
    GetEventHandler()->ProcessEvent(event8);
    
    wxCommandEvent event9(wxEVT_COMMAND_MENU_SELECTED,
        XRCID("IDM_GAME_GIVE_PBOMBS"));
    GetEventHandler()->ProcessEvent(event9);
    
    wxCommandEvent event10(wxEVT_COMMAND_MENU_SELECTED,
        XRCID("IDM_GAME_GIVE_ETANKS"));
    GetEventHandler()->ProcessEvent(event10);
    
    wxCommandEvent event11(wxEVT_COMMAND_MENU_SELECTED,
        XRCID("IDM_GAME_GIVE_RTANKS"));
    GetEventHandler()->ProcessEvent(event11);
    
    wxCommandEvent event12(wxEVT_COMMAND_MENU_SELECTED,
        XRCID("IDM_GAME_GIVE_REDDOORS"));
    GetEventHandler()->ProcessEvent(event12);
    
    wxCommandEvent event13(wxEVT_COMMAND_MENU_SELECTED,
        XRCID("IDM_GAME_GIVE_GREENDOORS"));
    GetEventHandler()->ProcessEvent(event13);
    
    wxCommandEvent event14(wxEVT_COMMAND_MENU_SELECTED,
        XRCID("IDM_GAME_GIVE_YELLOWDOORS"));
    GetEventHandler()->ProcessEvent(event14);
    
    wxCommandEvent event15(wxEVT_COMMAND_MENU_SELECTED,
        XRCID("IDM_GAME_GIVE_EYEDOORS"));
    GetEventHandler()->ProcessEvent(event15);
    
    wxCommandEvent event16(wxEVT_COMMAND_MENU_SELECTED,
        XRCID("IDM_GAME_GIVE_METALDOORS"));
    GetEventHandler()->ProcessEvent(event16);
    
    wxCommandEvent event17(wxEVT_COMMAND_MENU_SELECTED,
        XRCID("IDM_GAME_GIVE_ENERGY"));
    GetEventHandler()->ProcessEvent(event17);
    
    wxCommandEvent event18(wxEVT_COMMAND_MENU_SELECTED,
        XRCID("IDM_GAME_GIVE_INVENTORY"));
    GetEventHandler()->ProcessEvent(event18);
}

void Frame::onGameGiveEyeDoors(wxCommandEvent &) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    for (int i = 0; i < EYEDOORS; ++i) {
        sram->setDoor(i + ED_KRAID);
    }
    
    loadEyeDoors();
}

void Frame::onGameGiveGreenDoors(wxCommandEvent &) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    for (int i = 0; i < GREENDOORS; ++i) {
        sram->setDoor(i + GD_C1);
    }
    
    loadGreenDoors();
}

void Frame::onGameGiveInventory(wxCommandEvent &) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    sram->setMissiles();
    sram->setSuperMissiles();
    sram->setPowerBombs();
    loadInventory();
}

void Frame::onGameGiveItems(wxCommandEvent &) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    sram->setItem(ITEM_MORPHINGBALL);
    sram->setItemEquipped(ITEM_MORPHINGBALL);
    
    sram->setItem(ITEM_BOMBS);
    sram->setItemEquipped(ITEM_BOMBS);
    
    sram->setItem(ITEM_SPRINGBALL);
    sram->setItemEquipped(ITEM_SPRINGBALL);
    
    sram->setItem(ITEM_SCREWATTACK);
    sram->setItemEquipped(ITEM_SCREWATTACK);
    
    sram->setItem(ITEM_VARIASUIT);
    sram->setItemEquipped(ITEM_VARIASUIT);
    
    sram->setItem(ITEM_GRAVITYSUIT);
    sram->setItemEquipped(ITEM_GRAVITYSUIT);
    
    sram->setItem(ITEM_HJBOOTS);
    sram->setItemEquipped(ITEM_HJBOOTS);
    
    sram->setItem(ITEM_SPACEJUMP);
    sram->setItemEquipped(ITEM_SPACEJUMP);
    
    sram->setItem(ITEM_SPEEDBOOSTER);
    sram->setItemEquipped(ITEM_SPEEDBOOSTER);
    
    sram->setItem(ITEM_CHARGEBEAM);
    sram->setItemEquipped(ITEM_CHARGEBEAM);
    
    sram->setItem(ITEM_ICEBEAM);
    sram->setItemEquipped(ITEM_ICEBEAM);
    
    sram->setItem(ITEM_WAVEBEAM);
    sram->setItemEquipped(ITEM_WAVEBEAM);
    
    sram->setItem(ITEM_SPAZER);
    sram->setItemEquipped(ITEM_SPAZER, false);
    
    sram->setItem(ITEM_PLASMA);
    sram->setItemEquipped(ITEM_PLASMA);
    
    sram->setItem(ITEM_GRAPPLINGBEAM);
    sram->setItemEquipped(ITEM_GRAPPLINGBEAM);
    
    sram->setItem(ITEM_XRAYSCOPE);
    sram->setItemEquipped(ITEM_XRAYSCOPE);
    
    loadItems();
}

void Frame::onGameGiveMaps(wxCommandEvent &) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    sram->setMap(AREA_C);
    sram->setMap(AREA_BS);
    sram->setMap(AREA_NF);
    sram->setMap(AREA_WS);
    sram->setMap(AREA_M);
    sram->setMap(AREA_T);
    
    loadMaps();
}

void Frame::onGameGiveMetalDoors(wxCommandEvent &) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    for (int i = 0; i < METALDOORS; ++i) {
        sram->setDoor(i + MD_C_BOMBSEXIT);
    }
    
    loadMetalDoors();
}

void Frame::onGameGiveMetroidRooms(wxCommandEvent &) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    sram->setMetroidRoom(MR1);
    sram->setMetroidRoom(MR2);
    sram->setMetroidRoom(MR3);
    sram->setMetroidRoom(MR4);
    
    loadMetroidRooms();
}

void Frame::onGameGiveMiniBosses(wxCommandEvent &) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    sram->setBoss(BOSS_SILVERTORIZO);
    sram->setBoss(BOSS_SPORESPAWN);
    sram->setBoss(BOSS_BOTWOON);
    sram->setBoss(BOSS_CROCOMIRE);
    sram->setBoss(BOSS_GOLDENTORIZO);
    
    loadMiniBosses();
}

void Frame::onGameGiveMissilePacks(wxCommandEvent &) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    for (int i = 0; i < MISSILE_PACKS; ++i) {
        sram->setMissilePack(i + SO_CMISSILEPACKS);
    }
    
    loadMissilePacks();
}

void Frame::onGameGivePowerBombPacks(wxCommandEvent &) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    for (int i = 0; i < PBOMB_PACKS; ++i) {
        sram->setPowerBombPack(i + SO_CPBOMBPACKS);
    }
    
    loadPowerBombPacks();
}

void Frame::onGameGiveRedDoors(wxCommandEvent &) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    for (int i = 0; i < REDDOORS; ++i) {
        sram->setDoor(i + RD_C_MAP);
    }
    
    loadRedDoors();
}

void Frame::onGameGiveReserveTanks(wxCommandEvent &) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    for (int i = 0; i < RTANKS; ++i) {
        sram->setReserveTank(i + SO_BSRTANK);
    }
    
    loadReserveTanks();
}

void Frame::onGameGiveSuperMissilePacks(wxCommandEvent &) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    for (int i = 0; i < SMISSILE_PACKS; ++i) {
        sram->setSuperMissilePack(i + SO_CSMISSILEPACKS);
    }
    
    loadSuperMissilePacks();
}

void Frame::onGameGiveYellowDoors(wxCommandEvent &) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    for (int i = 0; i < YELLOWDOORS; ++i) {
        sram->setDoor(i + YD_C1);
    }
    
    loadYellowDoors();
}

void Frame::onGameGiveZebetites(wxCommandEvent &) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    sram->setZebetites();
    loadZebetites();
}

void Frame::onGameTimeChange(wxCommandEvent &event) {
    if (!ignoreTextEvents) {
        // ensure we have an open SRAMFile
        wxASSERT(sram);
        
        wxRegEx regex(wxT("^([0-9]{1,2}):([0-9]{2,2})$"));
        
        // ensure our regular expression is valid
        wxASSERT(regex.IsValid());
        
        wxString value = event.GetString();
        
        if (regex.Matches(value)) {
            long hours, minutes;
            
            regex.GetMatch(value, 1).ToLong(&hours);
            regex.GetMatch(value, 2).ToLong(&minutes);
            
            if (minutes < 60) {
                sram->setGameHours(static_cast<unsigned char>(hours));
                sram->setGameMinutes(static_cast<unsigned char>(minutes));
                
                GetStatusBar()->SetStatusText(wxEmptyString);
            } else {
                GetStatusBar()->SetStatusText
                    (wxT("Warning: Game Minutes cannot exceed 60"));
                timer.Start(CLEAR_DELAY, true);
            }
        } else {
            GetStatusBar()->SetStatusText
                (wxT("Warning: Game time must be of the form HH:MM"));
            timer.Start(CLEAR_DELAY, true);
        }
    }
}

void Frame::onGameUpdate(wxUpdateUIEvent &event) {
    if (sram) {
        int game;
    
        if (event.GetId() == XRCID("IDM_GAME_GAMEA")) {
            game = 0;
        } else if (event.GetId() == XRCID("IDM_GAME_GAMEB")) {
            game = 1;
        } else {
            // ensure we are only getting the third game here
            wxASSERT(event.GetId() == XRCID("IDM_GAME_GAMEC"));
            
            game = 2;
        }
        
        if (game == sram->getGame()) {
            event.Check(true);
        }
        
        event.Enable(sram->isValidGame(game));
    }
}

void Frame::onGreenDoorChange(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    if (event.GetId() == XRCID("IDC_GREENDOOR_C1")) {
        sram->setDoor(GD_C1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GREENDOOR_C_WS")) {
        sram->setDoor(GD_C_WS, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GREENDOOR_BS1")) {
        sram->setDoor(GD_BS1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GREENDOOR_BS2")) {
        sram->setDoor(GD_BS2, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GREENDOOR_BS3")) {
        sram->setDoor(GD_BS3, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GREENDOOR_BS4")) {
        sram->setDoor(GD_BS4, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GREENDOOR_BS5")) {
        sram->setDoor(GD_BS5, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GREENDOOR_BS6")) {
        sram->setDoor(GD_BS6, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GREENDOOR_BS7")) {
        sram->setDoor(GD_BS7, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GREENDOOR_BS8")) {
        sram->setDoor(GD_BS8, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GREENDOOR_BS_SPAZER")) {
        sram->setDoor(GD_BS_SPAZER, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GREENDOOR_BS_SPORESPAWNEXIT")) {
        sram->setDoor(GD_BS_SPORESPAWNEXIT, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GREENDOOR_NF1")) {
        sram->setDoor(GD_NF1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GREENDOOR_NF2")) {
        sram->setDoor(GD_NF2, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GREENDOOR_NF3")) {
        sram->setDoor(GD_NF3, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GREENDOOR_NF4")) {
        sram->setDoor(GD_NF4, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GREENDOOR_NF_ICEBEAM")) {
        sram->setDoor(GD_NF_ICEBEAM, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GREENDOOR_NF_SPEEDBOOSTER")) {
        sram->setDoor(GD_NF_SPEEDBOOSTER, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GREENDOOR_WS")) {
        sram->setDoor(GD_WS, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GREENDOOR_M1")) {
        sram->setDoor(GD_M1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GREENDOOR_M2")) {
        sram->setDoor(GD_M2, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GREENDOOR_M3")) {
        sram->setDoor(GD_M3, event.IsChecked());
    } else {
        // ensure we have Draygon's green door if we're here
        wxASSERT(event.GetId() == XRCID("IDC_GREENDOOR_M_DRAYGON"));
        
        sram->setDoor(GD_M_DRAYGON, event.IsChecked());
    }
}

void Frame::onHelpAbout(wxCommandEvent &) {
    wxString message = APP_NAME + wxT(' ') + APP_VERSION + wxT('\n') +
                       APP_COPYRIGHT + wxT('\n') + APP_URL;
    wxString title = wxT("About ") + APP_NAME + wxT("...");
    
    wxMessageBox(message, title, wxOK | wxICON_INFORMATION);
}

void Frame::onItemChange(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    enum Item item;
    
    if (event.GetId() == XRCID("IDC_ITEMS_MORPHINGBALL")) {
        item = ITEM_MORPHINGBALL;
    } else if (event.GetId() == XRCID("IDC_ITEMS_BOMBS")) {
        item = ITEM_BOMBS;
    } else if (event.GetId() == XRCID("IDC_ITEMS_SPRINGBALL")) {
        item = ITEM_SPRINGBALL;
    } else if (event.GetId() == XRCID("IDC_ITEMS_SCREWATTACK")) {
        item = ITEM_SCREWATTACK;
    } else if (event.GetId() == XRCID("IDC_ITEMS_VARIASUIT")) {
        item = ITEM_VARIASUIT;
    } else if (event.GetId() == XRCID("IDC_ITEMS_GRAVITYSUIT")) {
        item = ITEM_GRAVITYSUIT;
    } else if (event.GetId() == XRCID("IDC_ITEMS_HJBOOTS")) {
        item = ITEM_HJBOOTS;
    } else if (event.GetId() == XRCID("IDC_ITEMS_SPACEJUMP")) {
        item = ITEM_SPACEJUMP;
    } else if (event.GetId() == XRCID("IDC_ITEMS_SPEEDBOOSTER")) {
        item = ITEM_SPEEDBOOSTER;
    } else if (event.GetId() == XRCID("IDC_ITEMS_CHARGEBEAM")) {
        item = ITEM_CHARGEBEAM;
    } else if (event.GetId() == XRCID("IDC_ITEMS_ICEBEAM")) {
        item = ITEM_ICEBEAM;
    } else if (event.GetId() == XRCID("IDC_ITEMS_WAVEBEAM")) {
        item = ITEM_WAVEBEAM;
    } else if (event.GetId() == XRCID("IDC_ITEMS_SPAZER")) {
        item = ITEM_SPAZER;
    } else if (event.GetId() == XRCID("IDC_ITEMS_PLASMA")) {
        item = ITEM_PLASMA;
    } else if (event.GetId() == XRCID("IDC_ITEMS_XRAYSCOPE")) {
        item = ITEM_XRAYSCOPE;
        sram->setItemEquipped(item, event.IsChecked());
    } else {
        // ensure we have the grappling beam control at this point
        wxASSERT(event.GetId() == XRCID("IDC_ITEMS_GRAPPLINGBEAM"));
        
        item = ITEM_GRAPPLINGBEAM;
        sram->setItemEquipped(ITEM_GRAPPLINGBEAM, event.IsChecked());
    }
    
    sram->setItem(item, event.IsChecked());
    
    if (!event.IsChecked()) {
        sram->setItemEquipped(item, false);
    }
}

void Frame::onItemEquippedChange(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    if (event.GetId() == XRCID("IDC_ITEMS_MORPHINGBALL_EQUIPPED")) {
        sram->setItemEquipped(ITEM_MORPHINGBALL, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_BOMBS_EQUIPPED")) {
        sram->setItemEquipped(ITEM_BOMBS, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_SPRINGBALL_EQUIPPED")) {
        sram->setItemEquipped(ITEM_SPRINGBALL, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_SCREWATTACK_EQUIPPED")) {
        sram->setItemEquipped(ITEM_SCREWATTACK, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_VARIASUIT_EQUIPPED")) {
        sram->setItemEquipped(ITEM_VARIASUIT, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_GRAVITYSUIT_EQUIPPED")) {
        sram->setItemEquipped(ITEM_GRAVITYSUIT, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_HJBOOTS_EQUIPPED")) {
        sram->setItemEquipped(ITEM_HJBOOTS, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_SPACEJUMP_EQUIPPED")) {
        sram->setItemEquipped(ITEM_SPACEJUMP, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_SPEEDBOOSTER_EQUIPPED")) {
        sram->setItemEquipped(ITEM_SPEEDBOOSTER, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_CHARGEBEAM_EQUIPPED")) {
        sram->setItemEquipped(ITEM_CHARGEBEAM, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_ICEBEAM_EQUIPPED")) {
        sram->setItemEquipped(ITEM_ICEBEAM, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_WAVEBEAM_EQUIPPED")) {
        sram->setItemEquipped(ITEM_WAVEBEAM, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_SPAZER_EQUIPPED")) {
        sram->setItemEquipped(ITEM_SPAZER, event.IsChecked());
        
        if (event.IsChecked()) {
            sram->setItemEquipped(ITEM_PLASMA, false);
        }
    } else {
        // ensure we have the plasma control at this point
        wxASSERT(event.GetId() == XRCID("IDC_ITEMS_PLASMA_EQUIPPED"));
        
        sram->setItemEquipped(ITEM_PLASMA, event.IsChecked());
        
        if (event.IsChecked()) {
            sram->setItemEquipped(ITEM_SPAZER, false);
        }
    }
}

void Frame::onItemEquippedUpdate(wxUpdateUIEvent &event) {
    if (sram) {
        enum Item item;
        
        if (event.GetId() == XRCID("IDC_ITEMS_MORPHINGBALL_EQUIPPED")) {
            item = ITEM_MORPHINGBALL;
        } else if (event.GetId() == XRCID("IDC_ITEMS_BOMBS_EQUIPPED")) {
            item = ITEM_BOMBS;
        } else if (event.GetId() == XRCID("IDC_ITEMS_SPRINGBALL_EQUIPPED")) {
            item = ITEM_SPRINGBALL;
        } else if (event.GetId() == XRCID("IDC_ITEMS_HJBOOTS_EQUIPPED")) {
            item = ITEM_HJBOOTS;
        } else if (event.GetId() == XRCID("IDC_ITEMS_VARIASUIT_EQUIPPED")) {
            item = ITEM_VARIASUIT;
        } else if (event.GetId() == XRCID("IDC_ITEMS_GRAVITYSUIT_EQUIPPED")) {
            item = ITEM_GRAVITYSUIT;
        } else if (event.GetId() == XRCID("IDC_ITEMS_SPEEDBOOSTER_EQUIPPED")) {
            item = ITEM_SPEEDBOOSTER;
        } else if (event.GetId() == XRCID("IDC_ITEMS_SPACEJUMP_EQUIPPED")) {
            item = ITEM_SPACEJUMP;
        } else if (event.GetId() == XRCID("IDC_ITEMS_SCREWATTACK_EQUIPPED")) {
            item = ITEM_SCREWATTACK;
        } else if (event.GetId() == XRCID("IDC_ITEMS_CHARGEBEAM_EQUIPPED")) {
            item = ITEM_CHARGEBEAM;
        } else if (event.GetId() == XRCID("IDC_ITEMS_ICEBEAM_EQUIPPED")) {
            item = ITEM_ICEBEAM;
        } else if (event.GetId() == XRCID("IDC_ITEMS_WAVEBEAM_EQUIPPED")) {
            item = ITEM_WAVEBEAM;
        } else if (event.GetId() == XRCID("IDC_ITEMS_SPAZER_EQUIPPED")) {
            item = ITEM_SPAZER;
            
            if (sram->isItemEquipped(ITEM_PLASMA)) {
                event.Check(false);
            }
        } else {
            // ensure we are really getting the plasma
            wxASSERT(event.GetId() == XRCID("IDC_ITEMS_PLASMA_EQUIPPED"));
            
            item = ITEM_PLASMA;
            
            if (sram->isItemEquipped(ITEM_SPAZER)) {
                event.Check(false);
            }
        }
        
        event.Enable(sram->hasItem(item));
        
        if (!sram->hasItem(item)) {
            event.Check(false);
        }
    }
}

void Frame::onMapChange(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    if (event.GetId() == XRCID("IDC_GAME_MAP_C")) {
        sram->setMap(AREA_C, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GAME_MAP_BS")) {
        sram->setMap(AREA_BS, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GAME_MAP_NF")) {
        sram->setMap(AREA_NF, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GAME_MAP_WS")) {
        sram->setMap(AREA_WS, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GAME_MAP_M")) {
        sram->setMap(AREA_M, event.IsChecked());
    } else {
        // ensure we are getting the Tourian Map at this point
        wxASSERT(event.GetId() == XRCID("IDC_GAME_MAP_T"));
        
        sram->setMap(AREA_T, event.IsChecked());
    }
}

void Frame::onMetalDoorChange(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    if (event.GetId() == XRCID("IDC_METALDOOR_C_BOMBSEXIT")) {
        sram->setDoor(MD_C_BOMBSEXIT, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_BS1")) {
        sram->setDoor(MD_BS1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_BS2")) {
        sram->setDoor(MD_BS2, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_BS3")) {
        sram->setDoor(MD_BS3, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_BS4")) {
        sram->setDoor(MD_BS4, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_BS5")) {
        sram->setDoor(MD_BS5, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_BS6")) {
        sram->setDoor(MD_BS6, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_BS7")) {
        sram->setDoor(MD_BS7, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_BS8")) {
        sram->setDoor(MD_BS8, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_BS9")) {
        sram->setDoor(MD_BS9, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_BS_MAPEXIT")) {
        sram->setDoor(MD_BS_MAPEXIT, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_BS_OLDTLEFT")) {
        sram->setDoor(MD_BS_OLDTLEFT, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_BS_OLDTRIGHT")) {
        sram->setDoor(MD_BS_OLDTRIGHT, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_BS_MINIKRAIDLEFT")) {
        sram->setDoor(MD_BS_MINIKRAIDLEFT, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_BS_MINIKRAIDRIGHT")) {
        sram->setDoor(MD_BS_MINIKRAIDRIGHT, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_BS_VARIASUIT")) {
        sram->setDoor(MD_BS_VARIASUIT, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_BS_KRAIDEXIT")) {
        sram->setDoor(MD_BS_KRAIDEXIT, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_NF_HJBOOTSEXIT")) {
        sram->setDoor(MD_NF_HJBOOTSEXIT, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_NF_CROCOMIREEXIT")) {
        sram->setDoor(MD_NF_CROCOMIREEXIT, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_NF_SCREWATTACK")) {
        sram->setDoor(MD_NF_SCREWATTACK, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_NF_GOLDSPACEPIRATES")) {
        sram->setDoor(MD_NF_GOLDSPACEPIRATES, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_NF_RIDLEYLEFT")) {
        sram->setDoor(MD_NF_RIDLEYLEFT, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_NF_RIDLEYEXIT")) {
        sram->setDoor(MD_NF_RIDLEYEXIT, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_WS1")) {
        sram->setDoor(MD_WS1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_WS2")) {
        sram->setDoor(MD_WS2, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_WS3")) {
        sram->setDoor(MD_WS3, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_WS4")) {
        sram->setDoor(MD_WS4, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_WS_PHANTOONEXIT")) {
        sram->setDoor(MD_WS_PHANTOONEXIT, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_M1")) {
        sram->setDoor(MD_M1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_M2")) {
        sram->setDoor(MD_M2, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_M_BOTWOONEXIT")) {
        sram->setDoor(MD_M_BOTWOONEXIT, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_M_SPACEJUMP")) {
        sram->setDoor(MD_M_SPACEJUMP, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_M_DRAYGONEXIT")) {
        sram->setDoor(MD_M_DRAYGONEXIT, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_M_PLASMA")) {
        sram->setDoor(MD_M_PLASMA, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_M_PLASMAEXIT")) {
        sram->setDoor(MD_M_PLASMAEXIT, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_T1")) {
        sram->setDoor(MD_T1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_T2")) {
        sram->setDoor(MD_T2, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_T3")) {
        sram->setDoor(MD_T3, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_METALDOOR_T4")) {
        sram->setDoor(MD_T4, event.IsChecked());
    } else {
        // ensure we have tourian metal door 5 if we're here
        wxASSERT(event.GetId() == XRCID("IDC_METALDOOR_T5"));
        
        sram->setDoor(MD_T5, event.IsChecked());
    }
}

void Frame::onMetroidRoomChange(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    if (event.GetId() == XRCID("IDC_GAME_METROIDROOM1")) {
        sram->setMetroidRoom(MR1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GAME_METROIDROOM2")) {
        sram->setMetroidRoom(MR2, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GAME_METROIDROOM3")) {
        sram->setMetroidRoom(MR3, event.IsChecked());
    } else {
        // ensure we have metroid room 5 if we're here
        wxASSERT(event.GetId() == XRCID("IDC_GAME_METROIDROOM4"));
        
        sram->setMetroidRoom(MR4, event.IsChecked());
    }
}

void Frame::onMiniBossChange(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    if (event.GetId() == XRCID("IDC_GAME_MINIBOSS_SILVERTORIZO")) {
        sram->setBoss(BOSS_SILVERTORIZO, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GAME_MINIBOSS_SPORESPAWN")) {
        sram->setBoss(BOSS_SPORESPAWN, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GAME_MINIBOSS_BOTWOON")) {
        sram->setBoss(BOSS_BOTWOON, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GAME_MINIBOSS_CROCOMIRE")) {
        sram->setBoss(BOSS_CROCOMIRE, event.IsChecked());
    } else {
        // ensure we came from the Golden Torizo mini-boss control
        wxASSERT(event.GetId() == XRCID("IDC_GAME_MINIBOSS_GOLDENTORIZO"));
        
        sram->setBoss(BOSS_GOLDENTORIZO, event.IsChecked());
    }
}

void Frame::onMiscConfigChange(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    if (event.GetId() == XRCID("IDRB_MISC_LANGUAGE")) {
        sram->setMiscBit(MB_LANGUAGE, event.GetSelection());
    } else if (event.GetId() == XRCID("IDRB_MISC_MOONWALK")) {
        sram->setMiscBit(MB_MOONWALK, event.GetSelection());
    } else {
        // ensure we have icon cancel at this point
        wxASSERT(event.GetId() == XRCID("IDRB_MISC_ICONCANCEL"));
        
        sram->setMiscBit(MB_ICONCANCEL, event.GetSelection());
    }
}

void Frame::onMiscStatChange(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    if (event.GetId() == XRCID("IDC_MISC_MTUBEBROKEN")) {
        sram->setMiscBit(MB_MTUBEBROKEN, event.IsChecked());
    } else {
        // ensure only rescued animals control here
        wxASSERT(event.GetId() == XRCID("IDC_MISC_RESCUEDANIMALS"));
        
        sram->setMiscBit(MB_RESCUEDANIMALS, event.IsChecked());
    }
}

void Frame::onMissilePackChange(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_C1")) {
        sram->setMissilePack(SO_CMISSILEPACKS, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_C2")) {
        sram->setMissilePack(SO_CMISSILEPACKS + 1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_C3")) {
        sram->setMissilePack(SO_CMISSILEPACKS + 2, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_C4")) {
        sram->setMissilePack(SO_CMISSILEPACKS + 3, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_C5")) {
        sram->setMissilePack(SO_CMISSILEPACKS + 4, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_C6")) {
        sram->setMissilePack(SO_CMISSILEPACKS + 5, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_C7")) {
        sram->setMissilePack(SO_CMISSILEPACKS + 6, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_C8")) {
        sram->setMissilePack(SO_CMISSILEPACKS + 7, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_BS1")) {
        sram->setMissilePack(SO_BSMISSILEPACKS, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_BS2")) {
        sram->setMissilePack(SO_BSMISSILEPACKS + 1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_BS3")) {
        sram->setMissilePack(SO_BSMISSILEPACKS + 2, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_BS4")) {
        sram->setMissilePack(SO_BSMISSILEPACKS + 3, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_BS5")) {
        sram->setMissilePack(SO_BSMISSILEPACKS + 4, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_BS6")) {
        sram->setMissilePack(SO_BSMISSILEPACKS + 5, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_BS7")) {
        sram->setMissilePack(SO_BSMISSILEPACKS + 6, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_BS8")) {
        sram->setMissilePack(SO_BSMISSILEPACKS + 7, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_BS9")) {
        sram->setMissilePack(SO_BSMISSILEPACKS + 8, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_BS10")) {
        sram->setMissilePack(SO_BSMISSILEPACKS + 9, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_BS11")) {
        sram->setMissilePack(SO_BSMISSILEPACKS + 10, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_BS12")) {
        sram->setMissilePack(SO_BSMISSILEPACKS + 11, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_NF1")) {
        sram->setMissilePack(SO_NFMISSILEPACKS, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_NF2")) {
        sram->setMissilePack(SO_NFMISSILEPACKS + 1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_NF3")) {
        sram->setMissilePack(SO_NFMISSILEPACKS + 2, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_NF4")) {
        sram->setMissilePack(SO_NFMISSILEPACKS + 3, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_NF5")) {
        sram->setMissilePack(SO_NFMISSILEPACKS + 4, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_NF6")) {
        sram->setMissilePack(SO_NFMISSILEPACKS + 5, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_NF7")) {
        sram->setMissilePack(SO_NFMISSILEPACKS + 6, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_NF8")) {
        sram->setMissilePack(SO_NFMISSILEPACKS + 7, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_NF9")) {
        sram->setMissilePack(SO_NFMISSILEPACKS + 8, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_NF10")) {
        sram->setMissilePack(SO_NFMISSILEPACKS + 9, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_NF11")) {
        sram->setMissilePack(SO_NFMISSILEPACKS + 10, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_NF12")) {
        sram->setMissilePack(SO_NFMISSILEPACKS + 11, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_NF13")) {
        sram->setMissilePack(SO_NFMISSILEPACKS + 12, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_NF14")) {
        sram->setMissilePack(SO_NFMISSILEPACKS + 13, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_NF15")) {
        sram->setMissilePack(SO_NFMISSILEPACKS + 14, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_WS1")) {
        sram->setMissilePack(SO_WSMISSILEPACKS, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_WS2")) {
        sram->setMissilePack(SO_WSMISSILEPACKS + 1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_WS3")) {
        sram->setMissilePack(SO_WSMISSILEPACKS + 2, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_M1")) {
        sram->setMissilePack(SO_MMISSILEPACKS, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_M2")) {
        sram->setMissilePack(SO_MMISSILEPACKS + 1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_M3")) {
        sram->setMissilePack(SO_MMISSILEPACKS + 2, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_M4")) {
        sram->setMissilePack(SO_MMISSILEPACKS + 3, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_M5")) {
        sram->setMissilePack(SO_MMISSILEPACKS + 4, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_M6")) {
        sram->setMissilePack(SO_MMISSILEPACKS + 5, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_MISSILE_M7")) {
        sram->setMissilePack(SO_MMISSILEPACKS + 6, event.IsChecked());
    } else {
        // ensure we have maridia pack 8 if we're here
        wxASSERT(event.GetId() == XRCID("IDC_PICKUPS_MISSILE_M8"));
        
        sram->setMissilePack(SO_MMISSILEPACKS + 7, event.IsChecked());
    }
    
    wxTextCtrl *ctrl = XRCCTRL(*this, "IDT_GAME_MISSILES", wxTextCtrl);
    unsigned long value;
    
    if (ctrl->GetValue().ToULong(&value)) {
        unsigned char max = sram->getMaxMissiles();
        
        if (value > max) {
            ignoreTextEvents = true;
            ctrl->SetValue(wxString::Format(wxT("%d"), max));
            ignoreTextEvents = false;
        }
    }
}

void Frame::onMissilesSpinner(wxSpinEvent &event) {
    // ensure we are only called for the missiles spinner
    wxASSERT(event.GetId() == XRCID("IDSB_GAME_MISSILES"));
    
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    int missiles = event.GetPosition();
    int max = sram->getMaxMissiles();
    
    if (missiles > max) {
        missiles = max;
        
        wxSpinButton *ptr =
            dynamic_cast<wxSpinButton *>(event.GetEventObject());
            
        // ensure we got a wxSpinButton from that cast
        wxASSERT(ptr);
        
        ptr->SetValue(max);
    }
    
    sram->setMissiles(missiles);
    
    ignoreTextEvents = true;
    XRCCTRL(*this, "IDT_GAME_MISSILES",
        wxTextCtrl)->SetValue(wxString::Format(wxT("%d"), missiles));
    ignoreTextEvents = false;
}

void Frame::onMissilesChange(wxCommandEvent &event) {
    // ensure we are only called for the missiles text control
    wxASSERT(event.GetId() == XRCID("IDT_GAME_MISSILES"));
    
    if (!ignoreTextEvents) {
        // ensure we have an open SRAMFile
        wxASSERT(sram);
    
        unsigned long value;
        
        if (event.GetString().ToULong(&value)) {
            unsigned char missiles = static_cast<unsigned char>(value);
            unsigned char max = sram->getMaxMissiles();
            
            if (value > max) {
                missiles = max;
                
                GetStatusBar()->SetStatusText(wxString::Format
                    (wxT("Warning: Samus' missile capacity is %d"), max));
                timer.Start(CLEAR_DELAY, true);
            }
            
            sram->setMissiles(missiles);
            
            XRCCTRL(*this, "IDSB_GAME_MISSILES",
                wxSpinButton)->SetValue(missiles);
        }
    }
}

void Frame::onNotebookUpdate(wxUpdateUIEvent &event) {
    wxNotebook *notebook = dynamic_cast<wxNotebook *>(event.GetEventObject());
    
    // ensure we got a notebook on the cast
    wxASSERT(notebook);
    
    notebook->Show(sram);
    
    if (!sram) {
        notebook->SetSelection(0);
    }
}

void Frame::onPowerBombPackChange(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    if (event.GetId() == XRCID("IDC_PICKUPS_PBOMB_C")) {
        sram->setPowerBombPack(SO_CPBOMBPACKS, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_PBOMB_BS1")) {
        sram->setPowerBombPack(SO_BSPBOMBPACKS, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_PBOMB_BS2")) {
        sram->setPowerBombPack(SO_BSPBOMBPACKS + 1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_PBOMB_BS3")) {
        sram->setPowerBombPack(SO_BSPBOMBPACKS + 2, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_PBOMB_BS4")) {
        sram->setPowerBombPack(SO_BSPBOMBPACKS + 3, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_PBOMB_BS5")) {
        sram->setPowerBombPack(SO_BSPBOMBPACKS + 4, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_PBOMB_NF1")) {
        sram->setPowerBombPack(SO_NFPBOMBPACKS, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_PBOMB_NF2")) {
        sram->setPowerBombPack(SO_NFPBOMBPACKS + 1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_PBOMB_NF3")) {
        sram->setPowerBombPack(SO_NFPBOMBPACKS + 2, event.IsChecked());
    } else {
        // ensure we have the maridia power bomb pack if we're here
        wxASSERT(event.GetId() == XRCID("IDC_PICKUPS_PBOMB_M"));
        
        sram->setPowerBombPack(SO_MPBOMBPACKS, event.IsChecked());
    }
    
    wxTextCtrl *ctrl = XRCCTRL(*this, "IDT_GAME_PBOMBS", wxTextCtrl);
    unsigned long value;
    
    if (ctrl->GetValue().ToULong(&value)) {
        unsigned char max = sram->getMaxPowerBombs();
        
        if (value > max) {
            ignoreTextEvents = true;
            ctrl->SetValue(wxString::Format(wxT("%d"), max));
            ignoreTextEvents = false;
        }
    }
}

void Frame::onPowerBombsSpinner(wxSpinEvent &event) {
    // ensure we are only called for the power bombs spinner
    wxASSERT(event.GetId() == XRCID("IDSB_GAME_PBOMBS"));
    
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    int bombs = event.GetPosition();
    int max = sram->getMaxPowerBombs();
    
    if (bombs > max) {
        bombs = max;
        
        wxSpinButton *ptr =
            dynamic_cast<wxSpinButton *>(event.GetEventObject());
            
        // ensure we got a wxSpinButton from that cast
        wxASSERT(ptr);
            
        ptr->SetValue(max);
    }
    
    sram->setPowerBombs(bombs);
    
    ignoreTextEvents = true;
    XRCCTRL(*this, "IDT_GAME_PBOMBS",
        wxTextCtrl)->SetValue(wxString::Format(wxT("%d"), bombs));
    ignoreTextEvents = false;
}

void Frame::onPowerBombsChange(wxCommandEvent &event) {
    // ensure we are only called for the power bombs text control
    wxASSERT(event.GetId() == XRCID("IDT_GAME_PBOMBS"));
    
    if (!ignoreTextEvents) {
        // ensure we have an open SRAMFile
        wxASSERT(sram);
    
        unsigned long value;
        
        if (event.GetString().ToULong(&value)) {
            unsigned char bombs = static_cast<unsigned char>(value);
            unsigned char max = sram->getMaxPowerBombs();
            
            if (value > max) {
                bombs = max;
                
                GetStatusBar()->SetStatusText(wxString::Format
                    (wxT("Warning: Samus' power bomb capacity is %d"), max));
                timer.Start(CLEAR_DELAY, true);
            }
            
            sram->setPowerBombs(bombs);
            
            XRCCTRL(*this, "IDSB_GAME_PBOMBS",
                wxSpinButton)->SetValue(bombs);
        }
    }
}

void Frame::onRedDoorChange(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    if (event.GetId() == XRCID("IDC_REDDOOR_C_MAP")) {
        sram->setDoor(RD_C_MAP, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_C_BOMBS")) {
        sram->setDoor(RD_C_BOMBS, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_C_TELEVATOR")) {
        sram->setDoor(RD_C_TELEVATOR, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_BS1")) {
        sram->setDoor(RD_BS1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_BS2")) {
        sram->setDoor(RD_BS2, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_BS3")) {
        sram->setDoor(RD_BS3, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_BS4")) {
        sram->setDoor(RD_BS4, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_BS5")) {
        sram->setDoor(RD_BS5, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_BS6")) {
        sram->setDoor(RD_BS6, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_BS_MAP")) {
        sram->setDoor(RD_BS_MAP, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_BS_RTANK")) {
        sram->setDoor(RD_BS_RTANK, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_BS_SPORESPAWN")) {
        sram->setDoor(RD_BS_SPORESPAWN, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_BS_XRAYSCOPE")) {
        sram->setDoor(RD_BS_XRAYSCOPE, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_NF1")) {
        sram->setDoor(RD_NF1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_NF2")) {
        sram->setDoor(RD_NF2, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_NF3")) {
        sram->setDoor(RD_NF3, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_NF4")) {
        sram->setDoor(RD_NF4, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_NF_HJBOOTS")) {
        sram->setDoor(RD_NF_HJBOOTS, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_NF_SPEEDBOOSTER")) {
        sram->setDoor(RD_NF_SPEEDBOOSTER, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_NF_WAVEBEAM")) {
        sram->setDoor(RD_NF_WAVEBEAM, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_WS_RTANK")) {
        sram->setDoor(RD_WS_RTANK, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_M1")) {
        sram->setDoor(RD_M1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_M2")) {
        sram->setDoor(RD_M2, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_M3")) {
        sram->setDoor(RD_M3, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_M4")) {
        sram->setDoor(RD_M4, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_M5")) {
        sram->setDoor(RD_M5, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_M6")) {
        sram->setDoor(RD_M6, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_M_MAP")) {
        sram->setDoor(RD_M_MAP, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_REDDOOR_T1")) {
        sram->setDoor(RD_T1, event.IsChecked());
    } else {
        // ensure we have the red door to mother brain if we're here
        wxASSERT(event.GetId() == XRCID("IDC_REDDOOR_T_MB"));
        
        sram->setDoor(RD_T_MB, event.IsChecked());
    }
}

void Frame::onReserveTankChange(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    if (event.GetId() == XRCID("IDC_ITEMS_RTANK_BS")) {
        sram->setReserveTank(SO_BSRTANK, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_RTANK_NF")) {
        sram->setReserveTank(SO_NFRTANK, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_ITEMS_RTANK_WS")) {
        sram->setReserveTank(SO_WSRTANK, event.IsChecked());
    } else {
        // ensure we have the maridia reserve tank if we're here
        wxASSERT(event.GetId() == XRCID("IDC_ITEMS_RTANK_M"));
        
        sram->setReserveTank(SO_MRTANK, event.IsChecked());
    }
    
    wxTextCtrl *ctrl = XRCCTRL(*this, "IDT_GAME_RESERVE", wxTextCtrl);
    unsigned long value;
    
    if (ctrl->GetValue().ToULong(&value)) {
        wxUint16 max = sram->getMaxReserveEnergy();
        
        if (value > max) {
            ignoreTextEvents = true;
            ctrl->SetValue(wxString::Format(wxT("%d"), max));
            ignoreTextEvents = false;
        }
    }
}

void Frame::onSaveAreaChange(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    enum Area area = static_cast<enum Area>(event.GetSelection());
    sram->setSavePoint(area);
    
    XRCCTRL(*this, "IDR_GAME_SP1", wxRadioButton)->SetValue(true);
}

void Frame::onSavePointChange(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    std::pair<enum Area, int> sp = sram->getSavePoint();
    
    if (event.GetId() == XRCID("IDR_GAME_SP1")) {
        sp.second = 0;
    } else if (event.GetId() == XRCID("IDR_GAME_SP2")) {
        sp.second = 1;
    } else if (event.GetId() == XRCID("IDR_GAME_SP3")) {
        sp.second = 2;
    } else if (event.GetId() == XRCID("IDR_GAME_SP4")) {
        sp.second = 3;
    } else if (event.GetId() == XRCID("IDR_GAME_SP5")) {
        sp.second = 4;
    } else {
        // ensure we have SP6 at this point
        wxASSERT(event.GetId() == XRCID("IDR_GAME_SP6"));
        
        sp.second = 5;
    }
    
    sram->setSavePoint(sp.first, sp.second);
}

void Frame::onSavePointUpdate(wxUpdateUIEvent &event) {
    if (sram) {
        int points = 0;
        std::pair<enum Area, int> save = sram->getSavePoint();
        
        switch (save.first) {
            case AREA_C:
                points = C_SAVEPOINTS;
                break;
                
            case AREA_BS:
                points = BS_SAVEPOINTS;
                break;
                
            case AREA_NF:
                points = NF_SAVEPOINTS;
                break;
                
            case AREA_WS:
                points = WS_SAVEPOINTS;
                break;
                
            case AREA_M:
                points = M_SAVEPOINTS;
                break;
                
            case AREA_T:
                points = T_SAVEPOINTS;
                break;
        }
        
        int point;
        
        if (event.GetId() == XRCID("IDR_GAME_SP1")) {
            point = 0;
        } else if (event.GetId() == XRCID("IDR_GAME_SP2")) {
            point = 1;
        } else if (event.GetId() == XRCID("IDR_GAME_SP3")) {
            point = 2;
        } else if (event.GetId() == XRCID("IDR_GAME_SP4")) {
            point = 3;
        } else if (event.GetId() == XRCID("IDR_GAME_SP5")) {
            point = 4;
        } else {
            // ensure we have SP6
            wxASSERT(event.GetId() == XRCID("IDR_GAME_SP6"));
            
            point = 5;
        }
        
        event.Enable(point < points);
    }
}

void Frame::onStatueChange(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    if (event.GetId() == XRCID("IDC_GAME_STATUE_KRAID")) {
        sram->setStatue(STATUE_KRAID, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GAME_STATUE_PHANTOON")) {
        sram->setStatue(STATUE_PHANTOON, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_GAME_STATUE_DRAYGON")) {
        sram->setStatue(STATUE_DRAYGON, event.IsChecked());
    } else {
        // ensure we have ridley's statue
        wxASSERT(event.GetId() == XRCID("IDC_GAME_STATUE_RIDLEY"));
        
        sram->setStatue(STATUE_RIDLEY, event.IsChecked());
    }
    
    if (event.IsChecked()) {
        if (sram->getStatue(STATUE_KRAID) && sram->getStatue(STATUE_PHANTOON) &&
            sram->getStatue(STATUE_DRAYGON) && sram->getStatue(STATUE_RIDLEY)) {
            sram->setMiscBit(MB_TELEVATOR);
        }
    } else {
        sram->setMiscBit(MB_TELEVATOR, false);
    }
}

void Frame::onStatueUpdate(wxUpdateUIEvent &event) {
    if (sram) {
        bool value;
        
        if (event.GetId() == XRCID("IDC_GAME_STATUE_KRAID")) {
            value = sram->getBoss(BOSS_KRAID);
        } else if (event.GetId() == XRCID("IDC_GAME_STATUE_PHANTOON")) {
            value = sram->getBoss(BOSS_PHANTOON);
        } else if (event.GetId() == XRCID("IDC_GAME_STATUE_DRAYGON")) {
            value = sram->getBoss(BOSS_DRAYGON);
        } else {
            // ensure we have Ridley's statue
            wxASSERT(event.GetId() == XRCID("IDC_GAME_STATUE_RIDLEY"));
            
            value = sram->getBoss(BOSS_RIDLEY);
        }
        
        event.Enable(value);
        
        if (!value) {
            event.Check(false);
        }
    }
}

void Frame::onStatusBarTimer(wxTimerEvent &) {
    GetStatusBar()->SetStatusText(wxEmptyString);
}

void Frame::onSuperMissilePackChange(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    if (event.GetId() == XRCID("IDC_PICKUPS_SMISSILE_C")) {
        sram->setSuperMissilePack(SO_CSMISSILEPACKS, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_SMISSILE_BS1")) {
        sram->setSuperMissilePack(SO_BSSMISSILEPACKS, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_SMISSILE_BS2")) {
        sram->setSuperMissilePack(SO_BSSMISSILEPACKS + 1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_SMISSILE_BS3")) {
        sram->setSuperMissilePack(SO_BSSMISSILEPACKS + 2, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_SMISSILE_NF")) {
        sram->setSuperMissilePack(SO_NFSMISSILEPACKS, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_SMISSILE_WS1")) {
        sram->setSuperMissilePack(SO_WSSMISSILEPACKS, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_SMISSILE_WS2")) {
        sram->setSuperMissilePack(SO_WSSMISSILEPACKS + 1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_SMISSILE_M1")) {
        sram->setSuperMissilePack(SO_MSMISSILEPACKS, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_PICKUPS_SMISSILE_M2")) {
        sram->setSuperMissilePack(SO_MSMISSILEPACKS + 1, event.IsChecked());
    } else {
        // ensure we have maridia 3 super missile pack if we're here
        wxASSERT(event.GetId() == XRCID("IDC_PICKUPS_SMISSILE_M3"));
        
        sram->setSuperMissilePack(SO_MSMISSILEPACKS + 2, event.IsChecked());
    }
    
    wxTextCtrl *ctrl = XRCCTRL(*this, "IDT_GAME_SMISSILES", wxTextCtrl);
    unsigned long value;
    
    if (ctrl->GetValue().ToULong(&value)) {
        unsigned char max = sram->getMaxSuperMissiles();
        
        if (value > max) {
            ignoreTextEvents = true;
            ctrl->SetValue(wxString::Format(wxT("%d"), max));
            ignoreTextEvents = false;
        }
    }
}

void Frame::onSuperMissilesSpinner(wxSpinEvent &event) {
    // ensure we are only called for the super missiles spinner
    wxASSERT(event.GetId() == XRCID("IDSB_GAME_SMISSILES"));
    
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    int missiles = event.GetPosition();
    int max = sram->getMaxSuperMissiles();
    
    if (missiles > max) {
        missiles = max;
        
        wxSpinButton *ptr =
            dynamic_cast<wxSpinButton *>(event.GetEventObject());
            
        // ensure we got a wxSpinButton from that cast
        wxASSERT(ptr);
            
        ptr->SetValue(max);
    }
    
    sram->setSuperMissiles(missiles);
    
    ignoreTextEvents = true;
    XRCCTRL(*this, "IDT_GAME_SMISSILES",
        wxTextCtrl)->SetValue(wxString::Format(wxT("%d"), missiles));
    ignoreTextEvents = false;
}

void Frame::onSuperMissilesChange(wxCommandEvent &event) {
    // ensure we are only called for the super missiles text control
    wxASSERT(event.GetId() == XRCID("IDT_GAME_SMISSILES"));
    
    if (!ignoreTextEvents) {
        // ensure we have an open SRAMFile
        wxASSERT(sram);
    
        unsigned long value;
        
        if (event.GetString().ToULong(&value)) {
            unsigned char missiles = static_cast<unsigned char>(value);
            unsigned char max = sram->getMaxSuperMissiles();
            
            if (value > max) {
                missiles = max;
                
                GetStatusBar()->SetStatusText(wxString::Format
                    (wxT("Warning: Samus' super missile capacity is %d"), max));
                timer.Start(CLEAR_DELAY, true);
            }
            
            sram->setSuperMissiles(missiles);
            
            XRCCTRL(*this, "IDSB_GAME_SMISSILES",
                wxSpinButton)->SetValue(missiles);
        }
    }
}

void Frame::onWindowClosing(wxCloseEvent &event) {
    // if we can ignore the close and have a modified SRAMFile
    if (event.CanVeto() && sram && sram->isModified()) {
        int answer = wxMessageBox(wxT("Save before exit?"),
                                  wxT("Warning: SRAM has been modified"),
                                  wxYES_NO | wxCANCEL | wxICON_QUESTION);
                                  
        if (answer == wxYES) {
            if (!close(true)) {
                event.Veto();
                return;
            }
        } else if (answer == wxCANCEL) {
            event.Veto();
            return;
        }
    }
    
    Destroy();
}

void Frame::onYellowDoorChange(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    if (event.GetId() == XRCID("IDC_YELLOWDOOR_C1")) {
        sram->setDoor(YD_C1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_YELLOWDOOR_C2")) {
        sram->setDoor(YD_C2, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_YELLOWDOOR_C3")) {
        sram->setDoor(YD_C3, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_YELLOWDOOR_C4")) {
        sram->setDoor(YD_C4, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_YELLOWDOOR_C5")) {
        sram->setDoor(YD_C5, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_YELLOWDOOR_C6")) {
        sram->setDoor(YD_C6, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_YELLOWDOOR_BS1")) {
        sram->setDoor(YD_BS1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_YELLOWDOOR_BS2")) {
        sram->setDoor(YD_BS2, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_YELLOWDOOR_BS3")) {
        sram->setDoor(YD_BS3, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_YELLOWDOOR_BS_XRAYSCOPE")) {
        sram->setDoor(YD_BS_XRAYSCOPE, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_YELLOWDOOR_NF1")) {
        sram->setDoor(YD_NF1, event.IsChecked());
    } else if (event.GetId() == XRCID("IDC_YELLOWDOOR_NF2")) {
        sram->setDoor(YD_NF2, event.IsChecked());
    } else {
        // ensure we have the norfair map yellow door if we're here
        wxASSERT(event.GetId() == XRCID("IDC_YELLOWDOOR_NF_MAP"));
        
        sram->setDoor(YD_NF_MAP, event.IsChecked());
    }
}

void Frame::onZebetitesChange(wxCommandEvent &event) {
    // ensure we have an open SRAMFile
    wxASSERT(sram);
    
    sram->setZebetites(event.GetSelection());
}

BEGIN_EVENT_TABLE(Frame, wxFrame)
    EVT_CHECKBOX(XRCID("IDC_GAME_BOSS_KRAID"), Frame::onBossChange)
    EVT_CHECKBOX(XRCID("IDC_GAME_BOSS_PHANTOON"), Frame::onBossChange)
    EVT_CHECKBOX(XRCID("IDC_GAME_BOSS_DRAYGON"), Frame::onBossChange)
    EVT_CHECKBOX(XRCID("IDC_GAME_BOSS_RIDLEY"), Frame::onBossChange)
    
    EVT_CHECKBOX(XRCID("IDC_GAME_STATUE_KRAID"), Frame::onStatueChange)
    EVT_CHECKBOX(XRCID("IDC_GAME_STATUE_PHANTOON"), Frame::onStatueChange)
    EVT_CHECKBOX(XRCID("IDC_GAME_STATUE_DRAYGON"), Frame::onStatueChange)
    EVT_CHECKBOX(XRCID("IDC_GAME_STATUE_RIDLEY"), Frame::onStatueChange)
    
    EVT_CHECKBOX(XRCID("IDC_GAME_MINIBOSS_SILVERTORIZO"),
        Frame::onMiniBossChange)
    EVT_CHECKBOX(XRCID("IDC_GAME_MINIBOSS_SPORESPAWN"), Frame::onMiniBossChange)
    EVT_CHECKBOX(XRCID("IDC_GAME_MINIBOSS_BOTWOON"), Frame::onMiniBossChange)
    EVT_CHECKBOX(XRCID("IDC_GAME_MINIBOSS_CROCOMIRE"), Frame::onMiniBossChange)
    EVT_CHECKBOX(XRCID("IDC_GAME_MINIBOSS_GOLDENTORIZO"),
        Frame::onMiniBossChange)
        
    EVT_CHECKBOX(XRCID("IDC_GAME_METROIDROOM1"), Frame::onMetroidRoomChange)
    EVT_CHECKBOX(XRCID("IDC_GAME_METROIDROOM2"), Frame::onMetroidRoomChange)
    EVT_CHECKBOX(XRCID("IDC_GAME_METROIDROOM3"), Frame::onMetroidRoomChange)
    EVT_CHECKBOX(XRCID("IDC_GAME_METROIDROOM4"), Frame::onMetroidRoomChange)
        
    EVT_CHECKBOX(XRCID("IDC_GAME_MAP_C"), Frame::onMapChange)
    EVT_CHECKBOX(XRCID("IDC_GAME_MAP_BS"), Frame::onMapChange)
    EVT_CHECKBOX(XRCID("IDC_GAME_MAP_NF"), Frame::onMapChange)
    EVT_CHECKBOX(XRCID("IDC_GAME_MAP_WS"), Frame::onMapChange)
    EVT_CHECKBOX(XRCID("IDC_GAME_MAP_M"), Frame::onMapChange)
    EVT_CHECKBOX(XRCID("IDC_GAME_MAP_T"), Frame::onMapChange)
    
    EVT_CHECKBOX(XRCID("IDC_MISC_MTUBEBROKEN"), Frame::onMiscStatChange)
    EVT_CHECKBOX(XRCID("IDC_MISC_RESCUEDANIMALS"), Frame::onMiscStatChange)
    
    EVT_CHECKBOX(XRCID("IDC_ITEMS_MORPHINGBALL"), Frame::onItemChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_BOMBS"), Frame::onItemChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_SPRINGBALL"), Frame::onItemChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_SCREWATTACK"), Frame::onItemChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_VARIASUIT"), Frame::onItemChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_GRAVITYSUIT"), Frame::onItemChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_HJBOOTS"), Frame::onItemChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_SPACEJUMP"), Frame::onItemChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_SPEEDBOOSTER"), Frame::onItemChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_CHARGEBEAM"), Frame::onItemChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_ICEBEAM"), Frame::onItemChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_WAVEBEAM"), Frame::onItemChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_SPAZER"), Frame::onItemChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_PLASMA"), Frame::onItemChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_XRAYSCOPE"), Frame::onItemChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_GRAPPLINGBEAM"), Frame::onItemChange)
    
    EVT_CHECKBOX(XRCID("IDC_ITEMS_MORPHINGBALL_EQUIPPED"),
        Frame::onItemEquippedChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_BOMBS_EQUIPPED"),
        Frame::onItemEquippedChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_SPRINGBALL_EQUIPPED"),
        Frame::onItemEquippedChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_SCREWATTACK_EQUIPPED"),
        Frame::onItemEquippedChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_VARIASUIT_EQUIPPED"),
        Frame::onItemEquippedChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_GRAVITYSUIT_EQUIPPED"),
        Frame::onItemEquippedChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_HJBOOTS_EQUIPPED"),
        Frame::onItemEquippedChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_SPACEJUMP_EQUIPPED"),
        Frame::onItemEquippedChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_SPEEDBOOSTER_EQUIPPED"),
        Frame::onItemEquippedChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_CHARGEBEAM_EQUIPPED"),
        Frame::onItemEquippedChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_ICEBEAM_EQUIPPED"),
        Frame::onItemEquippedChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_WAVEBEAM_EQUIPPED"),
        Frame::onItemEquippedChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_SPAZER_EQUIPPED"),
        Frame::onItemEquippedChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_PLASMA_EQUIPPED"),
        Frame::onItemEquippedChange)
        
    EVT_CHECKBOX(XRCID("IDC_ITEMS_ET_C1"), Frame::onEnergyTankChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_ET_C2"), Frame::onEnergyTankChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_ET_BS1"), Frame::onEnergyTankChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_ET_BS2"), Frame::onEnergyTankChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_ET_BS3"), Frame::onEnergyTankChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_ET_BS4"), Frame::onEnergyTankChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_ET_BS5"), Frame::onEnergyTankChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_ET_NF1"), Frame::onEnergyTankChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_ET_NF2"), Frame::onEnergyTankChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_ET_NF3"), Frame::onEnergyTankChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_ET_NF4"), Frame::onEnergyTankChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_ET_WS"), Frame::onEnergyTankChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_ET_M1"), Frame::onEnergyTankChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_ET_M2"), Frame::onEnergyTankChange)
    
    EVT_CHECKBOX(XRCID("IDC_ITEMS_RTANK_BS"), Frame::onReserveTankChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_RTANK_NF"), Frame::onReserveTankChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_RTANK_WS"), Frame::onReserveTankChange)
    EVT_CHECKBOX(XRCID("IDC_ITEMS_RTANK_M"), Frame::onReserveTankChange)
    
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_C1"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_C2"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_C3"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_C4"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_C5"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_C6"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_C7"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_C8"), Frame::onMissilePackChange)
    
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_BS1"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_BS2"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_BS3"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_BS4"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_BS5"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_BS6"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_BS7"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_BS8"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_BS9"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_BS10"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_BS11"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_BS12"), Frame::onMissilePackChange)
    
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_NF1"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_NF2"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_NF3"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_NF4"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_NF5"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_NF6"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_NF7"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_NF8"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_NF9"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_NF10"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_NF11"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_NF12"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_NF13"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_NF14"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_NF15"), Frame::onMissilePackChange)
    
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_WS1"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_WS2"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_WS3"), Frame::onMissilePackChange)
    
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_M1"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_M2"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_M3"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_M4"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_M5"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_M6"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_M7"), Frame::onMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_MISSILE_M8"), Frame::onMissilePackChange)
    
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_SMISSILE_C"),
        Frame::onSuperMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_SMISSILE_BS1"),
        Frame::onSuperMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_SMISSILE_BS2"),
        Frame::onSuperMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_SMISSILE_BS3"),
        Frame::onSuperMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_SMISSILE_NF"),
        Frame::onSuperMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_SMISSILE_WS1"),
        Frame::onSuperMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_SMISSILE_WS2"),
        Frame::onSuperMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_SMISSILE_M1"),
        Frame::onSuperMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_SMISSILE_M2"),
        Frame::onSuperMissilePackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_SMISSILE_M3"),
        Frame::onSuperMissilePackChange)
        
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_PBOMB_C"), Frame::onPowerBombPackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_PBOMB_BS1"), Frame::onPowerBombPackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_PBOMB_BS2"), Frame::onPowerBombPackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_PBOMB_BS3"), Frame::onPowerBombPackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_PBOMB_BS4"), Frame::onPowerBombPackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_PBOMB_BS5"), Frame::onPowerBombPackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_PBOMB_NF1"), Frame::onPowerBombPackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_PBOMB_NF2"), Frame::onPowerBombPackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_PBOMB_NF3"), Frame::onPowerBombPackChange)
    EVT_CHECKBOX(XRCID("IDC_PICKUPS_PBOMB_M"), Frame::onPowerBombPackChange)
    
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_C_MAP"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_C_BOMBS"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_C_TELEVATOR"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_BS1"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_BS2"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_BS3"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_BS4"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_BS5"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_BS6"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_BS_MAP"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_BS_RTANK"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_BS_SPORESPAWN"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_BS_XRAYSCOPE"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_NF1"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_NF2"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_NF3"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_NF4"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_NF_HJBOOTS"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_NF_SPEEDBOOSTER"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_NF_WAVEBEAM"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_WS_RTANK"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_M1"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_M2"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_M3"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_M4"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_M5"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_M6"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_M_MAP"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_T1"), Frame::onRedDoorChange)
    EVT_CHECKBOX(XRCID("IDC_REDDOOR_T_MB"), Frame::onRedDoorChange)
    
    EVT_CHECKBOX(XRCID("IDC_GREENDOOR_C1"), Frame::onGreenDoorChange)
    EVT_CHECKBOX(XRCID("IDC_GREENDOOR_C_WS"), Frame::onGreenDoorChange)
    EVT_CHECKBOX(XRCID("IDC_GREENDOOR_BS1"), Frame::onGreenDoorChange)
    EVT_CHECKBOX(XRCID("IDC_GREENDOOR_BS2"), Frame::onGreenDoorChange)
    EVT_CHECKBOX(XRCID("IDC_GREENDOOR_BS3"), Frame::onGreenDoorChange)
    EVT_CHECKBOX(XRCID("IDC_GREENDOOR_BS4"), Frame::onGreenDoorChange)
    EVT_CHECKBOX(XRCID("IDC_GREENDOOR_BS5"), Frame::onGreenDoorChange)
    EVT_CHECKBOX(XRCID("IDC_GREENDOOR_BS6"), Frame::onGreenDoorChange)
    EVT_CHECKBOX(XRCID("IDC_GREENDOOR_BS7"), Frame::onGreenDoorChange)
    EVT_CHECKBOX(XRCID("IDC_GREENDOOR_BS8"), Frame::onGreenDoorChange)
    EVT_CHECKBOX(XRCID("IDC_GREENDOOR_BS_SPAZER"), Frame::onGreenDoorChange)
    EVT_CHECKBOX(XRCID("IDC_GREENDOOR_BS_SPORESPAWNEXIT"),
        Frame::onGreenDoorChange)
    EVT_CHECKBOX(XRCID("IDC_GREENDOOR_NF1"), Frame::onGreenDoorChange)
    EVT_CHECKBOX(XRCID("IDC_GREENDOOR_NF2"), Frame::onGreenDoorChange)
    EVT_CHECKBOX(XRCID("IDC_GREENDOOR_NF3"), Frame::onGreenDoorChange)
    EVT_CHECKBOX(XRCID("IDC_GREENDOOR_NF4"), Frame::onGreenDoorChange)
    EVT_CHECKBOX(XRCID("IDC_GREENDOOR_NF_ICEBEAM"), Frame::onGreenDoorChange)
    EVT_CHECKBOX(XRCID("IDC_GREENDOOR_NF_SPEEDBOOSTER"),
        Frame::onGreenDoorChange)
    EVT_CHECKBOX(XRCID("IDC_GREENDOOR_WS"), Frame::onGreenDoorChange)
    EVT_CHECKBOX(XRCID("IDC_GREENDOOR_M1"), Frame::onGreenDoorChange)
    EVT_CHECKBOX(XRCID("IDC_GREENDOOR_M2"), Frame::onGreenDoorChange)
    EVT_CHECKBOX(XRCID("IDC_GREENDOOR_M3"), Frame::onGreenDoorChange)
    EVT_CHECKBOX(XRCID("IDC_GREENDOOR_M_DRAYGON"), Frame::onGreenDoorChange)
    
    EVT_CHECKBOX(XRCID("IDC_YELLOWDOOR_C1"), Frame::onYellowDoorChange)
    EVT_CHECKBOX(XRCID("IDC_YELLOWDOOR_C2"), Frame::onYellowDoorChange)
    EVT_CHECKBOX(XRCID("IDC_YELLOWDOOR_C3"), Frame::onYellowDoorChange)
    EVT_CHECKBOX(XRCID("IDC_YELLOWDOOR_C4"), Frame::onYellowDoorChange)
    EVT_CHECKBOX(XRCID("IDC_YELLOWDOOR_C5"), Frame::onYellowDoorChange)
    EVT_CHECKBOX(XRCID("IDC_YELLOWDOOR_C6"), Frame::onYellowDoorChange)
    EVT_CHECKBOX(XRCID("IDC_YELLOWDOOR_BS1"), Frame::onYellowDoorChange)
    EVT_CHECKBOX(XRCID("IDC_YELLOWDOOR_BS2"), Frame::onYellowDoorChange)
    EVT_CHECKBOX(XRCID("IDC_YELLOWDOOR_BS3"), Frame::onYellowDoorChange)
    EVT_CHECKBOX(XRCID("IDC_YELLOWDOOR_BS_XRAYSCOPE"),
        Frame::onYellowDoorChange)
    EVT_CHECKBOX(XRCID("IDC_YELLOWDOOR_NF1"), Frame::onYellowDoorChange)
    EVT_CHECKBOX(XRCID("IDC_YELLOWDOOR_NF2"), Frame::onYellowDoorChange)
    EVT_CHECKBOX(XRCID("IDC_YELLOWDOOR_NF_MAP"), Frame::onYellowDoorChange)
    
    EVT_CHECKBOX(XRCID("IDC_EYEDOOR_KRAID"), Frame::onEyeDoorChange)
    EVT_CHECKBOX(XRCID("IDC_EYEDOOR_PHANTOON"), Frame::onEyeDoorChange)
    EVT_CHECKBOX(XRCID("IDC_EYEDOOR_DRAYGON"), Frame::onEyeDoorChange)
    EVT_CHECKBOX(XRCID("IDC_EYEDOOR_RIDLEY"), Frame::onEyeDoorChange)
    EVT_CHECKBOX(XRCID("IDC_EYEDOOR_T"), Frame::onEyeDoorChange)
    
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_C_BOMBSEXIT"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_BS1"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_BS2"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_BS3"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_BS4"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_BS5"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_BS6"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_BS7"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_BS8"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_BS9"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_BS_MAPEXIT"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_BS_OLDTLEFT"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_BS_OLDTRIGHT"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_BS_MINIKRAIDLEFT"),
        Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_BS_MINIKRAIDRIGHT"),
        Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_BS_VARIASUIT"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_BS_KRAIDEXIT"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_NF_HJBOOTSEXIT"),
        Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_NF_CROCOMIREEXIT"),
        Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_NF_SCREWATTACK"),
        Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_NF_GOLDSPACEPIRATES"),
        Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_NF_RIDLEYLEFT"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_NF_RIDLEYEXIT"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_WS1"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_WS2"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_WS3"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_WS4"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_WS_PHANTOONEXIT"),
        Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_M1"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_M2"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_M_BOTWOONEXIT"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_M_SPACEJUMP"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_M_DRAYGONEXIT"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_M_PLASMA"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_M_PLASMAEXIT"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_T1"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_T2"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_T3"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_T4"), Frame::onMetalDoorChange)
    EVT_CHECKBOX(XRCID("IDC_METALDOOR_T5"), Frame::onMetalDoorChange)
    
    EVT_CHOICE(XRCID("IDC_MISC_DASH"), Frame::onControllerChange)
    EVT_CHOICE(XRCID("IDC_MISC_JUMP"), Frame::onControllerChange)
    EVT_CHOICE(XRCID("IDC_MISC_ITEMCANCEL"), Frame::onControllerChange)
    EVT_CHOICE(XRCID("IDC_MISC_SHOT"), Frame::onControllerChange)
    EVT_CHOICE(XRCID("IDC_MISC_ANGLEUP"), Frame::onControllerChange)
    EVT_CHOICE(XRCID("IDC_MISC_ANGLEDOWN"), Frame::onControllerChange)
    EVT_CHOICE(XRCID("IDC_MISC_ITEMSELECT"), Frame::onControllerChange)

    EVT_CLOSE(Frame::onWindowClosing)
    
    EVT_MENU(wxID_OPEN, Frame::onFileOpen)
    EVT_MENU(wxID_CLOSE, Frame::onFileClose)
    EVT_MENU(wxID_SAVE, Frame::onFileSave)
    EVT_MENU(wxID_SAVEAS, Frame::onFileSaveAs)
    EVT_MENU(wxID_EXIT, Frame::onFileExit)
    
    EVT_MENU(XRCID("IDM_GAME_GAMEA"), Frame::onGame)
    EVT_MENU(XRCID("IDM_GAME_GAMEB"), Frame::onGame)
    EVT_MENU(XRCID("IDM_GAME_GAMEC"), Frame::onGame)
    
    EVT_MENU(XRCID("IDM_GAME_ENDING1"), Frame::onGameEnding)
    EVT_MENU(XRCID("IDM_GAME_ENDING2"), Frame::onGameEnding)
    EVT_MENU(XRCID("IDM_GAME_ENDING3"), Frame::onGameEnding)
    
    EVT_MENU(XRCID("IDM_GAME_GIVE_EVERYTHING"), Frame::onGameGiveEverything)
    
    EVT_MENU(XRCID("IDM_GAME_GIVE_ENERGY"), Frame::onGameGiveEnergy)
    EVT_MENU(XRCID("IDM_GAME_GIVE_INVENTORY"), Frame::onGameGiveInventory)
    
    EVT_MENU(XRCID("IDM_GAME_GIVE_BOSSES"), Frame::onGameGiveBosses)
    EVT_MENU(XRCID("IDM_GAME_GIVE_MINIBOSSES"), Frame::onGameGiveMiniBosses)
    EVT_MENU(XRCID("IDM_GAME_GIVE_METROIDROOMS"), Frame::onGameGiveMetroidRooms)
    EVT_MENU(XRCID("IDM_GAME_GIVE_ZEBETITES"), Frame::onGameGiveZebetites)
    
    EVT_MENU(XRCID("IDM_GAME_GIVE_MAPS"), Frame::onGameGiveMaps)
    
    EVT_MENU(XRCID("IDM_GAME_GIVE_ITEMS"), Frame::onGameGiveItems)
    
    EVT_MENU(XRCID("IDM_GAME_GIVE_MISSILES"), Frame::onGameGiveMissilePacks)
    EVT_MENU(XRCID("IDM_GAME_GIVE_SMISSILES"),
        Frame::onGameGiveSuperMissilePacks)
    EVT_MENU(XRCID("IDM_GAME_GIVE_PBOMBS"), Frame::onGameGivePowerBombPacks)
    
    EVT_MENU(XRCID("IDM_GAME_GIVE_ETANKS"), Frame::onGameGiveEnergyTanks)
    EVT_MENU(XRCID("IDM_GAME_GIVE_RTANKS"), Frame::onGameGiveReserveTanks)
    
    EVT_MENU(XRCID("IDM_GAME_GIVE_REDDOORS"), Frame::onGameGiveRedDoors)
    EVT_MENU(XRCID("IDM_GAME_GIVE_GREENDOORS"), Frame::onGameGiveGreenDoors)
    EVT_MENU(XRCID("IDM_GAME_GIVE_YELLOWDOORS"), Frame::onGameGiveYellowDoors)
    EVT_MENU(XRCID("IDM_GAME_GIVE_EYEDOORS"), Frame::onGameGiveEyeDoors)
    EVT_MENU(XRCID("IDM_GAME_GIVE_METALDOORS"), Frame::onGameGiveMetalDoors)
    
    EVT_MENU(XRCID("IDM_GAME_COPYAB"), Frame::onGameCopy)
    EVT_MENU(XRCID("IDM_GAME_COPYAC"), Frame::onGameCopy)
    EVT_MENU(XRCID("IDM_GAME_COPYBA"), Frame::onGameCopy)
    EVT_MENU(XRCID("IDM_GAME_COPYBC"), Frame::onGameCopy)
    EVT_MENU(XRCID("IDM_GAME_COPYCA"), Frame::onGameCopy)
    EVT_MENU(XRCID("IDM_GAME_COPYCB"), Frame::onGameCopy)
    
    EVT_MENU(XRCID("IDM_GAME_CLEARA"), Frame::onGameClear)
    EVT_MENU(XRCID("IDM_GAME_CLEARB"), Frame::onGameClear)
    EVT_MENU(XRCID("IDM_GAME_CLEARC"), Frame::onGameClear)
    
    EVT_MENU(wxID_ABOUT, Frame::onHelpAbout)
    
    EVT_RADIOBOX(XRCID("IDRB_GAME_ZEBETITES"), Frame::onZebetitesChange)
    EVT_RADIOBOX(XRCID("IDRB_GAME_SAVEAREA"), Frame::onSaveAreaChange)
    
    EVT_RADIOBOX(XRCID("IDRB_MISC_LANGUAGE"), Frame::onMiscConfigChange)
    EVT_RADIOBOX(XRCID("IDRB_MISC_MOONWALK"), Frame::onMiscConfigChange)
    EVT_RADIOBOX(XRCID("IDRB_MISC_ICONCANCEL"), Frame::onMiscConfigChange)
    
    EVT_RADIOBUTTON(XRCID("IDR_GAME_SP1"), Frame::onSavePointChange)
    EVT_RADIOBUTTON(XRCID("IDR_GAME_SP2"), Frame::onSavePointChange)
    EVT_RADIOBUTTON(XRCID("IDR_GAME_SP3"), Frame::onSavePointChange)
    EVT_RADIOBUTTON(XRCID("IDR_GAME_SP4"), Frame::onSavePointChange)
    EVT_RADIOBUTTON(XRCID("IDR_GAME_SP5"), Frame::onSavePointChange)
    EVT_RADIOBUTTON(XRCID("IDR_GAME_SP6"), Frame::onSavePointChange)
    
    EVT_SPIN(XRCID("IDSB_GAME_MISSILES"), Frame::onMissilesSpinner)
    EVT_SPIN(XRCID("IDSB_GAME_SMISSILES"), Frame::onSuperMissilesSpinner)
    EVT_SPIN(XRCID("IDSB_GAME_PBOMBS"), Frame::onPowerBombsSpinner)
    
    EVT_TEXT(XRCID("IDT_GAME_ENERGY"), Frame::onEnergyChange)
    EVT_TEXT(XRCID("IDT_GAME_RESERVE"), Frame::onEnergyChange)
    
    EVT_TEXT(XRCID("IDT_GAME_MISSILES"), Frame::onMissilesChange)
    EVT_TEXT(XRCID("IDT_GAME_SMISSILES"), Frame::onSuperMissilesChange)
    EVT_TEXT(XRCID("IDT_GAME_PBOMBS"), Frame::onPowerBombsChange)
    
    EVT_TEXT(XRCID("IDT_MISC_GAMETIME"), Frame::onGameTimeChange)
    
    EVT_TIMER(ID_SBTIMER, Frame::onStatusBarTimer)
    
    EVT_UPDATE_UI(wxID_CLOSE, Frame::onFileCloseUpdate)
    EVT_UPDATE_UI(wxID_SAVE, Frame::onFileSaveUpdate)
    EVT_UPDATE_UI(wxID_SAVEAS, Frame::onFileSaveUpdate)
    
    EVT_UPDATE_UI(XRCID("IDM_GAME_GAMEA"), Frame::onGameUpdate)
    EVT_UPDATE_UI(XRCID("IDM_GAME_GAMEB"), Frame::onGameUpdate)
    EVT_UPDATE_UI(XRCID("IDM_GAME_GAMEC"), Frame::onGameUpdate)
    
    EVT_UPDATE_UI(XRCID("IDM_GAME_ENDING"), Frame::onGameEndingUpdate)
    EVT_UPDATE_UI(XRCID("IDM_GAME_GIVE"), Frame::onGameGiveUpdate)
    
    EVT_UPDATE_UI(XRCID("IDM_GAME_COPY"), Frame::onGameCopyUpdate)
    EVT_UPDATE_UI(XRCID("IDM_GAME_COPYA"), Frame::onGameCopyUpdate)
    EVT_UPDATE_UI(XRCID("IDM_GAME_COPYB"), Frame::onGameCopyUpdate)
    EVT_UPDATE_UI(XRCID("IDM_GAME_COPYC"), Frame::onGameCopyUpdate)
    
    EVT_UPDATE_UI(XRCID("IDM_GAME_CLEAR"), Frame::onGameClearUpdate)
    EVT_UPDATE_UI(XRCID("IDM_GAME_CLEARA"), Frame::onGameClearUpdate)
    EVT_UPDATE_UI(XRCID("IDM_GAME_CLEARB"), Frame::onGameClearUpdate)
    EVT_UPDATE_UI(XRCID("IDM_GAME_CLEARC"), Frame::onGameClearUpdate)
    
    EVT_UPDATE_UI(XRCID("IDN_SMSE"), Frame::onNotebookUpdate)
    
    EVT_UPDATE_UI(XRCID("IDC_GAME_STATUE_KRAID"), Frame::onStatueUpdate)
    EVT_UPDATE_UI(XRCID("IDC_GAME_STATUE_PHANTOON"), Frame::onStatueUpdate)
    EVT_UPDATE_UI(XRCID("IDC_GAME_STATUE_DRAYGON"), Frame::onStatueUpdate)
    EVT_UPDATE_UI(XRCID("IDC_GAME_STATUE_RIDLEY"), Frame::onStatueUpdate)
    
    EVT_UPDATE_UI(XRCID("IDR_GAME_SP1"), Frame::onSavePointUpdate)
    EVT_UPDATE_UI(XRCID("IDR_GAME_SP2"), Frame::onSavePointUpdate)
    EVT_UPDATE_UI(XRCID("IDR_GAME_SP3"), Frame::onSavePointUpdate)
    EVT_UPDATE_UI(XRCID("IDR_GAME_SP4"), Frame::onSavePointUpdate)
    EVT_UPDATE_UI(XRCID("IDR_GAME_SP5"), Frame::onSavePointUpdate)
    EVT_UPDATE_UI(XRCID("IDR_GAME_SP6"), Frame::onSavePointUpdate)
    
    EVT_UPDATE_UI(XRCID("IDC_ITEMS_MORPHINGBALL_EQUIPPED"),
        Frame::onItemEquippedUpdate)
    EVT_UPDATE_UI(XRCID("IDC_ITEMS_BOMBS_EQUIPPED"),
        Frame::onItemEquippedUpdate)
    EVT_UPDATE_UI(XRCID("IDC_ITEMS_SPRINGBALL_EQUIPPED"),
        Frame::onItemEquippedUpdate)
    EVT_UPDATE_UI(XRCID("IDC_ITEMS_SCREWATTACK_EQUIPPED"),
        Frame::onItemEquippedUpdate)
    EVT_UPDATE_UI(XRCID("IDC_ITEMS_VARIASUIT_EQUIPPED"),
        Frame::onItemEquippedUpdate)
    EVT_UPDATE_UI(XRCID("IDC_ITEMS_GRAVITYSUIT_EQUIPPED"),
        Frame::onItemEquippedUpdate)
    EVT_UPDATE_UI(XRCID("IDC_ITEMS_HJBOOTS_EQUIPPED"),
        Frame::onItemEquippedUpdate)
    EVT_UPDATE_UI(XRCID("IDC_ITEMS_SPACEJUMP_EQUIPPED"),
        Frame::onItemEquippedUpdate)
    EVT_UPDATE_UI(XRCID("IDC_ITEMS_SPEEDBOOSTER_EQUIPPED"),
        Frame::onItemEquippedUpdate)
    EVT_UPDATE_UI(XRCID("IDC_ITEMS_CHARGEBEAM_EQUIPPED"),
        Frame::onItemEquippedUpdate)
    EVT_UPDATE_UI(XRCID("IDC_ITEMS_ICEBEAM_EQUIPPED"),
        Frame::onItemEquippedUpdate)
    EVT_UPDATE_UI(XRCID("IDC_ITEMS_WAVEBEAM_EQUIPPED"),
        Frame::onItemEquippedUpdate)
    EVT_UPDATE_UI(XRCID("IDC_ITEMS_SPAZER_EQUIPPED"),
        Frame::onItemEquippedUpdate)
    EVT_UPDATE_UI(XRCID("IDC_ITEMS_PLASMA_EQUIPPED"),
        Frame::onItemEquippedUpdate)
END_EVENT_TABLE()

IMPLEMENT_CLASS(Frame, wxFrame)

