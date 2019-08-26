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
 
// $Id: App.hh,v 1.3 2008/12/17 04:14:15 jdratlif Exp $

#ifndef SMSE_APP_HH_
#define SMSE_APP_HH_

/// a namespace for all the classes used by smse
namespace smse {
    class Frame;
    
    /// The application class and starting point.
    class App : public wxApp {
        DECLARE_CLASS(App)
        
    private:
        Frame *frame;
        
    public:
        /**
         * Called to initialize the application.
         *
         * @return true on successful initialization; false otherwise.
         */
        virtual bool OnInit();
        
        /**
         * Called to run the application.
         *
         * @return The exit code.
         */
        virtual int OnRun();
    };
    
    DECLARE_APP(App)
}

#endif

