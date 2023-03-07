/***************************************************************************//**
 * @file
 * @brief File System - Cyclic Redundancy Check (Crc) Calculation
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_FS_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_path.h>
#include  <fs_storage_cfg.h>

#include  <fs/source/shared/crc/edc_crc.h>
#include  <common/include/lib_utils.h>
#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_MODULE_CUR  RTOS_CFG_MODULE_FS

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_CRC16_1021_EN == DEF_ENABLED)
const CPU_INT16U CRC_TblCRC16_1021[256] = {
  0x0000u, 0x1021u, 0x2042u, 0x3063u, 0x4084u, 0x50A5u, 0x60C6u, 0x70E7u, 0x8108u, 0x9129u, 0xA14Au, 0xB16Bu, 0xC18Cu, 0xD1ADu, 0xE1CEu, 0xF1EFu,
  0x1231u, 0x0210u, 0x3273u, 0x2252u, 0x52B5u, 0x4294u, 0x72F7u, 0x62D6u, 0x9339u, 0x8318u, 0xB37Bu, 0xA35Au, 0xD3BDu, 0xC39Cu, 0xF3FFu, 0xE3DEu,
  0x2462u, 0x3443u, 0x0420u, 0x1401u, 0x64E6u, 0x74C7u, 0x44A4u, 0x5485u, 0xA56Au, 0xB54Bu, 0x8528u, 0x9509u, 0xE5EEu, 0xF5CFu, 0xC5ACu, 0xD58Du,
  0x3653u, 0x2672u, 0x1611u, 0x0630u, 0x76D7u, 0x66F6u, 0x5695u, 0x46B4u, 0xB75Bu, 0xA77Au, 0x9719u, 0x8738u, 0xF7DFu, 0xE7FEu, 0xD79Du, 0xC7BCu,
  0x48C4u, 0x58E5u, 0x6886u, 0x78A7u, 0x0840u, 0x1861u, 0x2802u, 0x3823u, 0xC9CCu, 0xD9EDu, 0xE98Eu, 0xF9AFu, 0x8948u, 0x9969u, 0xA90Au, 0xB92Bu,
  0x5AF5u, 0x4AD4u, 0x7AB7u, 0x6A96u, 0x1A71u, 0x0A50u, 0x3A33u, 0x2A12u, 0xDBFDu, 0xCBDCu, 0xFBBFu, 0xEB9Eu, 0x9B79u, 0x8B58u, 0xBB3Bu, 0xAB1Au,
  0x6CA6u, 0x7C87u, 0x4CE4u, 0x5CC5u, 0x2C22u, 0x3C03u, 0x0C60u, 0x1C41u, 0xEDAEu, 0xFD8Fu, 0xCDECu, 0xDDCDu, 0xAD2Au, 0xBD0Bu, 0x8D68u, 0x9D49u,
  0x7E97u, 0x6EB6u, 0x5ED5u, 0x4EF4u, 0x3E13u, 0x2E32u, 0x1E51u, 0x0E70u, 0xFF9Fu, 0xEFBEu, 0xDFDDu, 0xCFFCu, 0xBF1Bu, 0xAF3Au, 0x9F59u, 0x8F78u,
  0x9188u, 0x81A9u, 0xB1CAu, 0xA1EBu, 0xD10Cu, 0xC12Du, 0xF14Eu, 0xE16Fu, 0x1080u, 0x00A1u, 0x30C2u, 0x20E3u, 0x5004u, 0x4025u, 0x7046u, 0x6067u,
  0x83B9u, 0x9398u, 0xA3FBu, 0xB3DAu, 0xC33Du, 0xD31Cu, 0xE37Fu, 0xF35Eu, 0x02B1u, 0x1290u, 0x22F3u, 0x32D2u, 0x4235u, 0x5214u, 0x6277u, 0x7256u,
  0xB5EAu, 0xA5CBu, 0x95A8u, 0x8589u, 0xF56Eu, 0xE54Fu, 0xD52Cu, 0xC50Du, 0x34E2u, 0x24C3u, 0x14A0u, 0x0481u, 0x7466u, 0x6447u, 0x5424u, 0x4405u,
  0xA7DBu, 0xB7FAu, 0x8799u, 0x97B8u, 0xE75Fu, 0xF77Eu, 0xC71Du, 0xD73Cu, 0x26D3u, 0x36F2u, 0x0691u, 0x16B0u, 0x6657u, 0x7676u, 0x4615u, 0x5634u,
  0xD94Cu, 0xC96Du, 0xF90Eu, 0xE92Fu, 0x99C8u, 0x89E9u, 0xB98Au, 0xA9ABu, 0x5844u, 0x4865u, 0x7806u, 0x6827u, 0x18C0u, 0x08E1u, 0x3882u, 0x28A3u,
  0xCB7Du, 0xDB5Cu, 0xEB3Fu, 0xFB1Eu, 0x8BF9u, 0x9BD8u, 0xABBBu, 0xBB9Au, 0x4A75u, 0x5A54u, 0x6A37u, 0x7A16u, 0x0AF1u, 0x1AD0u, 0x2AB3u, 0x3A92u,
  0xFD2Eu, 0xED0Fu, 0xDD6Cu, 0xCD4Du, 0xBDAAu, 0xAD8Bu, 0x9DE8u, 0x8DC9u, 0x7C26u, 0x6C07u, 0x5C64u, 0x4C45u, 0x3CA2u, 0x2C83u, 0x1CE0u, 0x0CC1u,
  0xEF1Fu, 0xFF3Eu, 0xCF5Du, 0xDF7Cu, 0xAF9Bu, 0xBFBAu, 0x8FD9u, 0x9FF8u, 0x6E17u, 0x7E36u, 0x4E55u, 0x5E74u, 0x2E93u, 0x3EB2u, 0x0ED1u, 0x1EF0u
};

const CRC_MODEL_16 CRC_ModelCRC16_1021 = {
  0x1021u,
  0xFFFFu,
  DEF_NO,
  0x0000u,
  &CRC_TblCRC16_1021[0]
};
#endif

#if (FS_STORAGE_CFG_CRC16_1021_REF_EN == DEF_ENABLED)
const CPU_INT16U CRC_TblCRC16_1021_ref[256] = {
  0x0000u, 0x1189u, 0x2312u, 0x329Bu, 0x4624u, 0x57ADu, 0x6536u, 0x74BFu, 0x8C48u, 0x9DC1u, 0xAF5Au, 0xBED3u, 0xCA6Cu, 0xDBE5u, 0xE97Eu, 0xF8F7u,
  0x1081u, 0x0108u, 0x3393u, 0x221Au, 0x56A5u, 0x472Cu, 0x75B7u, 0x643Eu, 0x9CC9u, 0x8D40u, 0xBFDBu, 0xAE52u, 0xDAEDu, 0xCB64u, 0xF9FFu, 0xE876u,
  0x2102u, 0x308Bu, 0x0210u, 0x1399u, 0x6726u, 0x76AFu, 0x4434u, 0x55BDu, 0xAD4Au, 0xBCC3u, 0x8E58u, 0x9FD1u, 0xEB6Eu, 0xFAE7u, 0xC87Cu, 0xD9F5u,
  0x3183u, 0x200Au, 0x1291u, 0x0318u, 0x77A7u, 0x662Eu, 0x54B5u, 0x453Cu, 0xBDCBu, 0xAC42u, 0x9ED9u, 0x8F50u, 0xFBEFu, 0xEA66u, 0xD8FDu, 0xC974u,
  0x4204u, 0x538Du, 0x6116u, 0x709Fu, 0x0420u, 0x15A9u, 0x2732u, 0x36BBu, 0xCE4Cu, 0xDFC5u, 0xED5Eu, 0xFCD7u, 0x8868u, 0x99E1u, 0xAB7Au, 0xBAF3u,
  0x5285u, 0x430Cu, 0x7197u, 0x601Eu, 0x14A1u, 0x0528u, 0x37B3u, 0x263Au, 0xDECDu, 0xCF44u, 0xFDDFu, 0xEC56u, 0x98E9u, 0x8960u, 0xBBFBu, 0xAA72u,
  0x6306u, 0x728Fu, 0x4014u, 0x519Du, 0x2522u, 0x34ABu, 0x0630u, 0x17B9u, 0xEF4Eu, 0xFEC7u, 0xCC5Cu, 0xDDD5u, 0xA96Au, 0xB8E3u, 0x8A78u, 0x9BF1u,
  0x7387u, 0x620Eu, 0x5095u, 0x411Cu, 0x35A3u, 0x242Au, 0x16B1u, 0x0738u, 0xFFCFu, 0xEE46u, 0xDCDDu, 0xCD54u, 0xB9EBu, 0xA862u, 0x9AF9u, 0x8B70u,
  0x8408u, 0x9581u, 0xA71Au, 0xB693u, 0xC22Cu, 0xD3A5u, 0xE13Eu, 0xF0B7u, 0x0840u, 0x19C9u, 0x2B52u, 0x3ADBu, 0x4E64u, 0x5FEDu, 0x6D76u, 0x7CFFu,
  0x9489u, 0x8500u, 0xB79Bu, 0xA612u, 0xD2ADu, 0xC324u, 0xF1BFu, 0xE036u, 0x18C1u, 0x0948u, 0x3BD3u, 0x2A5Au, 0x5EE5u, 0x4F6Cu, 0x7DF7u, 0x6C7Eu,
  0xA50Au, 0xB483u, 0x8618u, 0x9791u, 0xE32Eu, 0xF2A7u, 0xC03Cu, 0xD1B5u, 0x2942u, 0x38CBu, 0x0A50u, 0x1BD9u, 0x6F66u, 0x7EEFu, 0x4C74u, 0x5DFDu,
  0xB58Bu, 0xA402u, 0x9699u, 0x8710u, 0xF3AFu, 0xE226u, 0xD0BDu, 0xC134u, 0x39C3u, 0x284Au, 0x1AD1u, 0x0B58u, 0x7FE7u, 0x6E6Eu, 0x5CF5u, 0x4D7Cu,
  0xC60Cu, 0xD785u, 0xE51Eu, 0xF497u, 0x8028u, 0x91A1u, 0xA33Au, 0xB2B3u, 0x4A44u, 0x5BCDu, 0x6956u, 0x78DFu, 0x0C60u, 0x1DE9u, 0x2F72u, 0x3EFBu,
  0xD68Du, 0xC704u, 0xF59Fu, 0xE416u, 0x90A9u, 0x8120u, 0xB3BBu, 0xA232u, 0x5AC5u, 0x4B4Cu, 0x79D7u, 0x685Eu, 0x1CE1u, 0x0D68u, 0x3FF3u, 0x2E7Au,
  0xE70Eu, 0xF687u, 0xC41Cu, 0xD595u, 0xA12Au, 0xB0A3u, 0x8238u, 0x93B1u, 0x6B46u, 0x7ACFu, 0x4854u, 0x59DDu, 0x2D62u, 0x3CEBu, 0x0E70u, 0x1FF9u,
  0xF78Fu, 0xE606u, 0xD49Du, 0xC514u, 0xB1ABu, 0xA022u, 0x92B9u, 0x8330u, 0x7BC7u, 0x6A4Eu, 0x58D5u, 0x495Cu, 0x3DE3u, 0x2C6Au, 0x1EF1u, 0x0F78u
};

const CRC_MODEL_16 CRC_ModelCRC16_1021_ref = {
  0x1021u,
  0xFFFFu,
  DEF_YES,
  0x0000u,
  &CRC_TblCRC16_1021_ref[0]
};
#endif

#if (FS_STORAGE_CFG_CRC16_8005_EN == DEF_ENABLED)
const CPU_INT16U CRC_TblCRC16_8005[256] = {
  0x0000u, 0x8005u, 0x800Fu, 0x000Au, 0x801Bu, 0x001Eu, 0x0014u, 0x8011u, 0x8033u, 0x0036u, 0x003Cu, 0x8039u, 0x0028u, 0x802Du, 0x8027u, 0x0022u,
  0x8063u, 0x0066u, 0x006Cu, 0x8069u, 0x0078u, 0x807Du, 0x8077u, 0x0072u, 0x0050u, 0x8055u, 0x805Fu, 0x005Au, 0x804Bu, 0x004Eu, 0x0044u, 0x8041u,
  0x80C3u, 0x00C6u, 0x00CCu, 0x80C9u, 0x00D8u, 0x80DDu, 0x80D7u, 0x00D2u, 0x00F0u, 0x80F5u, 0x80FFu, 0x00FAu, 0x80EBu, 0x00EEu, 0x00E4u, 0x80E1u,
  0x00A0u, 0x80A5u, 0x80AFu, 0x00AAu, 0x80BBu, 0x00BEu, 0x00B4u, 0x80B1u, 0x8093u, 0x0096u, 0x009Cu, 0x8099u, 0x0088u, 0x808Du, 0x8087u, 0x0082u,
  0x8183u, 0x0186u, 0x018Cu, 0x8189u, 0x0198u, 0x819Du, 0x8197u, 0x0192u, 0x01B0u, 0x81B5u, 0x81BFu, 0x01BAu, 0x81ABu, 0x01AEu, 0x01A4u, 0x81A1u,
  0x01E0u, 0x81E5u, 0x81EFu, 0x01EAu, 0x81FBu, 0x01FEu, 0x01F4u, 0x81F1u, 0x81D3u, 0x01D6u, 0x01DCu, 0x81D9u, 0x01C8u, 0x81CDu, 0x81C7u, 0x01C2u,
  0x0140u, 0x8145u, 0x814Fu, 0x014Au, 0x815Bu, 0x015Eu, 0x0154u, 0x8151u, 0x8173u, 0x0176u, 0x017Cu, 0x8179u, 0x0168u, 0x816Du, 0x8167u, 0x0162u,
  0x8123u, 0x0126u, 0x012Cu, 0x8129u, 0x0138u, 0x813Du, 0x8137u, 0x0132u, 0x0110u, 0x8115u, 0x811Fu, 0x011Au, 0x810Bu, 0x010Eu, 0x0104u, 0x8101u,
  0x8303u, 0x0306u, 0x030Cu, 0x8309u, 0x0318u, 0x831Du, 0x8317u, 0x0312u, 0x0330u, 0x8335u, 0x833Fu, 0x033Au, 0x832Bu, 0x032Eu, 0x0324u, 0x8321u,
  0x0360u, 0x8365u, 0x836Fu, 0x036Au, 0x837Bu, 0x037Eu, 0x0374u, 0x8371u, 0x8353u, 0x0356u, 0x035Cu, 0x8359u, 0x0348u, 0x834Du, 0x8347u, 0x0342u,
  0x03C0u, 0x83C5u, 0x83CFu, 0x03CAu, 0x83DBu, 0x03DEu, 0x03D4u, 0x83D1u, 0x83F3u, 0x03F6u, 0x03FCu, 0x83F9u, 0x03E8u, 0x83EDu, 0x83E7u, 0x03E2u,
  0x83A3u, 0x03A6u, 0x03ACu, 0x83A9u, 0x03B8u, 0x83BDu, 0x83B7u, 0x03B2u, 0x0390u, 0x8395u, 0x839Fu, 0x039Au, 0x838Bu, 0x038Eu, 0x0384u, 0x8381u,
  0x0280u, 0x8285u, 0x828Fu, 0x028Au, 0x829Bu, 0x029Eu, 0x0294u, 0x8291u, 0x82B3u, 0x02B6u, 0x02BCu, 0x82B9u, 0x02A8u, 0x82ADu, 0x82A7u, 0x02A2u,
  0x82E3u, 0x02E6u, 0x02ECu, 0x82E9u, 0x02F8u, 0x82FDu, 0x82F7u, 0x02F2u, 0x02D0u, 0x82D5u, 0x82DFu, 0x02DAu, 0x82CBu, 0x02CEu, 0x02C4u, 0x82C1u,
  0x8243u, 0x0246u, 0x024Cu, 0x8249u, 0x0258u, 0x825Du, 0x8257u, 0x0252u, 0x0270u, 0x8275u, 0x827Fu, 0x027Au, 0x826Bu, 0x026Eu, 0x0264u, 0x8261u,
  0x0220u, 0x8225u, 0x822Fu, 0x022Au, 0x823Bu, 0x023Eu, 0x0234u, 0x8231u, 0x8213u, 0x0216u, 0x021Cu, 0x8219u, 0x0208u, 0x820Du, 0x8207u, 0x0202u
};

const CRC_MODEL_16 CRC_ModelCRC16_8005 = {
  0x8005u,
  0x0000u,
  DEF_NO,
  0x0000u,
  &CRC_TblCRC16_8005[0]
};
#endif

#if (FS_STORAGE_CFG_CRC16_8005_REF_EN == DEF_ENABLED)
const CPU_INT16U CRC_TblCRC16_8005_ref[256] = {
  0x0000u, 0xC0C1u, 0xC181u, 0x0140u, 0xC301u, 0x03C0u, 0x0280u, 0xC241u, 0xC601u, 0x06C0u, 0x0780u, 0xC741u, 0x0500u, 0xC5C1u, 0xC481u, 0x0440u,
  0xCC01u, 0x0CC0u, 0x0D80u, 0xCD41u, 0x0F00u, 0xCFC1u, 0xCE81u, 0x0E40u, 0x0A00u, 0xCAC1u, 0xCB81u, 0x0B40u, 0xC901u, 0x09C0u, 0x0880u, 0xC841u,
  0xD801u, 0x18C0u, 0x1980u, 0xD941u, 0x1B00u, 0xDBC1u, 0xDA81u, 0x1A40u, 0x1E00u, 0xDEC1u, 0xDF81u, 0x1F40u, 0xDD01u, 0x1DC0u, 0x1C80u, 0xDC41u,
  0x1400u, 0xD4C1u, 0xD581u, 0x1540u, 0xD701u, 0x17C0u, 0x1680u, 0xD641u, 0xD201u, 0x12C0u, 0x1380u, 0xD341u, 0x1100u, 0xD1C1u, 0xD081u, 0x1040u,
  0xF001u, 0x30C0u, 0x3180u, 0xF141u, 0x3300u, 0xF3C1u, 0xF281u, 0x3240u, 0x3600u, 0xF6C1u, 0xF781u, 0x3740u, 0xF501u, 0x35C0u, 0x3480u, 0xF441u,
  0x3C00u, 0xFCC1u, 0xFD81u, 0x3D40u, 0xFF01u, 0x3FC0u, 0x3E80u, 0xFE41u, 0xFA01u, 0x3AC0u, 0x3B80u, 0xFB41u, 0x3900u, 0xF9C1u, 0xF881u, 0x3840u,
  0x2800u, 0xE8C1u, 0xE981u, 0x2940u, 0xEB01u, 0x2BC0u, 0x2A80u, 0xEA41u, 0xEE01u, 0x2EC0u, 0x2F80u, 0xEF41u, 0x2D00u, 0xEDC1u, 0xEC81u, 0x2C40u,
  0xE401u, 0x24C0u, 0x2580u, 0xE541u, 0x2700u, 0xE7C1u, 0xE681u, 0x2640u, 0x2200u, 0xE2C1u, 0xE381u, 0x2340u, 0xE101u, 0x21C0u, 0x2080u, 0xE041u,
  0xA001u, 0x60C0u, 0x6180u, 0xA141u, 0x6300u, 0xA3C1u, 0xA281u, 0x6240u, 0x6600u, 0xA6C1u, 0xA781u, 0x6740u, 0xA501u, 0x65C0u, 0x6480u, 0xA441u,
  0x6C00u, 0xACC1u, 0xAD81u, 0x6D40u, 0xAF01u, 0x6FC0u, 0x6E80u, 0xAE41u, 0xAA01u, 0x6AC0u, 0x6B80u, 0xAB41u, 0x6900u, 0xA9C1u, 0xA881u, 0x6840u,
  0x7800u, 0xB8C1u, 0xB981u, 0x7940u, 0xBB01u, 0x7BC0u, 0x7A80u, 0xBA41u, 0xBE01u, 0x7EC0u, 0x7F80u, 0xBF41u, 0x7D00u, 0xBDC1u, 0xBC81u, 0x7C40u,
  0xB401u, 0x74C0u, 0x7580u, 0xB541u, 0x7700u, 0xB7C1u, 0xB681u, 0x7640u, 0x7200u, 0xB2C1u, 0xB381u, 0x7340u, 0xB101u, 0x71C0u, 0x7080u, 0xB041u,
  0x5000u, 0x90C1u, 0x9181u, 0x5140u, 0x9301u, 0x53C0u, 0x5280u, 0x9241u, 0x9601u, 0x56C0u, 0x5780u, 0x9741u, 0x5500u, 0x95C1u, 0x9481u, 0x5440u,
  0x9C01u, 0x5CC0u, 0x5D80u, 0x9D41u, 0x5F00u, 0x9FC1u, 0x9E81u, 0x5E40u, 0x5A00u, 0x9AC1u, 0x9B81u, 0x5B40u, 0x9901u, 0x59C0u, 0x5880u, 0x9841u,
  0x8801u, 0x48C0u, 0x4980u, 0x8941u, 0x4B00u, 0x8BC1u, 0x8A81u, 0x4A40u, 0x4E00u, 0x8EC1u, 0x8F81u, 0x4F40u, 0x8D01u, 0x4DC0u, 0x4C80u, 0x8C41u,
  0x4400u, 0x84C1u, 0x8581u, 0x4540u, 0x8701u, 0x47C0u, 0x4680u, 0x8641u, 0x8201u, 0x42C0u, 0x4380u, 0x8341u, 0x4100u, 0x81C1u, 0x8081u, 0x4040u
};

const CRC_MODEL_16 CRC_ModelCRC16_8005_ref = {
  0x8005u,
  0x0000u,
  DEF_YES,
  0x0000u,
  &CRC_TblCRC16_8005_ref[0]
};
#endif

#if (FS_STORAGE_CFG_CRC16_8048_EN == DEF_ENABLED)
const CPU_INT16U CRC_TblCRC16_8048[256] = {
  0x0000u, 0x8048u, 0x80D8u, 0x0090u, 0x81F8u, 0x01B0u, 0x0120u, 0x8168u, 0x83B8u, 0x03F0u, 0x0360u, 0x8328u, 0x0240u, 0x8208u, 0x8298u, 0x02D0u,
  0x8738u, 0x0770u, 0x07E0u, 0x87A8u, 0x06C0u, 0x8688u, 0x8618u, 0x0650u, 0x0480u, 0x84C8u, 0x8458u, 0x0410u, 0x8578u, 0x0530u, 0x05A0u, 0x85E8u,
  0x8E38u, 0x0E70u, 0x0EE0u, 0x8EA8u, 0x0FC0u, 0x8F88u, 0x8F18u, 0x0F50u, 0x0D80u, 0x8DC8u, 0x8D58u, 0x0D10u, 0x8C78u, 0x0C30u, 0x0CA0u, 0x8CE8u,
  0x0900u, 0x8948u, 0x89D8u, 0x0990u, 0x88F8u, 0x08B0u, 0x0820u, 0x8868u, 0x8AB8u, 0x0AF0u, 0x0A60u, 0x8A28u, 0x0B40u, 0x8B08u, 0x8B98u, 0x0BD0u,
  0x9C38u, 0x1C70u, 0x1CE0u, 0x9CA8u, 0x1DC0u, 0x9D88u, 0x9D18u, 0x1D50u, 0x1F80u, 0x9FC8u, 0x9F58u, 0x1F10u, 0x9E78u, 0x1E30u, 0x1EA0u, 0x9EE8u,
  0x1B00u, 0x9B48u, 0x9BD8u, 0x1B90u, 0x9AF8u, 0x1AB0u, 0x1A20u, 0x9A68u, 0x98B8u, 0x18F0u, 0x1860u, 0x9828u, 0x1940u, 0x9908u, 0x9998u, 0x19D0u,
  0x1200u, 0x9248u, 0x92D8u, 0x1290u, 0x93F8u, 0x13B0u, 0x1320u, 0x9368u, 0x91B8u, 0x11F0u, 0x1160u, 0x9128u, 0x1040u, 0x9008u, 0x9098u, 0x10D0u,
  0x9538u, 0x1570u, 0x15E0u, 0x95A8u, 0x14C0u, 0x9488u, 0x9418u, 0x1450u, 0x1680u, 0x96C8u, 0x9658u, 0x1610u, 0x9778u, 0x1730u, 0x17A0u, 0x97E8u,
  0xB838u, 0x3870u, 0x38E0u, 0xB8A8u, 0x39C0u, 0xB988u, 0xB918u, 0x3950u, 0x3B80u, 0xBBC8u, 0xBB58u, 0x3B10u, 0xBA78u, 0x3A30u, 0x3AA0u, 0xBAE8u,
  0x3F00u, 0xBF48u, 0xBFD8u, 0x3F90u, 0xBEF8u, 0x3EB0u, 0x3E20u, 0xBE68u, 0xBCB8u, 0x3CF0u, 0x3C60u, 0xBC28u, 0x3D40u, 0xBD08u, 0xBD98u, 0x3DD0u,
  0x3600u, 0xB648u, 0xB6D8u, 0x3690u, 0xB7F8u, 0x37B0u, 0x3720u, 0xB768u, 0xB5B8u, 0x35F0u, 0x3560u, 0xB528u, 0x3440u, 0xB408u, 0xB498u, 0x34D0u,
  0xB138u, 0x3170u, 0x31E0u, 0xB1A8u, 0x30C0u, 0xB088u, 0xB018u, 0x3050u, 0x3280u, 0xB2C8u, 0xB258u, 0x3210u, 0xB378u, 0x3330u, 0x33A0u, 0xB3E8u,
  0x2400u, 0xA448u, 0xA4D8u, 0x2490u, 0xA5F8u, 0x25B0u, 0x2520u, 0xA568u, 0xA7B8u, 0x27F0u, 0x2760u, 0xA728u, 0x2640u, 0xA608u, 0xA698u, 0x26D0u,
  0xA338u, 0x2370u, 0x23E0u, 0xA3A8u, 0x22C0u, 0xA288u, 0xA218u, 0x2250u, 0x2080u, 0xA0C8u, 0xA058u, 0x2010u, 0xA178u, 0x2130u, 0x21A0u, 0xA1E8u,
  0xAA38u, 0x2A70u, 0x2AE0u, 0xAAA8u, 0x2BC0u, 0xAB88u, 0xAB18u, 0x2B50u, 0x2980u, 0xA9C8u, 0xA958u, 0x2910u, 0xA878u, 0x2830u, 0x28A0u, 0xA8E8u,
  0x2D00u, 0xAD48u, 0xADD8u, 0x2D90u, 0xACF8u, 0x2CB0u, 0x2C20u, 0xAC68u, 0xAEB8u, 0x2EF0u, 0x2E60u, 0xAE28u, 0x2F40u, 0xAF08u, 0xAF98u, 0x2FD0u
};

const CRC_MODEL_16 CRC_ModelCRC16_8048 = {
  0x8048u,
  0x0000u,
  DEF_NO,
  0x0000u,
  &CRC_TblCRC16_8048[0]
};
#endif

#if (FS_STORAGE_CFG_CRC16_8048_REF_EN == DEF_ENABLED)
const CPU_INT16U CRC_TblCRC16_8048_ref[256] = {
  0x0000u, 0x1C1Du, 0x1C39u, 0x0024u, 0x1C71u, 0x006Cu, 0x0048u, 0x1C55u, 0x1CE1u, 0x00FCu, 0x00D8u, 0x1CC5u, 0x0090u, 0x1C8Du, 0x1CA9u, 0x00B4u,
  0x1DC1u, 0x01DCu, 0x01F8u, 0x1DE5u, 0x01B0u, 0x1DADu, 0x1D89u, 0x0194u, 0x0120u, 0x1D3Du, 0x1D19u, 0x0104u, 0x1D51u, 0x014Cu, 0x0168u, 0x1D75u,
  0x1F81u, 0x039Cu, 0x03B8u, 0x1FA5u, 0x03F0u, 0x1FEDu, 0x1FC9u, 0x03D4u, 0x0360u, 0x1F7Du, 0x1F59u, 0x0344u, 0x1F11u, 0x030Cu, 0x0328u, 0x1F35u,
  0x0240u, 0x1E5Du, 0x1E79u, 0x0264u, 0x1E31u, 0x022Cu, 0x0208u, 0x1E15u, 0x1EA1u, 0x02BCu, 0x0298u, 0x1E85u, 0x02D0u, 0x1ECDu, 0x1EE9u, 0x02F4u,
  0x1B01u, 0x071Cu, 0x0738u, 0x1B25u, 0x0770u, 0x1B6Du, 0x1B49u, 0x0754u, 0x07E0u, 0x1BFDu, 0x1BD9u, 0x07C4u, 0x1B91u, 0x078Cu, 0x07A8u, 0x1BB5u,
  0x06C0u, 0x1ADDu, 0x1AF9u, 0x06E4u, 0x1AB1u, 0x06ACu, 0x0688u, 0x1A95u, 0x1A21u, 0x063Cu, 0x0618u, 0x1A05u, 0x0650u, 0x1A4Du, 0x1A69u, 0x0674u,
  0x0480u, 0x189Du, 0x18B9u, 0x04A4u, 0x18F1u, 0x04ECu, 0x04C8u, 0x18D5u, 0x1861u, 0x047Cu, 0x0458u, 0x1845u, 0x0410u, 0x180Du, 0x1829u, 0x0434u,
  0x1941u, 0x055Cu, 0x0578u, 0x1965u, 0x0530u, 0x192Du, 0x1909u, 0x0514u, 0x05A0u, 0x19BDu, 0x1999u, 0x0584u, 0x19D1u, 0x05CCu, 0x05E8u, 0x19F5u,
  0x1201u, 0x0E1Cu, 0x0E38u, 0x1225u, 0x0E70u, 0x126Du, 0x1249u, 0x0E54u, 0x0EE0u, 0x12FDu, 0x12D9u, 0x0EC4u, 0x1291u, 0x0E8Cu, 0x0EA8u, 0x12B5u,
  0x0FC0u, 0x13DDu, 0x13F9u, 0x0FE4u, 0x13B1u, 0x0FACu, 0x0F88u, 0x1395u, 0x1321u, 0x0F3Cu, 0x0F18u, 0x1305u, 0x0F50u, 0x134Du, 0x1369u, 0x0F74u,
  0x0D80u, 0x119Du, 0x11B9u, 0x0DA4u, 0x11F1u, 0x0DECu, 0x0DC8u, 0x11D5u, 0x1161u, 0x0D7Cu, 0x0D58u, 0x1145u, 0x0D10u, 0x110Du, 0x1129u, 0x0D34u,
  0x1041u, 0x0C5Cu, 0x0C78u, 0x1065u, 0x0C30u, 0x102Du, 0x1009u, 0x0C14u, 0x0CA0u, 0x10BDu, 0x1099u, 0x0C84u, 0x10D1u, 0x0CCCu, 0x0CE8u, 0x10F5u,
  0x0900u, 0x151Du, 0x1539u, 0x0924u, 0x1571u, 0x096Cu, 0x0948u, 0x1555u, 0x15E1u, 0x09FCu, 0x09D8u, 0x15C5u, 0x0990u, 0x158Du, 0x15A9u, 0x09B4u,
  0x14C1u, 0x08DCu, 0x08F8u, 0x14E5u, 0x08B0u, 0x14ADu, 0x1489u, 0x0894u, 0x0820u, 0x143Du, 0x1419u, 0x0804u, 0x1451u, 0x084Cu, 0x0868u, 0x1475u,
  0x1681u, 0x0A9Cu, 0x0AB8u, 0x16A5u, 0x0AF0u, 0x16EDu, 0x16C9u, 0x0AD4u, 0x0A60u, 0x167Du, 0x1659u, 0x0A44u, 0x1611u, 0x0A0Cu, 0x0A28u, 0x1635u,
  0x0B40u, 0x175Du, 0x1779u, 0x0B64u, 0x1731u, 0x0B2Cu, 0x0B08u, 0x1715u, 0x17A1u, 0x0BBCu, 0x0B98u, 0x1785u, 0x0BD0u, 0x17CDu, 0x17E9u, 0x0BF4u
};

const CRC_MODEL_16 CRC_ModelCRC16_8048_ref = {
  0x8048u,
  0x0000u,
  DEF_YES,
  0x0000u,
  &CRC_TblCRC16_8048_ref[0]
};
#endif

#if (FS_STORAGE_CFG_CRC32_EN == DEF_ENABLED)
const CPU_INT32U CRC_TblCRC32[256] = {
  0x00000000u, 0x04C11DB7u, 0x09823B6Eu, 0x0D4326D9u, 0x130476DCu, 0x17C56B6Bu, 0x1A864DB2u, 0x1E475005u, 0x2608EDB8u, 0x22C9F00Fu, 0x2F8AD6D6u, 0x2B4BCB61u, 0x350C9B64u, 0x31CD86D3u, 0x3C8EA00Au, 0x384FBDBDu,
  0x4C11DB70u, 0x48D0C6C7u, 0x4593E01Eu, 0x4152FDA9u, 0x5F15ADACu, 0x5BD4B01Bu, 0x569796C2u, 0x52568B75u, 0x6A1936C8u, 0x6ED82B7Fu, 0x639B0DA6u, 0x675A1011u, 0x791D4014u, 0x7DDC5DA3u, 0x709F7B7Au, 0x745E66CDu,
  0x9823B6E0u, 0x9CE2AB57u, 0x91A18D8Eu, 0x95609039u, 0x8B27C03Cu, 0x8FE6DD8Bu, 0x82A5FB52u, 0x8664E6E5u, 0xBE2B5B58u, 0xBAEA46EFu, 0xB7A96036u, 0xB3687D81u, 0xAD2F2D84u, 0xA9EE3033u, 0xA4AD16EAu, 0xA06C0B5Du,
  0xD4326D90u, 0xD0F37027u, 0xDDB056FEu, 0xD9714B49u, 0xC7361B4Cu, 0xC3F706FBu, 0xCEB42022u, 0xCA753D95u, 0xF23A8028u, 0xF6FB9D9Fu, 0xFBB8BB46u, 0xFF79A6F1u, 0xE13EF6F4u, 0xE5FFEB43u, 0xE8BCCD9Au, 0xEC7DD02Du,
  0x34867077u, 0x30476DC0u, 0x3D044B19u, 0x39C556AEu, 0x278206ABu, 0x23431B1Cu, 0x2E003DC5u, 0x2AC12072u, 0x128E9DCFu, 0x164F8078u, 0x1B0CA6A1u, 0x1FCDBB16u, 0x018AEB13u, 0x054BF6A4u, 0x0808D07Du, 0x0CC9CDCAu,
  0x7897AB07u, 0x7C56B6B0u, 0x71159069u, 0x75D48DDEu, 0x6B93DDDBu, 0x6F52C06Cu, 0x6211E6B5u, 0x66D0FB02u, 0x5E9F46BFu, 0x5A5E5B08u, 0x571D7DD1u, 0x53DC6066u, 0x4D9B3063u, 0x495A2DD4u, 0x44190B0Du, 0x40D816BAu,
  0xACA5C697u, 0xA864DB20u, 0xA527FDF9u, 0xA1E6E04Eu, 0xBFA1B04Bu, 0xBB60ADFCu, 0xB6238B25u, 0xB2E29692u, 0x8AAD2B2Fu, 0x8E6C3698u, 0x832F1041u, 0x87EE0DF6u, 0x99A95DF3u, 0x9D684044u, 0x902B669Du, 0x94EA7B2Au,
  0xE0B41DE7u, 0xE4750050u, 0xE9362689u, 0xEDF73B3Eu, 0xF3B06B3Bu, 0xF771768Cu, 0xFA325055u, 0xFEF34DE2u, 0xC6BCF05Fu, 0xC27DEDE8u, 0xCF3ECB31u, 0xCBFFD686u, 0xD5B88683u, 0xD1799B34u, 0xDC3ABDEDu, 0xD8FBA05Au,
  0x690CE0EEu, 0x6DCDFD59u, 0x608EDB80u, 0x644FC637u, 0x7A089632u, 0x7EC98B85u, 0x738AAD5Cu, 0x774BB0EBu, 0x4F040D56u, 0x4BC510E1u, 0x46863638u, 0x42472B8Fu, 0x5C007B8Au, 0x58C1663Du, 0x558240E4u, 0x51435D53u,
  0x251D3B9Eu, 0x21DC2629u, 0x2C9F00F0u, 0x285E1D47u, 0x36194D42u, 0x32D850F5u, 0x3F9B762Cu, 0x3B5A6B9Bu, 0x0315D626u, 0x07D4CB91u, 0x0A97ED48u, 0x0E56F0FFu, 0x1011A0FAu, 0x14D0BD4Du, 0x19939B94u, 0x1D528623u,
  0xF12F560Eu, 0xF5EE4BB9u, 0xF8AD6D60u, 0xFC6C70D7u, 0xE22B20D2u, 0xE6EA3D65u, 0xEBA91BBCu, 0xEF68060Bu, 0xD727BBB6u, 0xD3E6A601u, 0xDEA580D8u, 0xDA649D6Fu, 0xC423CD6Au, 0xC0E2D0DDu, 0xCDA1F604u, 0xC960EBB3u,
  0xBD3E8D7Eu, 0xB9FF90C9u, 0xB4BCB610u, 0xB07DABA7u, 0xAE3AFBA2u, 0xAAFBE615u, 0xA7B8C0CCu, 0xA379DD7Bu, 0x9B3660C6u, 0x9FF77D71u, 0x92B45BA8u, 0x9675461Fu, 0x8832161Au, 0x8CF30BADu, 0x81B02D74u, 0x857130C3u,
  0x5D8A9099u, 0x594B8D2Eu, 0x5408ABF7u, 0x50C9B640u, 0x4E8EE645u, 0x4A4FFBF2u, 0x470CDD2Bu, 0x43CDC09Cu, 0x7B827D21u, 0x7F436096u, 0x7200464Fu, 0x76C15BF8u, 0x68860BFDu, 0x6C47164Au, 0x61043093u, 0x65C52D24u,
  0x119B4BE9u, 0x155A565Eu, 0x18197087u, 0x1CD86D30u, 0x029F3D35u, 0x065E2082u, 0x0B1D065Bu, 0x0FDC1BECu, 0x3793A651u, 0x3352BBE6u, 0x3E119D3Fu, 0x3AD08088u, 0x2497D08Du, 0x2056CD3Au, 0x2D15EBE3u, 0x29D4F654u,
  0xC5A92679u, 0xC1683BCEu, 0xCC2B1D17u, 0xC8EA00A0u, 0xD6AD50A5u, 0xD26C4D12u, 0xDF2F6BCBu, 0xDBEE767Cu, 0xE3A1CBC1u, 0xE760D676u, 0xEA23F0AFu, 0xEEE2ED18u, 0xF0A5BD1Du, 0xF464A0AAu, 0xF9278673u, 0xFDE69BC4u,
  0x89B8FD09u, 0x8D79E0BEu, 0x803AC667u, 0x84FBDBD0u, 0x9ABC8BD5u, 0x9E7D9662u, 0x933EB0BBu, 0x97FFAD0Cu, 0xAFB010B1u, 0xAB710D06u, 0xA6322BDFu, 0xA2F33668u, 0xBCB4666Du, 0xB8757BDAu, 0xB5365D03u, 0xB1F740B4u,
};

const CRC_MODEL_32 CRC_ModelCRC32 = {
  0x04C11DB7u,
  0xFFFFFFFFu,
  DEF_NO,
  0xFFFFFFFFu,
  &CRC_TblCRC32[0]
};
#endif

#if (FS_STORAGE_CFG_CRC32_REF_EN == DEF_ENABLED)
const CPU_INT32U CRC_TblCRC32_ref[256] = {
  0x00000000u, 0x77073096u, 0xEE0E612Cu, 0x990951BAu, 0x076DC419u, 0x706AF48Fu, 0xE963A535u, 0x9E6495A3u, 0x0EDB8832u, 0x79DCB8A4u, 0xE0D5E91Eu, 0x97D2D988u, 0x09B64C2Bu, 0x7EB17CBDu, 0xE7B82D07u, 0x90BF1D91u,
  0x1DB71064u, 0x6AB020F2u, 0xF3B97148u, 0x84BE41DEu, 0x1ADAD47Du, 0x6DDDE4EBu, 0xF4D4B551u, 0x83D385C7u, 0x136C9856u, 0x646BA8C0u, 0xFD62F97Au, 0x8A65C9ECu, 0x14015C4Fu, 0x63066CD9u, 0xFA0F3D63u, 0x8D080DF5u,
  0x3B6E20C8u, 0x4C69105Eu, 0xD56041E4u, 0xA2677172u, 0x3C03E4D1u, 0x4B04D447u, 0xD20D85FDu, 0xA50AB56Bu, 0x35B5A8FAu, 0x42B2986Cu, 0xDBBBC9D6u, 0xACBCF940u, 0x32D86CE3u, 0x45DF5C75u, 0xDCD60DCFu, 0xABD13D59u,
  0x26D930ACu, 0x51DE003Au, 0xC8D75180u, 0xBFD06116u, 0x21B4F4B5u, 0x56B3C423u, 0xCFBA9599u, 0xB8BDA50Fu, 0x2802B89Eu, 0x5F058808u, 0xC60CD9B2u, 0xB10BE924u, 0x2F6F7C87u, 0x58684C11u, 0xC1611DABu, 0xB6662D3Du,
  0x76DC4190u, 0x01DB7106u, 0x98D220BCu, 0xEFD5102Au, 0x71B18589u, 0x06B6B51Fu, 0x9FBFE4A5u, 0xE8B8D433u, 0x7807C9A2u, 0x0F00F934u, 0x9609A88Eu, 0xE10E9818u, 0x7F6A0DBBu, 0x086D3D2Du, 0x91646C97u, 0xE6635C01u,
  0x6B6B51F4u, 0x1C6C6162u, 0x856530D8u, 0xF262004Eu, 0x6C0695EDu, 0x1B01A57Bu, 0x8208F4C1u, 0xF50FC457u, 0x65B0D9C6u, 0x12B7E950u, 0x8BBEB8EAu, 0xFCB9887Cu, 0x62DD1DDFu, 0x15DA2D49u, 0x8CD37CF3u, 0xFBD44C65u,
  0x4DB26158u, 0x3AB551CEu, 0xA3BC0074u, 0xD4BB30E2u, 0x4ADFA541u, 0x3DD895D7u, 0xA4D1C46Du, 0xD3D6F4FBu, 0x4369E96Au, 0x346ED9FCu, 0xAD678846u, 0xDA60B8D0u, 0x44042D73u, 0x33031DE5u, 0xAA0A4C5Fu, 0xDD0D7CC9u,
  0x5005713Cu, 0x270241AAu, 0xBE0B1010u, 0xC90C2086u, 0x5768B525u, 0x206F85B3u, 0xB966D409u, 0xCE61E49Fu, 0x5EDEF90Eu, 0x29D9C998u, 0xB0D09822u, 0xC7D7A8B4u, 0x59B33D17u, 0x2EB40D81u, 0xB7BD5C3Bu, 0xC0BA6CADu,
  0xEDB88320u, 0x9ABFB3B6u, 0x03B6E20Cu, 0x74B1D29Au, 0xEAD54739u, 0x9DD277AFu, 0x04DB2615u, 0x73DC1683u, 0xE3630B12u, 0x94643B84u, 0x0D6D6A3Eu, 0x7A6A5AA8u, 0xE40ECF0Bu, 0x9309FF9Du, 0x0A00AE27u, 0x7D079EB1u,
  0xF00F9344u, 0x8708A3D2u, 0x1E01F268u, 0x6906C2FEu, 0xF762575Du, 0x806567CBu, 0x196C3671u, 0x6E6B06E7u, 0xFED41B76u, 0x89D32BE0u, 0x10DA7A5Au, 0x67DD4ACCu, 0xF9B9DF6Fu, 0x8EBEEFF9u, 0x17B7BE43u, 0x60B08ED5u,
  0xD6D6A3E8u, 0xA1D1937Eu, 0x38D8C2C4u, 0x4FDFF252u, 0xD1BB67F1u, 0xA6BC5767u, 0x3FB506DDu, 0x48B2364Bu, 0xD80D2BDAu, 0xAF0A1B4Cu, 0x36034AF6u, 0x41047A60u, 0xDF60EFC3u, 0xA867DF55u, 0x316E8EEFu, 0x4669BE79u,
  0xCB61B38Cu, 0xBC66831Au, 0x256FD2A0u, 0x5268E236u, 0xCC0C7795u, 0xBB0B4703u, 0x220216B9u, 0x5505262Fu, 0xC5BA3BBEu, 0xB2BD0B28u, 0x2BB45A92u, 0x5CB36A04u, 0xC2D7FFA7u, 0xB5D0CF31u, 0x2CD99E8Bu, 0x5BDEAE1Du,
  0x9B64C2B0u, 0xEC63F226u, 0x756AA39Cu, 0x026D930Au, 0x9C0906A9u, 0xEB0E363Fu, 0x72076785u, 0x05005713u, 0x95BF4A82u, 0xE2B87A14u, 0x7BB12BAEu, 0x0CB61B38u, 0x92D28E9Bu, 0xE5D5BE0Du, 0x7CDCEFB7u, 0x0BDBDF21u,
  0x86D3D2D4u, 0xF1D4E242u, 0x68DDB3F8u, 0x1FDA836Eu, 0x81BE16CDu, 0xF6B9265Bu, 0x6FB077E1u, 0x18B74777u, 0x88085AE6u, 0xFF0F6A70u, 0x66063BCAu, 0x11010B5Cu, 0x8F659EFFu, 0xF862AE69u, 0x616BFFD3u, 0x166CCF45u,
  0xA00AE278u, 0xD70DD2EEu, 0x4E048354u, 0x3903B3C2u, 0xA7672661u, 0xD06016F7u, 0x4969474Du, 0x3E6E77DBu, 0xAED16A4Au, 0xD9D65ADCu, 0x40DF0B66u, 0x37D83BF0u, 0xA9BCAE53u, 0xDEBB9EC5u, 0x47B2CF7Fu, 0x30B5FFE9u,
  0xBDBDF21Cu, 0xCABAC28Au, 0x53B39330u, 0x24B4A3A6u, 0xBAD03605u, 0xCDD70693u, 0x54DE5729u, 0x23D967BFu, 0xB3667A2Eu, 0xC4614AB8u, 0x5D681B02u, 0x2A6F2B94u, 0xB40BBE37u, 0xC30C8EA1u, 0x5A05DF1Bu, 0x2D02EF8Du
};

const CRC_MODEL_32 CRC_ModelCRC32_ref = {
  0x04C11DB7u,
  0xFFFFFFFFu,
  DEF_YES,
  0xFFFFFFFFu,
  &CRC_TblCRC32_ref[0]
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL TABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static const CPU_INT08U CRC_ReflectTbl[256] = {
  0x00u, 0x80u, 0x40u, 0xC0u, 0x20u, 0xA0u, 0x60u, 0xE0u, 0x10u, 0x90u, 0x50u, 0xD0u, 0x30u, 0xB0u, 0x70u, 0xF0u,
  0x08u, 0x88u, 0x48u, 0xC8u, 0x28u, 0xA8u, 0x68u, 0xE8u, 0x18u, 0x98u, 0x58u, 0xD8u, 0x38u, 0xB8u, 0x78u, 0xF8u,
  0x04u, 0x84u, 0x44u, 0xC4u, 0x24u, 0xA4u, 0x64u, 0xE4u, 0x14u, 0x94u, 0x54u, 0xD4u, 0x34u, 0xB4u, 0x74u, 0xF4u,
  0x0Cu, 0x8Cu, 0x4Cu, 0xCCu, 0x2Cu, 0xACu, 0x6Cu, 0xECu, 0x1Cu, 0x9Cu, 0x5Cu, 0xDCu, 0x3Cu, 0xBCu, 0x7Cu, 0xFCu,
  0x02u, 0x82u, 0x42u, 0xC2u, 0x22u, 0xA2u, 0x62u, 0xE2u, 0x12u, 0x92u, 0x52u, 0xD2u, 0x32u, 0xB2u, 0x72u, 0xF2u,
  0x0Au, 0x8Au, 0x4Au, 0xCAu, 0x2Au, 0xAAu, 0x6Au, 0xEAu, 0x1Au, 0x9Au, 0x5Au, 0xDAu, 0x3Au, 0xBAu, 0x7Au, 0xFAu,
  0x06u, 0x86u, 0x46u, 0xC6u, 0x26u, 0xA6u, 0x66u, 0xE6u, 0x16u, 0x96u, 0x56u, 0xD6u, 0x36u, 0xB6u, 0x76u, 0xF6u,
  0x0Eu, 0x8Eu, 0x4Eu, 0xCEu, 0x2Eu, 0xAEu, 0x6Eu, 0xEEu, 0x1Eu, 0x9Eu, 0x5Eu, 0xDEu, 0x3Eu, 0xBEu, 0x7Eu, 0xFEu,
  0x01u, 0x81u, 0x41u, 0xC1u, 0x21u, 0xA1u, 0x61u, 0xE1u, 0x11u, 0x91u, 0x51u, 0xD1u, 0x31u, 0xB1u, 0x71u, 0xF1u,
  0x09u, 0x89u, 0x49u, 0xC9u, 0x29u, 0xA9u, 0x69u, 0xE9u, 0x19u, 0x99u, 0x59u, 0xD9u, 0x39u, 0xB9u, 0x79u, 0xF9u,
  0x05u, 0x85u, 0x45u, 0xC5u, 0x25u, 0xA5u, 0x65u, 0xE5u, 0x15u, 0x95u, 0x55u, 0xD5u, 0x35u, 0xB5u, 0x75u, 0xF5u,
  0x0Du, 0x8Du, 0x4Du, 0xCDu, 0x2Du, 0xADu, 0x6Du, 0xEDu, 0x1Du, 0x9Du, 0x5Du, 0xDDu, 0x3Du, 0xBDu, 0x7Du, 0xFDu,
  0x03u, 0x83u, 0x43u, 0xC3u, 0x23u, 0xA3u, 0x63u, 0xE3u, 0x13u, 0x93u, 0x53u, 0xD3u, 0x33u, 0xB3u, 0x73u, 0xF3u,
  0x0Bu, 0x8Bu, 0x4Bu, 0xCBu, 0x2Bu, 0xABu, 0x6Bu, 0xEBu, 0x1Bu, 0x9Bu, 0x5Bu, 0xDBu, 0x3Bu, 0xBBu, 0x7Bu, 0xFBu,
  0x07u, 0x87u, 0x47u, 0xC7u, 0x27u, 0xA7u, 0x67u, 0xE7u, 0x17u, 0x97u, 0x57u, 0xD7u, 0x37u, 0xB7u, 0x77u, 0xF7u,
  0x0Fu, 0x8Fu, 0x4Fu, 0xCFu, 0x2Fu, 0xAFu, 0x6Fu, 0xEFu, 0x1Fu, 0x9Fu, 0x5Fu, 0xDFu, 0x3Fu, 0xBFu, 0x7Fu, 0xFFu
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_CRC_OPTIMIZE_ASM_EN == DEF_DISABLED)
static CPU_INT16U CRC_ChkSumCalcTbl_16Bit(CPU_INT16U       init_val,
                                          const CPU_INT16U *p_tbl,
                                          CPU_INT08U       *p_data,
                                          CPU_SIZE_T       size);

static CPU_INT16U CRC_ChkSumCalcTbl_16Bit_ref(CPU_INT16U       init_val,
                                              const CPU_INT16U *p_tbl,
                                              CPU_INT08U       *p_data,
                                              CPU_SIZE_T       size);

static CPU_INT32U CRC_ChkSumCalcTbl_32Bit(CPU_INT32U       init_val,
                                          const CPU_INT32U *p_tbl,
                                          CPU_INT08U       *p_data,
                                          CPU_SIZE_T       size);

static CPU_INT32U CRC_ChkSumCalcTbl_32Bit_ref(CPU_INT32U       init_val,
                                              const CPU_INT32U *p_tbl,
                                              CPU_INT08U       *p_data,
                                              CPU_SIZE_T       size);
#endif

static CPU_INT16U CRC_ChkSumCalcNoTbl_16Bit(CPU_INT16U init_val,
                                            CPU_INT16U poly,
                                            CPU_INT08U *p_data,
                                            CPU_SIZE_T size);

static CPU_INT16U CRC_ChkSumCalcNoTbl_16Bit_ref(CPU_INT16U init_val,
                                                CPU_INT16U poly,
                                                CPU_INT08U *p_data,
                                                CPU_SIZE_T size);

static CPU_INT32U CRC_ChkSumCalcNoTbl_32Bit(CPU_INT32U init_val,
                                            CPU_INT32U poly,
                                            CPU_INT08U *p_data,
                                            CPU_SIZE_T size);

static CPU_INT32U CRC_ChkSumCalcNoTbl_32Bit_ref(CPU_INT32U init_val,
                                                CPU_INT32U poly,
                                                CPU_INT08U *p_data,
                                                CPU_SIZE_T size);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               CRC_Open_16Bit()
 *
 * @brief    Open (begin) a CRC calculation for a data stream.
 *
 * @param    p_model     Pointer to model to use in calculation.
 *
 * @param    p_calc      Pointer to calculation.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *                       EDC_CRC_ERR_NONE        CRC calculated successfully.
 *                       EDC_CRC_ERR_NULL_PTR    Argument 'p_model'/'p_calc' passed a NULL pointer.
 *
 * @note     (1) For a data stream CRC calculation, the CRC model MUST include a table of pre-computed
 *               CRC values.  See 'CRC_ChkSumCalc_16Bit()  Notes #1u, #2'.
 *
 * @note     (2) A data stream CRC calculation proceeds as follows :
 *           - (a) 'CRC_Open_16Bit()'    called to initialize 'CRC_CALC_16' structure.
 *           - (b) 'CRC_WrBlock_16Bit()' called for each block of bytes to factor into CRC.
 *                 AND / OR
 *                 'CRC_WrOctet_16Bit()' called for each octet          to factor into CRC.
 *           - (c) 'CRC_Close_16Bit()'   called to get final CRC value.
 *******************************************************************************************************/
void CRC_Open_16Bit(CRC_MODEL_16 *p_model,
                    CRC_CALC_16  *p_calc,
                    RTOS_ERR     *p_err)
{
  CRC_MODEL_16 *p_model_calc;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG(p_model != DEF_NULL, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG(p_calc != DEF_NULL, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG(p_model->TblPtr != DEF_NULL, RTOS_ERR_INVALID_CFG,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ------------------ INIT CALC INFO ------------------
  p_model_calc = &p_calc->Model;
  p_model_calc->Poly = p_model->Poly;
  p_model_calc->InitVal = p_model->InitVal;
  p_model_calc->Reflect = p_model->Reflect;
  p_model_calc->XorOut = p_model->XorOut;
  p_model_calc->TblPtr = p_model->TblPtr;

  if (p_model->Reflect == DEF_YES) {
    p_calc->CRC_Curr = CRC_Reflect_16Bit(p_model->InitVal);
  } else {
    p_calc->CRC_Curr = p_model->InitVal;
  }
}

/****************************************************************************************************//**
 *                                           CRC_WrBlock_16Bit()
 *
 * @brief    Process buffer for data stream CRC calculation.
 *
 * @param    p_calc  Pointer to calculation.
 *
 * @param    p_data  Pointer to data buffer.
 *
 * @param    size    Size of buffer, in octets.
 *
 * @note     (1) See 'CRC_Open_16Bit()  Note #1'.
 *******************************************************************************************************/
#if (FS_STORAGE_CFG_CRC_OPTIMIZE_ASM_EN == DEF_DISABLED)
void CRC_WrBlock_16Bit(CRC_CALC_16 *p_calc,
                       void        *p_data,
                       CPU_SIZE_T  size)
{
  CPU_INT08U  *p_data_08;
  CPU_INT16U  crc;
  CPU_BOOLEAN reflect;

  reflect = (p_calc->Model).Reflect;
  p_data_08 = (CPU_INT08U *)p_data;

  if (reflect == DEF_YES) {
    crc = CRC_ChkSumCalcTbl_16Bit_ref(p_calc->CRC_Curr,
                                      (p_calc->Model).TblPtr,
                                      p_data_08,
                                      size);
  } else {
    crc = CRC_ChkSumCalcTbl_16Bit(p_calc->CRC_Curr,
                                  (p_calc->Model).TblPtr,
                                  p_data_08,
                                  size);
  }

  p_calc->CRC_Curr = crc;
}
#endif

/****************************************************************************************************//**
 *                                           CRC_WrOctet_16Bit()
 *
 * @brief    Process octet for data stream CRC calculation.
 *
 * @param    p_calc  Pointer to calculation.
 *
 * @param    octet   Octet for CRC calculation.
 *
 * @note     (1) See 'CRC_Open_16Bit()  Note #1'.
 *******************************************************************************************************/
void CRC_WrOctet_16Bit(CRC_CALC_16 *p_calc,
                       CPU_INT08U  octet)
{
  CPU_INT16U       crc;
  CPU_INT08U       ix;
  CPU_BOOLEAN      reflect;
  const CPU_INT16U *p_tbl;

  crc = p_calc->CRC_Curr;
  reflect = (p_calc->Model).Reflect;
  p_tbl = (p_calc->Model).TblPtr;

  if (reflect == DEF_YES) {
    ix = (CPU_INT08U)(crc ^ octet) & 0xFFu;
    crc = p_tbl[ix] ^ (CPU_INT16U)(crc >> 8);
  } else {
    ix = (CPU_INT08U)((CPU_INT08U)(crc >> 8) ^ octet) & 0xFFu;
    crc = p_tbl[ix] ^ (CPU_INT16U)(crc << 8);
  }

  p_calc->CRC_Curr = crc;
}

/****************************************************************************************************//**
 *                                               CRC_Close_16Bit()
 *
 * @brief    Close (end) CRC calculation.
 *
 * @param    p_calc  Pointer to calculation.
 *
 * @return   16-bit CRC.
 *
 * @note     (1) See 'CRC_Open_16Bit()  Note #1'.
 *******************************************************************************************************/
CPU_INT16U CRC_Close_16Bit(CRC_CALC_16 *p_calc)
{
  CPU_INT16U crc;

  crc = p_calc->CRC_Curr;
  crc ^= (p_calc->Model).XorOut;

  return (crc);
}

/****************************************************************************************************//**
 *                                               CRC_Open_32Bit()
 *
 * @brief    Open (begin) a CRC calculation for a data stream.
 *
 * @param    p_model     Pointer to model to use in calculation.
 *
 * @param    p_calc      Pointer to calculation.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *                       EDC_CRC_ERR_NONE        CRC calculated successfully.
 *                       EDC_CRC_ERR_NULL_PTR    Argument 'p_model'/'p_calc' passed a NULL pointer.
 *
 * @note     (1) For a data stream CRC calculation, the CRC model MUST include a table of pre-computed
 *               CRC values.  See 'CRC_ChkSumCalc_32Bit()  Notes #1u, #2'.
 *
 * @note     (2) A data stream CRC calculation proceeds as follows :
 *           - (a) 'CRC_Open_32Bit()'    called to initialize 'CRC_CALC_32' structure.
 *
 *           - (b) 'CRC_WrBlock_32Bit()' called for each block of bytes to factor into CRC.
 *                 AND / OR
 *                 'CRC_WrOctet_32Bit()' called for each octet          to factor into CRC.
 *           - (c) 'CRC_Close_32Bit()'   called to get final CRC value.
 *******************************************************************************************************/
void CRC_Open_32Bit(CRC_MODEL_32 *p_model,
                    CRC_CALC_32  *p_calc,
                    RTOS_ERR     *p_err)
{
  CRC_MODEL_32 *p_model_calc;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG(p_model != DEF_NULL, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG(p_calc != DEF_NULL, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG(p_model->TblPtr != DEF_NULL, RTOS_ERR_INVALID_CFG,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ------------------ INIT CALC INFO ------------------
  p_model_calc = &p_calc->Model;
  p_model_calc->Poly = p_model->Poly;
  p_model_calc->InitVal = p_model->InitVal;
  p_model_calc->Reflect = p_model->Reflect;
  p_model_calc->XorOut = p_model->XorOut;
  p_model_calc->TblPtr = p_model->TblPtr;

  if (p_model->Reflect == DEF_YES) {
    p_calc->CRC_Curr = CRC_Reflect_32Bit(p_model->InitVal);
  } else {
    p_calc->CRC_Curr = p_model->InitVal;
  }
}

/****************************************************************************************************//**
 *                                           CRC_WrBlock_32Bit()
 *
 * @brief    Process buffer for data stream CRC calculation.
 *
 * @param    p_calc  Pointer to calculation.
 *
 * @param    p_data  Pointer to data buffer.
 *
 * @param    size    Size of buffer, in octets.
 *
 * @note     (1) See 'CRC_Open_32Bit()  Note #1'.
 *******************************************************************************************************/
#if (FS_STORAGE_CFG_CRC_OPTIMIZE_ASM_EN == DEF_DISABLED)
void CRC_WrBlock_32Bit(CRC_CALC_32 *p_calc,
                       void        *p_data,
                       CPU_SIZE_T  size)
{
  CPU_INT08U  *p_data_08;
  CPU_INT32U  crc;
  CPU_BOOLEAN reflect;

  reflect = (p_calc->Model).Reflect;
  p_data_08 = (CPU_INT08U *)p_data;

  if (reflect == DEF_YES) {
    crc = CRC_ChkSumCalcTbl_32Bit_ref(p_calc->CRC_Curr,
                                      (p_calc->Model).TblPtr,
                                      p_data_08,
                                      size);
  } else {
    crc = CRC_ChkSumCalcTbl_32Bit(p_calc->CRC_Curr,
                                  (p_calc->Model).TblPtr,
                                  p_data_08,
                                  size);
  }

  p_calc->CRC_Curr = crc;
}
#endif

/****************************************************************************************************//**
 *                                           CRC_WrOctet_32Bit()
 *
 * @brief    Process octet for data stream CRC calculation.
 *
 * @param    p_calc  Pointer to calculation.
 *
 * @param    octet   Octet for CRC calculation.
 *
 * @note     (1) See 'CRC_Open_32Bit()  Note #1'.
 *******************************************************************************************************/
void CRC_WrOctet_32Bit(CRC_CALC_32 *p_calc,
                       CPU_INT08U  octet)
{
  CPU_INT32U       crc;
  CPU_INT08U       ix;
  CPU_BOOLEAN      reflect;
  const CPU_INT32U *p_tbl;

  crc = p_calc->CRC_Curr;
  reflect = (p_calc->Model).Reflect;
  p_tbl = (p_calc->Model).TblPtr;

  if (reflect == DEF_YES) {
    ix = (CPU_INT08U)(crc ^ octet) & 0xFFu;
    crc = p_tbl[ix] ^ (CPU_INT32U)(crc >> 8);
  } else {
    ix = (CPU_INT08U)(((crc >> 24) ^ octet) & 0x000000FFu);
    crc = p_tbl[ix] ^ (CPU_INT32U)(crc << 8);
  }

  p_calc->CRC_Curr = crc;
}

/****************************************************************************************************//**
 *                                               CRC_Close_32Bit()
 *
 * @brief    Close (end) CRC calculation.
 *
 * @param    p_calc  Pointer to calculation.
 *
 * @return   32-bit CRC.
 *
 * @note     (1) See 'CRC_Open_32Bit()  Note #1'.
 *******************************************************************************************************/
CPU_INT32U CRC_Close_32Bit(CRC_CALC_32 *p_calc)
{
  CPU_INT32U crc;

  crc = p_calc->CRC_Curr;
  crc ^= (p_calc->Model).XorOut;

  return (crc);
}

/****************************************************************************************************//**
 *                                           CRC_ChkSumCalc_16Bit()
 *
 * @brief    Calculate a 16-bit CRC.
 *
 * @param    p_model     Pointer to model to use in calculation.
 *
 * @param    p_data      Pointer to data buffer.
 *
 * @param    size        Size of buffer, in octets.
 *
 * @return   16-bit CRC.
 *
 * @note     (1) If 'p_model->TblPtr' is a NULL pointer, then this function will use a slower method
 *               for calculating the CRC that does NOT use a pre-computed table.  Otherwise, the table
 *               located at 'p_model->TblPtr' will be used in the computation.
 *
 * @note     (2) If you have not pre-computed a table (using 'CRC_TblMake_16Bit()') or are not using
 *               one of the provided tables, then be certain that 'p_model->TblPtr' is a NULL pointer.
 *******************************************************************************************************/
CPU_INT16U CRC_ChkSumCalc_16Bit(CRC_MODEL_16 *p_model,
                                void         *p_data,
                                CPU_SIZE_T   size)
{
  CPU_INT08U *p_data_08;
  CPU_INT16U crc = 0u;
  CPU_INT16U init_val;

  RTOS_ASSERT_DBG(p_model != DEF_NULL, RTOS_ERR_NULL_PTR, 0);
  RTOS_ASSERT_DBG(p_data != DEF_NULL, RTOS_ERR_NULL_PTR, 0);

  //                                                               ----------------------- INIT CALC ------------------
  p_data_08 = (CPU_INT08U *)p_data;

  if (p_model->Reflect == DEF_YES) {
    init_val = CRC_Reflect_16Bit(p_model->InitVal);
  } else {
    init_val = p_model->InitVal;
  }

  //                                                               ------------------------ DO CALC -------------------
  if (p_model->TblPtr == (const CPU_INT16U *)0) {               // See Note #1.
    if (p_model->Reflect == DEF_YES) {
      crc = CRC_ChkSumCalcNoTbl_16Bit_ref(init_val,
                                          p_model->Poly,
                                          p_data_08,
                                          size);
    } else {
      crc = CRC_ChkSumCalcNoTbl_16Bit(init_val,
                                      p_model->Poly,
                                      p_data_08,
                                      size);
    }
  } else {
#if (FS_STORAGE_CFG_CRC_OPTIMIZE_ASM_EN == DEF_DISABLED)
    if (p_model->Reflect == DEF_YES) {
      crc = CRC_ChkSumCalcTbl_16Bit_ref(init_val,
                                        p_model->TblPtr,
                                        p_data_08,
                                        size);
    } else {
      crc = CRC_ChkSumCalcTbl_16Bit(init_val,
                                    p_model->TblPtr,
                                    p_data_08,
                                    size);
    }
#else
    RTOS_DBG_FAIL_EXEC(RTOS_ERR_NOT_AVAIL, 0);
#endif
  }

  crc ^= p_model->XorOut;

  return (crc);
}

/****************************************************************************************************//**
 *                                           CRC_ChkSumCalc_32Bit()
 *
 * @brief    Calculate a 32-bit CRC.
 *
 * @param    p_model     Pointer to model to use in calculation.
 *
 * @param    p_data      Pointer to data buffer.
 *
 * @param    size        Size of buffer, in octets.
 *
 * @return   32-bit CRC.
 *
 * @note     (1) If 'p_model->TblPtr' is a NULL pointer, then this function will use a slower method
 *               for calculating the CRC that does NOT use a pre-computed table.  Otherwise, the table
 *               located at 'pm_odel->TblPtr' will be used in the computation.
 *
 * @note     (2) If you have not pre-computed a table (using 'CRC_TblMake_32Bit()') or are not using
 *               one of the provided tables, then be certain that 'p_model->TblPtr' is a NULL pointer.
 *******************************************************************************************************/
CPU_INT32U CRC_ChkSumCalc_32Bit(CRC_MODEL_32 *p_model,
                                void         *p_data,
                                CPU_SIZE_T   size)
{
  CPU_INT08U *p_data_08;
  CPU_INT32U crc = 0u;
  CPU_INT32U init_val;

  RTOS_ASSERT_DBG(p_model != DEF_NULL, RTOS_ERR_NULL_PTR, 0);
  RTOS_ASSERT_DBG(p_data != DEF_NULL, RTOS_ERR_NULL_PTR, 0);

  //                                                               ----------------------- INIT CALC ------------------
  p_data_08 = (CPU_INT08U *)p_data;

  if (p_model->Reflect == DEF_YES) {
    init_val = CRC_Reflect_32Bit(p_model->InitVal);
  } else {
    init_val = p_model->InitVal;
  }

  //                                                               ------------------------ DO CALC -------------------
  if (p_model->TblPtr == (const CPU_INT32U *)0) {               // See Note #1.
    if (p_model->Reflect == DEF_YES) {
      crc = CRC_ChkSumCalcNoTbl_32Bit_ref(init_val,
                                          p_model->Poly,
                                          p_data_08,
                                          size);
    } else {
      crc = CRC_ChkSumCalcNoTbl_32Bit(init_val,
                                      p_model->Poly,
                                      p_data_08,
                                      size);
    }
  } else {
#if (FS_STORAGE_CFG_CRC_OPTIMIZE_ASM_EN == DEF_DISABLED)
    if (p_model->Reflect == DEF_YES) {
      crc = CRC_ChkSumCalcTbl_32Bit_ref(init_val,
                                        p_model->TblPtr,
                                        p_data_08,
                                        size);
    } else {
      crc = CRC_ChkSumCalcTbl_32Bit(init_val,
                                    p_model->TblPtr,
                                    p_data_08,
                                    size);
    }
#else
    RTOS_DBG_FAIL_EXEC(RTOS_ERR_NOT_AVAIL, 0);
#endif
  }

  crc ^= p_model->XorOut;

  return (crc);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                           CRC_ChkSumCalcTbl_16Bit() / CRC_ChkSumCalcTbl_16Bit_ref()
 *
 * @brief    Calculate a 16-bit CRC using a table with or without reflection.
 *
 * @param    init_val    Initial CRC value.
 *
 * @param    p_tbl       Pre-computed CRC table to use in calculation.
 *
 * @param    p_data      Pointer to data buffer.
 *
 * @param    size        Size of buffer, in octets.
 *
 * @return   16-bit CRC.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_CRC_OPTIMIZE_ASM_EN == DEF_DISABLED)
static CPU_INT16U CRC_ChkSumCalcTbl_16Bit(CPU_INT16U       init_val,
                                          const CPU_INT16U *p_tbl,
                                          CPU_INT08U       *p_data,
                                          CPU_SIZE_T       size)
{
  CPU_INT16U crc;
  CPU_INT08U ix;

  crc = init_val;
  while (size > 0u) {
    ix = (CPU_INT08U)((CPU_INT08U)(crc >> 8) ^ *p_data) & 0xFFu;
    crc = p_tbl[ix] ^ (CPU_INT16U)(crc << 8);
    p_data += sizeof(CPU_INT08U);
    size -= sizeof(CPU_INT08U);
  }
  return (crc);
}

static CPU_INT16U CRC_ChkSumCalcTbl_16Bit_ref(CPU_INT16U       init_val,
                                              const CPU_INT16U *p_tbl,
                                              CPU_INT08U       *p_data,
                                              CPU_SIZE_T       size)
{
  CPU_INT16U crc;
  CPU_INT08U ix;

  crc = init_val;
  while (size > 0u) {
    ix = (CPU_INT08U)(crc ^ *p_data) & 0xFFu;
    crc = p_tbl[ix] ^ (CPU_INT08U)(crc >> 8);
    p_data += sizeof(CPU_INT08U);
    size -= sizeof(CPU_INT08U);
  }
  return (crc);
}
#endif

/****************************************************************************************************//**
 *                           CRC_ChkSumCalcTbl_32Bit() / CRC_ChkSumCalcTbl_32Bit_ref()
 *
 * @brief    Calculate a 32-bit CRC using a table with or without reflection.
 *
 * @param    init_val    Initial CRC value.
 *
 * @param    p_tbl       Pre-computed CRC table to use in calculation.
 *
 * @param    p_data      Pointer to data buffer.
 *
 * @param    size        Size of buffer, in octets.
 *
 * @return   32-bit CRC.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_CRC_OPTIMIZE_ASM_EN == DEF_DISABLED)
static CPU_INT32U CRC_ChkSumCalcTbl_32Bit(CPU_INT32U       init_val,
                                          const CPU_INT32U *p_tbl,
                                          CPU_INT08U       *p_data,
                                          CPU_SIZE_T       size)
{
  CPU_INT32U crc;
  CPU_INT08U ix;

  crc = init_val;
  while (size > 0u) {
    ix = (CPU_INT08U)(((crc >> 24) ^ *p_data) & 0xFFu);
    crc = p_tbl[ix] ^ (crc << 8);
    p_data += sizeof(CPU_INT08U);
    size -= sizeof(CPU_INT08U);
  }
  return (crc);
}

static CPU_INT32U CRC_ChkSumCalcTbl_32Bit_ref(CPU_INT32U       init_val,
                                              const CPU_INT32U *p_tbl,
                                              CPU_INT08U       *p_data,
                                              CPU_SIZE_T       size)
{
  CPU_INT32U crc;
  CPU_INT08U ix;

  crc = init_val;
  while (size > 0u) {
    ix = (CPU_INT08U)(crc ^ *p_data) & 0xFFu;
    crc = p_tbl[ix] ^ (crc >> 8);
    p_data += sizeof(CPU_INT08U);
    size -= sizeof(CPU_INT08U);
  }
  return (crc);
}
#endif

/****************************************************************************************************//**
 *                       CRC_ChkSumCalcNoTbl_16Bit() / CRC_ChkSumCalcNoTbl_16Bit_ref()
 *
 * @brief    Calculate a 16-bit CRC without a table with or without reflection.
 *
 * @param    init_val    Initial CRC value.
 *
 * @param    poly        Polynomial to use in calculation.
 *
 * @param    p_data      Pointer to data buffer.
 *
 * @param    size        Size of buffer, in octets.
 *
 * @return   16-bit CRC.
 *******************************************************************************************************/
static CPU_INT16U CRC_ChkSumCalcNoTbl_16Bit(CPU_INT16U init_val,
                                            CPU_INT16U poly,
                                            CPU_INT08U *p_data,
                                            CPU_SIZE_T size)
{
  CPU_INT16U crc;
  CPU_INT08U data;
  CPU_INT08U i;

  crc = init_val;
  while (size > 0u) {
    data = *p_data;
    crc ^= (CPU_INT16U)((CPU_INT16U)data << 8);
    for (i = 0u; i < 8u; i++) {
      if (DEF_BIT_IS_SET(crc, DEF_BIT_15) == DEF_YES) {
        crc = (CPU_INT16U)(crc << 1) ^ poly;
      } else {
        crc <<= 1;
      }
    }
    size -= sizeof(CPU_INT08U);
    p_data += sizeof(CPU_INT08U);
  }
  return (crc);
}

static CPU_INT16U CRC_ChkSumCalcNoTbl_16Bit_ref(CPU_INT16U init_val,
                                                CPU_INT16U poly,
                                                CPU_INT08U *p_data,
                                                CPU_SIZE_T size)
{
  CPU_INT16U crc;
  CPU_INT08U data;
  CPU_INT08U i;

  crc = init_val;
  while (size > 0u) {
    data = CRC_Reflect_08Bit(*p_data);
    crc ^= (CPU_INT16U)((CPU_INT16U)data << 8);
    for (i = 0u; i < 8u; i++) {
      if (DEF_BIT_IS_SET(crc, DEF_BIT_15) == DEF_YES) {
        crc = (CPU_INT16U)(crc << 1) ^ poly;
      } else {
        crc <<= 1;
      }
    }
    size -= sizeof(CPU_INT08U);
    p_data += sizeof(CPU_INT08U);
  }
  crc = CRC_Reflect_16Bit(crc);
  return (crc);
}

/****************************************************************************************************//**
 *                       CRC_ChkSumCalcNoTbl_32Bit() / CRC_ChkSumCalcNoTbl_32Bit_ref()
 *
 * @brief    Calculate a 32-bit CRC without a table with or without reflection.
 *
 * @param    init_val    Initial CRC value.
 *
 * @param    poly        Polynomial to use in calculation.
 *
 * @param    p_data      Pointer to data buffer.
 *
 * @param    size        Size of buffer, in octets.
 *
 * @return   32-bit CRC.
 *******************************************************************************************************/
static CPU_INT32U CRC_ChkSumCalcNoTbl_32Bit(CPU_INT32U init_val,
                                            CPU_INT32U poly,
                                            CPU_INT08U *p_data,
                                            CPU_SIZE_T size)
{
  CPU_INT32U crc;
  CPU_INT08U data;
  CPU_INT08U i;

  crc = init_val;
  while (size > 0u) {
    data = *p_data;
    crc ^= ((CPU_INT32U)data << 24);
    for (i = 0u; i < 8u; i++) {
      if (DEF_BIT_IS_SET(crc, DEF_BIT_31) == DEF_YES) {
        crc = (crc << 1) ^ poly;
      } else {
        crc <<= 1;
      }
    }
    size -= sizeof(CPU_INT08U);
    p_data += sizeof(CPU_INT08U);
  }
  return (crc);
}

static CPU_INT32U CRC_ChkSumCalcNoTbl_32Bit_ref(CPU_INT32U init_val,
                                                CPU_INT32U poly,
                                                CPU_INT08U *p_data,
                                                CPU_SIZE_T size)
{
  CPU_INT32U crc;
  CPU_INT08U data;
  CPU_INT08U i;

  crc = init_val;
  while (size > 0u) {
    data = CRC_Reflect_08Bit(*p_data);
    crc ^= ((CPU_INT32U)data << 24);
    for (i = 0u; i < 8u; i++) {
      if (DEF_BIT_IS_SET(crc, DEF_BIT_31) == DEF_YES) {
        crc = (crc << 1) ^ poly;
      } else {
        crc <<= 1;
      }
    }
    size -= sizeof(CPU_INT08U);
    p_data += sizeof(CPU_INT08U);
  }
  crc = CRC_Reflect_32Bit(crc);
  return (crc);
}

/****************************************************************************************************//**
 *                                           CRC_Reflect_08Bit()
 *
 * @brief    Reflect a 8-bit value.
 *
 * @param    datum   The datum to reflect.
 *
 * @return   The reflected value.
 *******************************************************************************************************/
CPU_INT08U CRC_Reflect_08Bit(CPU_INT08U datum)
{
  return (CRC_ReflectTbl[datum]);
}

/****************************************************************************************************//**
 *                                           CRC_Reflect_16Bit()
 *
 * @brief    Reflect a 16-bit value.
 *
 * @param    datum   The datum to reflect.
 *
 * @return   The reflected value.
 *******************************************************************************************************/
CPU_INT16U CRC_Reflect_16Bit(CPU_INT16U datum)
{
  CPU_INT08U tmp_datum_08;
  CPU_INT08U ref_datum_08;
  CPU_INT16U ref_datum;

  tmp_datum_08 = (CPU_INT08U)((datum & 0xFF00u) >> 8);
  ref_datum = (CPU_INT16U)CRC_ReflectTbl[tmp_datum_08];

  tmp_datum_08 = (CPU_INT08U)((datum & 0x00FFu) >> 0);
  ref_datum_08 = (CPU_INT08U)CRC_ReflectTbl[tmp_datum_08];

  ref_datum |= (CPU_INT16U)((CPU_INT16U)ref_datum_08 << 8);

  return (ref_datum);
}

/****************************************************************************************************//**
 *                                           CRC_Reflect_32Bit()
 *
 * @brief    Reflect a 32-bit value.
 *
 * @param    datum   The datum to reflect.
 *
 * @return   The reflected data.
 *******************************************************************************************************/
CPU_INT32U CRC_Reflect_32Bit(CPU_INT32U datum)
{
  CPU_INT08U tmp_datum_08;
  CPU_INT08U ref_datum_08;
  CPU_INT32U ref_datum;

  tmp_datum_08 = (CPU_INT08U)((datum & 0xFF000000u) >> 24);
  ref_datum = (CPU_INT16U)CRC_ReflectTbl[tmp_datum_08];

  tmp_datum_08 = (CPU_INT08U)((datum & 0x00FF0000u) >> 16);
  ref_datum_08 = (CPU_INT08U)CRC_ReflectTbl[tmp_datum_08];
  ref_datum |= (CPU_INT32U)ref_datum_08 << 8;

  tmp_datum_08 = (CPU_INT08U)((datum & 0x0000FF00u) >>  8);
  ref_datum_08 = (CPU_INT08U)CRC_ReflectTbl[tmp_datum_08];
  ref_datum |= (CPU_INT32U)ref_datum_08 << 16;

  tmp_datum_08 = (CPU_INT08U)((datum & 0x000000FFu) >>  0);
  ref_datum_08 = (CPU_INT08U)CRC_ReflectTbl[tmp_datum_08];
  ref_datum |= (CPU_INT32U)ref_datum_08 << 24;

  return (ref_datum);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL
