/* Copyright STIFTELSEN SINTEF 2012
 * 
 * This file is part of the Tinia Framework.
 * 
 * The Tinia Framework is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * The Tinia Framework is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with the Tinia Framework.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include <string>

namespace tinia {
namespace jobcontroller {
class Controller
{
public:
    Controller();
    
    virtual void addScript(const std::string& script) = 0;

    virtual
    void
    finish() = 0;

    virtual
    void
    fail() = 0;

    virtual int run(int argc, char** argv) = 0;
    
protected:
    void  (*m_logger_callback)( void* logger_data, int level, const char* who, const char* msg, ... );
    void*   m_logger_data;
    
};

}
}
