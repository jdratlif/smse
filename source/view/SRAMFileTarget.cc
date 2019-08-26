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
 
// $Id: SRAMFileTarget.cc,v 1.1 2005/10/15 03:13:48 technoplaza Exp $

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "model/SRAMFile.hh"
#include "view/Frame.hh"
#include "view/SRAMFileTarget.hh"

using namespace smse;

bool SRAMFileTarget::OnDropFiles(wxCoord, wxCoord, const wxArrayString &files) {
    if (files.GetCount() > 0) {
        // save current SRAMFile
        if (frame->sram && frame->sram->isModified()) {
            int answer = wxMessageBox(wxT("Save current SRAM?"),
                wxT("Warning: Unsaved SRAM file"),
                wxYES_NO | wxCANCEL | wxICON_QUESTION);
            
            if (answer == wxYES) {
                if (!frame->save()) {
                    return false;
                }
            } else if (answer == wxCANCEL) {
                return false;
            }
        }
        
        // open the first file
        frame->open(files[0]);
    }
    
    return true;
}

