/*
 * This file is part of the tumanako_vc project.
 *
 * Copyright (C) 2018 Johannes Huebner <dev@johanneshuebner.com>
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

#ifndef customBMS_H
#define customBMS_H
#include "bms.h"

class CustomBMS: public BMS
{
public:
    void SetCanInterface(CanHardware* c) override;
    void DecodeCAN(int id, uint8_t* data) override;
    float MaxChargeCurrent() override;
    void Task100Ms() override;
private:
    bool BMSDataValid();
    bool ChargeAllowed();
    float temperature(uint16_t adc);
    int timeoutCounter;
    float minTempC = 0;
    float maxTempC = 0;
    float avgTempC = 0;
};

#endif // CUSTOMBMS_H
