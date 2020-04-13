/*
* SPDX-License-Identifier: Apache-2.0
* Copyright 2019 Western Digital Corporation or its affiliates.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http:*www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
/**
* @file   psp_nmi_eh1.c
* @author Nati Rapaport
* @date   13.04.2020
* @brief  The file contains default NMI (Non-maskable Interrupt) handlers and registration service (relevant to SweRV EH1)
*/


/**
* include files
*/
#include "psp_api.h"

/**
* definitions
*/

/**
* macros
*/

/**
* types
*/

/**
* local prototypes
*/
void pspNmiPinAssertionDefaultHandler(void);
void pspNmiDbusLoadErrorDefaultHandler(void);
void pspNmiDbusStoreErrorDefaultHandler(void);

/**
* external prototypes
*/

/**
* global variables
*/
u32_t g_uiPspNmiErrorAddress = 0;

/* NMI handler pointers */
D_PSP_DATA_SECTION pspNmiHandler_t g_fptrNmiExtPinAssrtHandler    = pspNmiPinAssertionDefaultHandler;
D_PSP_DATA_SECTION pspNmiHandler_t g_fptrNmiDbusLoadErrHandler    = pspNmiDbusLoadErrorDefaultHandler;
D_PSP_DATA_SECTION pspNmiHandler_t g_fptrNmiDbusStoreErrHandler   = pspNmiDbusStoreErrorDefaultHandler;

/**
* functions
*/


/**
* @brief - The function installs an Non-Maskable Interrupt (NMI) service routine
*
* input parameter -  fptrNmiHandler     - function pointer to the NMI service routine
* input parameter -  uiNmiCause         - NMI source
*                    Note that the value of this input parameter could be only one of these:
*                    - D_PSP_NMI_EXT_PIN_ASSERTION
*                    - D_PSP_NMI_D_BUS_STORE_ERROR
*                    - D_PSP_NMI_D_BUS_LOAD_ERROR
*
* @return u32_t                               - previously registered ISR. If NULL then registeration had an error
*/
D_PSP_TEXT_SECTION pspNmiHandler_t pspNmiRegisterHandler(pspNmiHandler_t fptrNmiHandler, u32_t uiNmiCause)
{
	pspNmiHandler_t fptrNmiFunc;

	M_PSP_ASSERT(fptrNmiHandler != NULL && uiNmiCause == D_PSP_NMI_EXT_PIN_ASSERTION
			     && uiNmiCause == D_PSP_NMI_D_BUS_STORE_ERROR && uiNmiCause =d= D_PSP_NMI_D_BUS_LOAD_ERROR);

    switch (uiNmiCause)
	{
	   case D_PSP_NMI_EXT_PIN_ASSERTION:
	       fptrNmiFunc = g_fptrNmiExtPinAssrtHandler;
	       g_fptrNmiExtPinAssrtHandler = fptrNmiHandler;
	       break;
	   case D_PSP_NMI_D_BUS_STORE_ERROR:
	       fptrNmiFunc = g_fptrNmiDbusStoreErrHandler;
	       g_fptrNmiDbusStoreErrHandler = fptrNmiHandler;
	       break;
	   case D_PSP_NMI_D_BUS_LOAD_ERROR:
	       fptrNmiFunc = g_fptrNmiDbusLoadErrHandler;
	       g_fptrNmiDbusLoadErrHandler = fptrNmiHandler;
	       break;
	   default:
	       fptrNmiFunc = NULL;
	       break;
    }

	   return fptrNmiFunc;

}
/**
* @brief - Default handler for pin assertion NMI
*
*/
D_PSP_TEXT_SECTION void pspNmiPinAssertionDefaultHandler(void)
{
}

/**
* @brief - Default handler for D-Bus load error NMI
*
*/
D_PSP_TEXT_SECTION void pspNmiDbusLoadErrorDefaultHandler(void)
{
	/* Get the address of the D-bus load error. Note that this read locks the CSR and until it unlocks no further D-Bus error NMIs are honored */
	g_uiPspNmiErrorAddress = M_PSP_READ_CSR(D_PSP_MDSEAC_NUM);
}

/**
* @brief - Default handler for D-Bus store error NMI
*
*/
D_PSP_TEXT_SECTION void pspNmiDbusStoreErrorDefaultHandler(void)
{
	/* Get the address of the D-bus store error. Note that this read locks the CSR and until it unlocks no further D-Bus error NMIs are honored */
	g_uiPspNmiErrorAddress = M_PSP_READ_CSR(D_PSP_MDSEAC_NUM);
}

/**
* @brief - This function is called upon NMI and selects the appropriate handler
*
*/
D_PSP_TEXT_SECTION void pspNmiHandlerSelector(void)
{
	u32_t uiNmiCode;

	/* Select the correct handler according mcause CSR contents */
	uiNmiCode = M_PSP_READ_CSR(D_PSP_MCAUSE_NUM);

	switch (uiNmiCode)
	{
       case D_PSP_NMI_EXT_PIN_ASSERTION:
       	   pspNmiPinAssertionDefaultHandler();
       	   break;
       case D_PSP_NMI_D_BUS_LOAD_ERROR:
       	   pspNmiDbusLoadErrorDefaultHandler();
       	   break;
       case D_PSP_NMI_D_BUS_STORE_ERROR:
       	   pspNmiDbusStoreErrorDefaultHandler();
       	   break;
       default:
       	   break;
	}
}

