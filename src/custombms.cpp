/*
 * This file is part of the tumanako_vc project.
 *
 * Copyright (C) 2018 Johannes Huebner <dev@johanneshuebner.com>
 *               2024 Daniel Öster <info@dalasevrepair.fi>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "custombms.h"
#include <math.h>

 /*
  * This module receives messages from a custom BMS 
  * It also implements a timeout to indicate whether the BMS is actively
  * sending data or not. This data can be used to safely stop any charging
  * process if the BMS is not working correctly.
  */

void CustomBMS::SetCanInterface(CanHardware* c)
{
    can = c;
    can->RegisterUserMessage(0x4f1); // Primary BMS
}

bool CustomBMS::BMSDataValid() {
    // Return false if BMS is not sending data.
    if (timeoutCounter < 1) return false;

    return true;
}

// Return whether charging is currently permitted.
bool CustomBMS::ChargeAllowed()
{
    if (!BMSDataValid()) return false;

    // Refuse to charge if the voltage or temperature is out of range.
    if (maxTempC > Param::GetFloat(Param::BMS_TmaxLimit)) return false;
    if (minTempC < Param::GetFloat(Param::BMS_TminLimit)) return false;

    // Otherwise, charging is permitted.
    return true;
}


float CustomBMS::MaxChargeCurrent() //BMS can command an AC charge shutdown if its current limit is 0
{
    if (!ChargeAllowed()) return 0;
    return 9998.0;
}

// Process voltage and temperature message from TI Daisychain BMS.
void CustomBMS::DecodeCAN(int id, uint8_t* data)
{
    if (id != 0x4f1) return;

    avgTempC = static_cast<int8_t>(data[0]);
    minTempC = static_cast<int8_t>(data[1]);
    maxTempC = static_cast<int8_t>(data[2]);

    timeoutCounter = Param::GetInt(Param::BMS_Timeout) * 10;

}

void CustomBMS::Task100Ms()
{
    // Decrement timeout counters.
    if (timeoutCounter > 0) timeoutCounter--;

    // Update informational parameters.
    Param::SetInt(Param::BMS_ChargeLim, MaxChargeCurrent());

    if (BMSDataValid()) {
        Param::SetFloat(Param::BMS_Tmin, minTempC);
        Param::SetFloat(Param::BMS_Tmax, maxTempC);
        Param::SetFloat(Param::BMS_Tavg, avgTempC);
    }
    else
    {
        Param::SetFloat(Param::BMS_Tmin, -100);
        Param::SetFloat(Param::BMS_Tmax, -100);
        Param::SetFloat(Param::BMS_Tavg, -100);
    }
}
