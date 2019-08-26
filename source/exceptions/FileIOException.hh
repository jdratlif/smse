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
 
// $Id: FileIOException.hh,v 1.3 2008/12/17 04:14:15 jdratlif Exp $

#ifndef SMSE_FILEIOEXCEPTION_HH_
#define SMSE_FILEIOEXCEPTION_HH_

#include <stdexcept>

namespace smse {
    /// Exception thrown when File IO goes awry
    class FileIOException : public std::runtime_error {
    public:
        /**
         * Creates a new FileIOException.
         *
         * @param message The error message.
         */
        FileIOException(const std::string &message);
    };
    
    inline FileIOException::FileIOException(const std::string &message) :
        std::runtime_error(message) {}
}

#endif

