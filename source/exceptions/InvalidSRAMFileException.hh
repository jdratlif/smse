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
 
// $Id: InvalidSRAMFileException.hh,v 1.1 2005/10/07 13:01:25 technoplaza Exp $

#ifndef _SMSE_INVALIDSRAMFILEEXCEPTION_HH_
#define _SMSE_INVALIDSRAMFILEEXCEPTION_HH_

#include <stdexcept>

namespace smse {
    /// Exception thrown when a file is not a valid SRAM file.
    class InvalidSRAMFileException : public std::runtime_error {
    public:
        /**
         * Creates a new InvalidSRAMFileException.
         *
         * @param message The error message.
         */
        InvalidSRAMFileException(const std::string &message);
    };
    
    inline InvalidSRAMFileException::
        InvalidSRAMFileException(const std::string& message) : 
        std::runtime_error(message) {}
}

#endif

