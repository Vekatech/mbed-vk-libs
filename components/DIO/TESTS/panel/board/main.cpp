/*
 * Copyright (c) 2020 Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "unity.h"
#include "utest.h"
#include "greentea-client/test_env.h"
#include "Dio.hpp"

using namespace utest::v1;

#ifndef TARGET_VK_RZ_A1H
#error [NOT_SUPPORTED] No DIO support for this target.
#else

#ifndef MBED_TEST_TIMEOUT
#define MBED_TEST_TIMEOUT 120
#endif

#define WAIT_STIMUL_TIME  30 // Wait X sec for stimuls on the input

Dio PLC;
static control_t OUT_test(const size_t call_count)
{
    int pass = 6, OUT = 1;
    while(pass)
    {
        for(int i = 0; i < 6; i++)
        {
            PLC.Out[i] = OUT;
            ThisThread::sleep_for(200);
        }
        
        PLC.Rly[0] = OUT;
        ThisThread::sleep_for(200);
        PLC.Rly[1] = OUT;
        ThisThread::sleep_for(1200);
        OUT = OUT? 0 : 1;
        pass--;
    }

    TEST_ASSERT_EQUAL(0, 0);

    return CaseNext;
}

static control_t IN_test(const size_t call_count)
{
    int IN[8];
    uint8_t tst = 0;
    uint64_t pass = 0ULL, stamp = Kernel::get_ms_count();

    while (pass < (WAIT_STIMUL_TIME*1000))
    {
        for(int i = 0; i < 8; i++)
        {
            if(IN[i] ^ PLC.In[i])
            {
                if(i < 6)
                    PLC.Out[i] = IN[i] = PLC.In[i];
                else
                    PLC.Rly[i-6] = IN[i] = PLC.In[i];

                if(IN[i])
                   tst |= (1<<i);
            }
        }
        pass = Kernel::get_ms_count() - stamp;
    }

    TEST_ASSERT_EQUAL(0xFF, tst);

    return CaseNext;
}

utest::v1::status_t PANEL_setup(const size_t number_of_cases)
{
    GREENTEA_SETUP(MBED_TEST_TIMEOUT, "default_auto");

    //return greentea_test_setup_handler(number_of_cases);
    return verbose_test_setup_handler(number_of_cases);
}

Case cases[] = {
    Case("DIO test OUT", OUT_test),
    Case("DIO test IN", IN_test)
};

Specification specification(PANEL_setup, cases);

int main()
{
    return !Harness::run(specification);
}

#endif // !defined(TARGET_VK_RZ_A1H)