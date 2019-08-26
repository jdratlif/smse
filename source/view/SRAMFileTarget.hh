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
 
// $Id: SRAMFileTarget.hh,v 1.1 2005/10/15 03:13:48 technoplaza Exp $

#ifndef _SMSE_SRAMFILETARGET_HH_
#define _SMSE_SRAMFILETARGET_HH_

#include <wx/dnd.h>

namespace smse {
    class Frame;
    
    /// Class to implement a drop target for SRAM files on the Frame
    class SRAMFileTarget : public wxFileDropTarget {
    private:
        Frame *frame;
        
    public:
        /**
         * Creates a new SRAMFileTarget.
         *
         * @param frame The Frame to send files to.
         */
        SRAMFileTarget(Frame *frame);
        
        /**
         * Called when files are dropped on the associated Frame.
         *
         * @param x The x-coordinate of the drop (unused).
         * @param y The y-coordinate of the drop (unused).
         * @param files The filenames.
         *
         * @return true if the drop was accepted; false otherwise.
         */
        virtual bool OnDropFiles(wxCoord x, wxCoord y,
            const wxArrayString &files);
    };
    
    inline SRAMFileTarget::SRAMFileTarget(Frame *frame) : frame(frame) {}
}

#endif

