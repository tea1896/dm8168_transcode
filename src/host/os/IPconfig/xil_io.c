/******************************************************************************
*
* (c) Copyright 2009-2012 Xilinx, Inc. All rights reserved.
*
* This file contains confidential and proprietary information of Xilinx, Inc.
* and is protected under U.S. and international copyright and other
* intellectual property laws.
*
* DISCLAIMER
* This disclaimer is not a license and does not grant any rights to the
* materials distributed herewith. Except as otherwise provided in a valid
* license issued to you by Xilinx, and to the maximum extent permitted by
* applicable law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL
* FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS,
* IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NON-INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE;
* and (2) Xilinx shall not be liable (whether in contract or tort, including
* negligence, or under any other theory of liability) for any loss or damage
* of any kind or nature related to, arising under or in connection with these
* materials, including for any direct, or any indirect, special, incidental,
* or consequential loss or damage (including loss of data, profits, goodwill,
* or any type of loss or damage suffered as a result of any action brought by
* a third party) even if such damage or loss was reasonably foreseeable or
* Xilinx had been advised of the possibility of the same.
*
* CRITICAL APPLICATIONS
* Xilinx products are not designed or intended to be fail-safe, or for use in
* any application requiring fail-safe performance, such as life-support or
* safety devices or systems, Class III medical devices, nuclear facilities,
* applications related to the deployment of airbags, or any other applications
* that could lead to death, personal injury, or severe property or
* environmental damage (individually and collectively, "Critical
* Applications"). Customer assumes the sole risk and liability of any use of
* Xilinx products in Critical Applications, subject only to applicable laws
* and regulations governing limitations on product liability.
*
* THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE
* AT ALL TIMES.
*
*******************************************************************************/
/*****************************************************************************/
/**
*
* @file xil_io.c
*
* Contains I/O functions for memory-mapped or non-memory-mapped I/O
* architectures.  These functions encapsulate Cortex A9 architecture-specific
* I/O requirements.
*
* @note
*
* This file contains architecture-dependent code.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who      Date     Changes
* ----- -------- -------- -----------------------------------------------
* 1.00a ecm/sdm  10/24/09 First release
* 3.06a sgd      05/15/12 Pointer volatile used for the all read functions
* 3.07a sgd      08/17/12 Removed barriers (SYNCHRONIZE_IO) calls.
* </pre>
******************************************************************************/


/***************************** Include Files *********************************/
#include "xil_io.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

extern void *s_pGEM1LogicAddress;
extern void *s_pGEM2LogicAddress;
void *s_pGEMLogicAddress;

/************************** Function Prototypes ******************************/

/*****************************************************************************/
/**
*
* Performs an input operation for an 8-bit memory location by reading from the
* specified address and returning the Value read from that address.
*
* @param    Addr contains the address to perform the input operation
*       at.
*
* @return   The Value read from the specified input address.
*
* @note     None.
*
******************************************************************************/
U8 Xil_In8(U32 Addr)
{
    return *(volatile U8 *) Addr;
}

/*****************************************************************************/
/**
*
* Performs an input operation for a 16-bit memory location by reading from the
* specified address and returning the Value read from that address.
*
* @param    Addr contains the address to perform the input operation
*       at.
*
* @return   The Value read from the specified input address.
*
* @note     None.
*
******************************************************************************/
U16 Xil_In16(U32 Addr)
{
    return *(volatile U16 *) Addr;
}

/*****************************************************************************/
/**
*
* Performs an output operation for an 8-bit memory location by writing the
* specified Value to the the specified address.
*
* @param    OutAddress contains the address to perform the output operation
*       at.
* @param    Value contains the Value to be output at the specified address.
*
* @return   None.
*
* @note     None.
*
******************************************************************************/
void Xil_Out8(U32 OutAddress, U8 Value)
{
    *(volatile U8 *) OutAddress = Value;
}

/*****************************************************************************/
/**
*
* Performs an output operation for a 16-bit memory location by writing the
* specified Value to the the specified address.
*
* @param    OutAddress contains the address to perform the output operation
*       at.
* @param    Value contains the Value to be output at the specified address.
*
* @return   None.
*
* @note     None.
*
******************************************************************************/
void Xil_Out16(U32 OutAddress, U16 Value)
{
    *(volatile U16 *) OutAddress = Value;
}

/**
*
* Performs an input operation for a 16-bit memory location by reading from the
* specified address and returning the byte-swapped Value read from that
* address.
*
* @param    Addr contains the address to perform the input operation
*       at.
*
* @return   The byte-swapped Value read from the specified input address.
*
* @note     None.
*
******************************************************************************/
U16 Xil_In16BE(U32 Addr)
{
    U16 temp;
    U16 result;

    /* read the contents of the I/O location and then synchronize the I/O
     * such that the I/O operation completes before proceeding on
     */
    temp = Xil_In16(Addr);

    result = Xil_EndianSwap16(temp);
    /* write the contents of the I/O location and then synchronize the I/O
     * such that the I/O operation completes before proceeding on
     */

    return result;
}

/*****************************************************************************/
/**
*
* Performs an input operation for a 32-bit memory location by reading from the
* specified address and returning the byte-swapped Value read from that
* address.
*
* @param    Addr contains the address to perform the input operation
*       at.
*
* @return   The byte-swapped Value read from the specified input address.
*
* @note     None.
*
******************************************************************************/
U32 Xil_In32BE(U32 Addr)
{
    U32 temp;
    U32 result;

    /* read the contents of the I/O location and then synchronize the I/O
     * such that the I/O operation completes before proceeding on
     */
    temp = Xil_In32(Addr);

    result = Xil_EndianSwap32(temp);
    /* write the contents of the I/O location and then synchronize the I/O
     * such that the I/O operation completes before proceeding on
     */

    return result;
}

/*****************************************************************************/
/**
*
* Performs an output operation for a 16-bit memory location by writing the
* specified Value to the the specified address. The Value is byte-swapped
* before being written.
*
* @param    OutAddress contains the address to perform the output operation
*       at.
* @param    Value contains the Value to be output at the specified address.
*
* @return   None.
*
* @note     None.
*
******************************************************************************/
void Xil_Out16BE(U32 OutAddress, U16 Value)
{
    /* write the contents of the I/O location and then synchronize the I/O
     * such that the I/O operation completes before proceeding on
     */
    U16 temp;

    temp = Xil_EndianSwap16(Value);
    /* write the contents of the I/O location and then synchronize the I/O
     * such that the I/O operation completes before proceeding on
     */
    Xil_Out16(OutAddress, temp);
}

/*****************************************************************************/
/**
*
* Performs an output operation for a 32-bit memory location by writing the
* specified Value to the the specified address. The Value is byte-swapped
* before being written.
*
* @param    OutAddress contains the address to perform the output operation
*       at.
* @param    Value contains the Value to be output at the specified address.
*
* @return   None.
*
* @note     None.
*
******************************************************************************/
void Xil_Out32BE(U32 OutAddress, U32 Value)
{
    U32 temp;

    temp = Xil_EndianSwap32(Value);
    /* write the contents of the I/O location and then synchronize the I/O
     * such that the I/O operation completes before proceeding on
     */
    Xil_Out32(OutAddress, temp);
}

/*****************************************************************************/
/**
*
* Perform a 16-bit endian converion.
*
* @param    Data contains the value to be converted.
*
* @return   converted value.
*
* @note     None.
*
******************************************************************************/
U16 Xil_EndianSwap16(U16 Data)
{
    return (U16) (((Data & 0xFF00) >> 8) | ((Data & 0x00FF) << 8));
}

/*****************************************************************************/
/**
*
* Perform a 32-bit endian converion.
*
* @param    Data contains the value to be converted.
*
* @return   converted value.
*
* @note     None.
*
******************************************************************************/
U32 Xil_EndianSwap32(U32 Data)
{
    U16 LoWord;
    U16 HiWord;

    /* get each of the half words from the 32 bit word */

    LoWord = (U16) (Data & 0x0000FFFF);
    HiWord = (U16) ((Data & 0xFFFF0000) >> 16);

    /* byte swap each of the 16 bit half words */

    LoWord = (((LoWord & 0xFF00) >> 8) | ((LoWord & 0x00FF) << 8));
    HiWord = (((HiWord & 0xFF00) >> 8) | ((HiWord & 0x00FF) << 8));

    /* swap the half words before returning the value */

    return (U32) ((LoWord << 16) | HiWord);
}

U32 XEmacPs_ReadReg(U32 u32Addr)
{
    U32 *h2f_component_reg_rd_addr;
    U32 u32Data = 0;
    h2f_component_reg_rd_addr = ((U32 *)s_pGEM1LogicAddress + 0x0 + u32Addr);
    u32Data = *h2f_component_reg_rd_addr;

    printf("ReadAddr = 0x%x ReadValue = 0x%x\r\n", u32Addr, *h2f_component_reg_rd_addr );

    return u32Data;
}

void XEmacPs_WriteReg(U32 u32Addr, U32 u32Value)
{
    U32 *h2f_component_reg_wr_addr;
    h2f_component_reg_wr_addr = ((U32 *)s_pGEM1LogicAddress + 0x0 + u32Addr);
    *h2f_component_reg_wr_addr = u32Value;
}

/*****************************************************************************/
/**
* Read the current value of the PHY register indicated by the PhyAddress and
* the RegisterNum parameters. The MAC provides the driver with the ability to
* talk to a PHY that adheres to the Media Independent Interface (MII) as
* defined in the IEEE 802.3 standard.
*
* Prior to PHY access with this function, the user should have setup the MDIO
* clock with XEmacPs_SetMdioDivisor().
*
* @param InstancePtr is a pointer to the XEmacPs instance to be worked on.
* @param PhyAddress is the address of the PHY to be read (supports multiple
*        PHYs)
* @param RegisterNum is the register number, 0-31, of the specific PHY register
*        to read
* @param PhyDataPtr is an output parameter, and points to a 16-bit buffer into
*        which the current value of the register will be copied.
*
* @return
*
* - XST_SUCCESS if the PHY was read from successfully
* - XST_EMAC_MII_BUSY if there is another PHY operation in progress
*
* @note
*
* This function is not thread-safe. The user must provide mutually exclusive
* access to this function if there are to be multiple threads that can call it.
*
* There is the possibility that this function will not return if the hardware
* is broken (i.e., it never sets the status bit indicating that the read is
* done). If this is of concern to the user, the user should provide a mechanism
* suitable to their needs for recovery.
*
* For the duration of this function, all host interface reads and writes are
* blocked to the current XEmacPs instance.
*
******************************************************************************/
int XEmacPs_PhyRead(EthType type, U32 PhyAddress,
             U32 RegisterNum, U16 *PhyDataPtr)
{
    U32 Mgtcr;
    volatile U32 Ipisr;

    if(type == ETH0)
    {
        s_pGEMLogicAddress = s_pGEM1LogicAddress;
    }
    else if(type == ETH1)
    {
        s_pGEMLogicAddress = s_pGEM2LogicAddress;
    }
    
    /* Make sure no other PHY operation is currently in progress */
    if (!(XEmacPs_ReadReg(XEMACPS_NWSR_OFFSET) &
          XEMACPS_NWSR_MDIOIDLE_MASK)) {
        return (XST_EMAC_MII_BUSY);
    }

    /* Construct Mgtcr mask for the operation */
    Mgtcr = XEMACPS_PHYMNTNC_OP_MASK | XEMACPS_PHYMNTNC_OP_R_MASK |
        (PhyAddress << XEMACPS_PHYMNTNC_PHYAD_SHIFT_MASK) |
        (RegisterNum << XEMACPS_PHYMNTNC_PHREG_SHIFT_MASK);

    /* Write Mgtcr and wait for completion */
    XEmacPs_WriteReg(XEMACPS_PHYMNTNC_OFFSET, Mgtcr);

    do {
        Ipisr = XEmacPs_ReadReg(XEMACPS_NWSR_OFFSET);
    } while ((Ipisr & XEMACPS_NWSR_MDIOIDLE_MASK) == 0);

    /* Read data */
    *PhyDataPtr = XEmacPs_ReadReg(XEMACPS_PHYMNTNC_OFFSET);

    return (XST_SUCCESS);
}



/*****************************************************************************/
/**
* Write data to the specified PHY register. The Ethernet driver does not
* require the device to be stopped before writing to the PHY.  Although it is
* probably a good idea to stop the device, it is the responsibility of the
* application to deem this necessary. The MAC provides the driver with the
* ability to talk to a PHY that adheres to the Media Independent Interface
* (MII) as defined in the IEEE 802.3 standard.
*
* Prior to PHY access with this function, the user should have setup the MDIO
* clock with XEmacPs_SetMdioDivisor().
*
* @param InstancePtr is a pointer to the XEmacPs instance to be worked on.
* @param PhyAddress is the address of the PHY to be written (supports multiple
*        PHYs)
* @param RegisterNum is the register number, 0-31, of the specific PHY register
*        to write
* @param PhyData is the 16-bit value that will be written to the register
*
* @return
*
* - XST_SUCCESS if the PHY was written to successfully. Since there is no error
*   status from the MAC on a write, the user should read the PHY to verify the
*   write was successful.
* - XST_EMAC_MII_BUSY if there is another PHY operation in progress
*
* @note
*
* This function is not thread-safe. The user must provide mutually exclusive
* access to this function if there are to be multiple threads that can call it.
*
* There is the possibility that this function will not return if the hardware
* is broken (i.e., it never sets the status bit indicating that the write is
* done). If this is of concern to the user, the user should provide a mechanism
* suitable to their needs for recovery.
*
* For the duration of this function, all host interface reads and writes are
* blocked to the current XEmacPs instance.
*
******************************************************************************/
int XEmacPs_PhyWrite(EthType type, U32 PhyAddress,
              U32 RegisterNum, U16 PhyData)
{
    U32 Mgtcr;
    volatile U32 Ipisr;

    if(type == ETH0)
    {
        s_pGEMLogicAddress = s_pGEM1LogicAddress;
    }
    else if(type == ETH1)
    {
        s_pGEMLogicAddress = s_pGEM2LogicAddress;
    }

    /* Make sure no other PHY operation is currently in progress */
    if (!(XEmacPs_ReadReg(XEMACPS_NWSR_OFFSET) &
          XEMACPS_NWSR_MDIOIDLE_MASK)) {
        return (XST_EMAC_MII_BUSY);
    }

    /* Construct Mgtcr mask for the operation */
    Mgtcr = XEMACPS_PHYMNTNC_OP_MASK | XEMACPS_PHYMNTNC_OP_W_MASK |
        (PhyAddress << XEMACPS_PHYMNTNC_PHYAD_SHIFT_MASK) |
        (RegisterNum << XEMACPS_PHYMNTNC_PHREG_SHIFT_MASK) | PhyData;

    /* Write Mgtcr and wait for completion */
    XEmacPs_WriteReg(XEMACPS_PHYMNTNC_OFFSET, Mgtcr);

    do {
        Ipisr = XEmacPs_ReadReg(XEMACPS_NWSR_OFFSET);
    } while ((Ipisr & XEMACPS_NWSR_MDIOIDLE_MASK) == 0);

    return (XST_SUCCESS);
}


