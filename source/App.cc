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
 
// $Id: App.cc,v 1.1 2005/10/06 22:48:30 technoplaza Exp $

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/xrc/xmlres.h>

#include "AppConstants.hh"
#include "App.hh"
#include "view/Frame.hh"

using namespace smse;

const wxString smse::APP_NAME(wxT("Super Metroid SRAM Editor"));
const wxString smse::APP_VERSION(wxT(PACKAGE_VERSION));
const wxString smse::APP_COPYRIGHT(wxT("Copyright (C) 2005 emuWorks"));
const wxString smse::APP_URL(wxT("http://games.technoplaza.net/"));

// prototype for InitXmlResource function
void InitXmlResource();

bool App::OnInit() {
    // initialize the XRC resources
    wxXmlResource::Get()->InitAllHandlers();
    InitXmlResource();
    
    Frame *frame = new Frame;
    frame->Show();
    
    return true;
}

IMPLEMENT_CLASS(App, wxApp)
IMPLEMENT_APP(App)

