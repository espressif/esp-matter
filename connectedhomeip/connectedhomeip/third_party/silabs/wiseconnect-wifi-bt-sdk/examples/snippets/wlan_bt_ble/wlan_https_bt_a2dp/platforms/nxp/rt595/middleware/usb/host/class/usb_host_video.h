/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _USB_HOST_VIDEO_H_
#define _USB_HOST_VIDEO_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Video class code */
#define USB_HOST_VIDEO_CLASS_CODE (0x0EU)
/*! @brief Video class  control interface code*/
#define USB_HOST_VIDEO_SUBCLASS_CODE_CONTROL (0x01U)
/*! @brief Video class  stream interface code*/
#define USB_HOST_VIDEO_SUBCLASS_CODE_STREAM (0x02U)
/* Video Interface Protocol Codes */
#define USB_HOST_ROTOCOL_UNDEFINED (0x00U)

/* Video Class-Specific Descriptor Types */
#define USB_HOST_DESC_CS_UNDEFINED (0x20U)
#define USB_HOST_DESC_CS_DEVICE (0x21U)
#define USB_HOST_DESC_CS_CONFIGURATION (0x22U)
#define USB_HOST_DESC_CS_STRING (0x23U)
#define USB_HOST_DESC_CS_INTERFACE (0x24U)
#define USB_HOST_DESC_CS_ENDPOINT (0x25U)

/* Video Class-Specific VC Interface Descriptor Subtypes */
#define USB_HOST_DESC_SUBTYPE_VC_UNDEFINED (0x00U)
#define USB_HOST_DESC_SUBTYPE_VC_HEADER (0x01U)
#define USB_HOST_DESC_SUBTYPE_VC_INPUT_TERMINAL (0x02U)
#define USB_HOST_DESC_SUBTYPE_VC_OUTPUT_TERMINAL (0x03U)
#define USB_HOST_DESC_SUBTYPE_VC_SECLECTOR_UNIT (0x04U)
#define USB_HOST_DESC_SUBTYPE_VC_PROCESSING_UNIT (0X05U)
#define USB_HOST_DESC_SUBTYPE_VC_EXTENSION_UNIT (0X06U)

/* Video Class-specific VS Interface Desriptor Subtypes */
#define USB_HOST_DESC_SUBTYPE_VS_UNDEFINED (0x00U)
#define USB_HOST_DESC_SUBTYPE_VS_INPUT_HEADER (0x01U)
#define USB_HOST_DESC_SUBTYPE_VS_OUTPUT_HEADER (0x02U)
#define USB_HOST_DESC_SUBTYPE_VS_STILL_IMAGE_FRAME (0x03U)
#define USB_HOST_DESC_SUBTYPE_VS_FORMAT_UNCOMPRESSED (0x04U)
#define USB_HOST_DESC_SUBTYPE_VS_FRAME_UNCOMPRESSED (0x05U)
#define USB_HOST_DESC_SUBTYPE_VS_FORMAT_MJPEG (0x06U)
#define USB_HOST_DESC_SUBTYPE_VS_FRAME_MJPEG (0x07U)
#define USB_HOST_DESC_SUBTYPE_VS_FORMAT_MPEG2TS (0x0AU)
#define USB_HOST_DESC_SUBTYPE_VS_FORMAT_DV (0x0CU)
#define USB_HOST_DESC_SUBTYPE_VS_COLOR_FORMAT (0x0DU)
#define USB_HOST_DESC_SUBTYPE_VS_FORMAT_FRAME_BASED (0x10U)
#define USB_HOST_DESC_SUBTYPE_VS_FRAME_FRAME_BASED (0x11U)
#define USB_HOST_DESC_SUBTYPE_VS_FORMAT_STREAM_BASED (0x12U)

/* Video Class-Specific Endpoint Descriptor Subtypes */
#define USB_HOST_DESC_SUBTYPE_EP_UNDEFINED (0x00U)
#define USB_HOST_DESC_SUBTYPE_EP_GENERAL (0x01U)
#define USB_HOST_DESC_SUBTYPE_EP_ENDPOINT (0x02U)
#define USB_HOST_DESC_SUBTYPE_EP_INTERRUPT (0x03U)

/* Video Class-Specific Request Codes */
#define USB_HOST_VIDEO_SET_CUR (0x01U)
#define USB_HOST_VIDEO_GET_CUR (0x81U)
#define USB_HOST_VIDEO_GET_MIN (0x82U)
#define USB_HOST_VIDEO_GET_MAX (0x83U)
#define USB_HOST_VIDEO_GET_RES (0x84U)
#define USB_HOST_VIDEO_GET_LEN (0x85U)
#define USB_HOST_VIDEO_GET_INFO (0x86U)
#define USB_HOST_VIDEO_GET_DEF (0x87U)

/* VideoControl Interface Control Selector Codes */
#define USB_HOST_VC_CONTROL_UNDEFINED (0x00U)
#define USB_HOST_VC_VIDEO_POWER_MODE_CONTROL (0x01U)
#define USB_HOST_VC_REQUEST_ERROR_CODE_CONTROL (0x02U)

/* Terminal Control Selector Codes */
#define USB_HOST_USB_HOST_TE_CONTROL_UNDEFINED (0x00U)

/* Selector Unit Control Selector Codes */
#define USB_HOST_SU_CONTROL_UNDEFINED (0x00U)
#define USB_HOST_SU_INPUT_SELECT_CONTROL (0x01U)

/* Camera Terminal Control Selector Codes */
#define USB_HOST_CT_CONTROL_UNDEFINED (0x00U)
#define USB_HOST_CT_SCANNING_MODE_CONTROL (0x01U)
#define USB_HOST_CT_AE_MODE_CONTROL (0x02U)
#define USB_HOST_CT_AE_PRIORITY_CONTROL (0x03U)
#define USB_HOST_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL (0x04U)
#define USB_HOST_CT_EXPOSURE_TIME_RELATIVE_CONTROL (0x05U)
#define USB_HOST_CT_FOCUS_ABSOLUTE_CONTROL (0x06U)
#define USB_HOST_CT_FOCUS_RELATIVE_CONTROL (0x07U)
#define USB_HOST_CT_FOCUS_AUTO_CONTROL (0x08U)
#define USB_HOST_CT_IRIS_ABSOLUTE_CONTROL (0x09U)
#define USB_HOST_CT_IRIS_RELATIVE_CONTROL (0x0AU)
#define USB_HOST_CT_ZOOM_ABSOLUTE_CONTROL (0x0BU)
#define USB_HOST_CT_ZOOM_RELATIVE_CONTROL (0x0CU)
#define USB_HOST_CT_PANTILT_ABSOLUTE_CONTROL (0x0DU)
#define USB_HOST_CT_PANTILT_RELATIVE_CONTROL (0x0EU)
#define USB_HOST_CT_ROLL_ABSOLUTE_CONTROL (0x0FU)
#define USB_HOST_CT_ROLL_RELATIVE_CONTROL (0x10U)
#define USB_HOST_CT_PRIVACY_CONTROL (0x11U)

/* Processing Unit Control Selector Codes */
#define USB_HOST_PU_CONTROL_UNDEFINED (0x00U)
#define USB_HOST_PU_BACKLIGHT_COMPENSATION_CONTROL (0x01U)
#define USB_HOST_PU_BRIGHTNESS_CONTROL (0x02U)
#define USB_HOST_PU_CONTRACT_CONTROL (0x03U)
#define USB_HOST_PU_GAIN_CONTROL (0x04U)
#define USB_HOST_PU_POWER_LINE_FREQUENCY_CONTROL (0x05U)
#define USB_HOST_PU_HUE_CONTROL (0x06U)
#define USB_HOST_PU_SATURATION_CONTROL (0x07U)
#define USB_HOST_PU_SHARRNESS_CONTROL (0x08U)
#define USB_HOST_PU_GAMMA_CONTROL (0x09U)
#define USB_HOST_PU_WHITE_BALANCE_TEMPERATURE_CONTROL (0x0AU)
#define USB_HOST_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL (0x0BU)
#define USB_HOST_PU_WHITE_BALANCE_COMPONENT_CONTROL (0x0CU)
#define USB_HOST_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL (0x0DU)
#define USB_HOST_PU_DIGITAL_MULTIPLIER_CONTROL (0x0EU)
#define USB_HOST_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL (0x0FU)
#define USB_HOST_PU_HUE_AUTO_CONTROL (0x10U)
#define USB_HOST_PU_ANALOG_VIDEO_STANDARD_CONTROL (0x11U)
#define USB_HOST_PU_ANALOG_LOCK_STATUS_CONTROL (0x12U)

/* Extension Unit Control Selectors Codes */
#define USB_HOST_XU_CONTROL_UNDEFINED (0x00U)

/* VideoStreming Unit Control Selector Codes */
#define USB_HOST_VS_CONTROL_UNDEFINED (0x00U)
#define USB_HOST_VS_PROBE_CONTROL (0x01U)
#define USB_HOST_VS_COMMIT_CONTROL (0x02U)
#define USB_HOST_VS_STILL_PROBE_CONTROL (0x03U)
#define USB_HOST_VS_STILL_COMMIT_CONTROL (0x04U)
#define USB_HOST_VS_STILL_IMAGE_TRIGGER_CONTROL (0x05U)
#define USB_HOST_VS_STREAM_ERROR_CODE_CONTROL (0x06U)
#define USB_HOST_VS_GENERATE_KEY_FRAME_CONTROL (0x07U)
#define USB_HOST_VS_UPDATE_FRAME_SEGMENT_CONTROL (0x08U)
#define USB_HOST_VS_SYNCH_DELAY_CONTROL (0x09U)

/*! @brief Video control interface header descriptor structure */
typedef struct _usb_host_video_ctrl_header_desc
{
    /*< Total size of the video control header descriptor */
    uint8_t bLength;
    /*< Descriptor type of video control header descriptor */
    uint8_t bDescriptorType;
    /*< Subtype of video control header descriptor */
    uint8_t bDescriptorSubtype;
    /*< Video Device Class Specification release number in binary-coded decimal */
    uint8_t bcdVDC[2];
    /*< Total number of bytes returned for the class-specific VideoControl interface descriptor. Includes the combined
     * length of this descriptor header and all Unit and Terminal descriptors */
    uint8_t wTotalLength[2];
    /*< The device clock frequency in Hz. This will specify the units used for the time information fields in the Video
     * Payload Headers of the primary data stream and format */
    uint8_t dwClockFrequency[4];
    /*< The number of VideoStreaming interfaces in the Video Interface Collection to which this VideoControl interface
     * belongs: n */
    uint8_t bInCollection;
} usb_host_video_ctrl_header_desc_t;

/*! @brief video control interface iutput terminal descriptor structure */
typedef struct _usb_host_video_ctrl_it_desc
{
    /*< Total size of the video control input terminal descriptor */
    uint8_t bLength;
    /*< Descriptor type of video control input terminal descriptor */
    uint8_t bDescriptorType;
    /*< Subtype of video control input terminal descriptor */
    uint8_t bDescriptorSubtype;
    /*< A non-zero constant that uniquely identifies the Terminal within the video function */
    uint8_t bTerminalID;
    /*< Constant that characterizes the type of Terminal */
    uint8_t wTerminalType[2];
    /*< ID of the Output Terminal to which this Input Terminal is associated, or zero (0) if no such association exists
     */
    uint8_t bAssocTerminal;
    /*< Index of a string descriptor, describing the Input Terminal */
    uint8_t iTerminal;
} usb_host_video_ctrl_it_desc_t;

/*! @brief video control interface output terminal descriptor structure */
typedef struct _usb_host_video_ctrl_ot_desc
{
    /*< Total size of the video control output terminal descriptor */
    uint8_t bLength;
    /*< Descriptor type of video control output terminal descriptor */
    uint8_t bDescriptorType;
    /*< Subtype of video control output terminal descriptor */
    uint8_t bDescriptorSubtype;
    /*< A non-zero constant that uniquely identifies the Terminal within the video function */
    uint8_t bTerminalID;
    /*< Constant that characterizes the type of Terminal */
    uint8_t wTerminalType[2];
    /*< Constant, identifying the Input Terminal to which this Output Terminal is associated, or zero (0) if no such
     * association exists. */
    uint8_t bAssocTerminal;
    /*< ID of the Unit or Terminal to which this Terminal is connected */
    uint8_t bSourceID;
    /*< Index of a string descriptor, describing the Output Terminal. */
    uint8_t iTerminal;
} usb_host_video_ctrl_ot_desc_t;

/*! @brief video control interface camera terminal descriptor structure */
typedef struct _usb_host_video_ctrl_ct_desc
{
    /*< Total size of the video control camera terminal descriptor */
    uint8_t bLength;
    /*< Descriptor type of video control camera terminal descriptor */
    uint8_t bDescriptorType;
    /*< Subtype of video control camera terminal descriptor */
    uint8_t bDescriptorSubtype;
    /*< A non-zero constant that uniquely identifies the Terminal within the video function */
    uint8_t bTerminalID;
    /*< Constant that characterizes the type of Terminal */
    uint8_t wTerminalType[2];
    /*< ID of the Output Terminal to which this Input Terminal is associated */
    uint8_t bAssocTerminal;
    /*< Index of a string descriptor that describes the Camera Terminal */
    uint8_t iTerminal;
    /*< The value of Lmim If Optical Zoom is not supported; this field shall be set to 0 */
    uint8_t wObjectiveFocalLengthMin[2];
    /*< The value of Lmax If Optical Zoom is not supported; this field shall be set to 0 */
    uint8_t wObjectiveFocalLengthMax[2];
    /*< The value of Locular If Optical Zoom is not supported; this field shall be set to 0 */
    uint8_t wOcularFocalLength[2];
    /*< Size in bytes of the bmControls field:3 */
    uint8_t bControlSize;
    /*< A bit set to 1 indicates that the mentioned Control is supported for the video stream */
    uint8_t bmControls[2];
} usb_host_video_ctrl_ct_desc_t;

/*! @brief video control interface selcetor unit descriptor structure */
typedef struct _usb_host_video_ctrl_su_desc
{
    /*< Total size of the video control selector unit descriptor */
    uint8_t bLength;
    /*< Descriptor type of video control selector unit descriptor */
    uint8_t bDescriptorType;
    /*< Subtype of the video control selector unit descriptor */
    uint8_t bDescriptorSubtype;
    /*< A non-zero constant that uniquely identifies the Unit within the video function */
    uint8_t bUnitID;
    /*< Number of Input Pins of this Unit: p */
    uint8_t bNrInPins;
} usb_host_video_ctrl_su_desc_t;

/*! @brief video control interface processing unit descriptor structure */
typedef struct _usb_host_video_ctrl_pu_desc
{
    /*< Total size of the video control processing unit descriptor */
    uint8_t bLength;
    /*< Descriptor type of video control processing unit descriptor */
    uint8_t bDescriptorType;
    /*< Subtype of the video control processing unit descriptor */
    uint8_t bDescriptorSubtype;
    /*< A non-zero constant that uniquely identifies the Unit within the video function */
    uint8_t bUnitID;
    /*< ID of the Unit or Terminal to which this Unit is connected */
    uint8_t bSourceID;
    /*< If the Digital Multiplier control is supported, this field indicates the maximum digital magnification,
     * multiplied by 100 */
    uint8_t wMaxMultiplier[2];
    /*< Size of the bmControls field, in bytes:3 */
    uint8_t bControlSize;

} usb_host_video_ctrl_pu_desc_t;

/*! @brief video stream interface input header descriptor structure */
typedef struct _usb_host_video_stream_input_header_desc
{
    /*< Total size of the video stream input header descriptor */
    uint8_t bLength;
    /*< Descriptor type of video stream input header descriptor */
    uint8_t bDescriptorType;
    /*< Subtype of video stream input header descriptor */
    uint8_t bDescriptorSubtype;
    /*< Number of video payload Format descriptors following for this interface */
    uint8_t bNumFormats;
    /*< Total number of bytes returned for the class-specific VideoStreaming interface descriptors including this header
     * descriptor */
    uint8_t wTotalLength[2];
    /*< The address of the isochronous or bulk endpoint used for video data */
    uint8_t bEndpointAddress;
    /*< Indicates the capabilities of this VideoStreaming interface */
    uint8_t bmInfo;
    /*< The terminal ID of the Output Terminal to which the video endpoint of this interface is connected */
    uint8_t bTerminalLink;
    /*< Method of still image capture supported as described in section 2.4.2.4 in UVC 1.5 class Spec */
    uint8_t bStillCaptureMethod;
    /*< Specifies if hardware triggering is supported through this interface */
    uint8_t bTriggerSupport;
    /*< Specifies how the host software shall respond to a hardware trigger interrupt event from this interface */
    uint8_t bTriggerUsage;
    /*< Size of each bmaControls(x) field, in bytes:n */
    uint8_t bControlSize;
} usb_host_video_stream_input_header_desc_t;

/*! @brief video stream interface output header descriptor structure */
typedef struct _usb_host_video_stream_output_header_desc
{
    /*< Total size of the video stream output header descriptor */
    uint8_t bLength;
    /*< Descriptor type of video stream output header descriptor */
    uint8_t bDescriptorType;
    /*< Subtype of video stream output header descriptor */
    uint8_t bDescriptorSubtype;
    /*< Number of video payload Format descriptors following for this interface */
    uint8_t bNumFormats;
    /*< Total number of bytes returned for the class-specific VideoStreaming interface descriptors including this header
     * descriptor */
    uint8_t wTotalLength[2];
    /*< The address of the isochronous or bulk endpoint used for video data */
    uint8_t bEndpointAddress;
    /*< The terminal ID of the Input Terminal to which the video endpoint of this interface is connected */
    uint8_t bTerminalLink;
    /*< Size of each bmaControls(x) field, in bytes:n */
    uint8_t bControlSize;
} usb_host_video_stream_output_header_desc_t;

/*! @brief video stream interface Motion-JPEG format descriptor structure */
typedef struct _usb_host_video_stream_payload_mjpeg_format_desc
{
    /*< Total size of the video stream MJPEG format descriptor */
    uint8_t bLength;
    /*< Descriptor type of video stream MJPEG format descriptor */
    uint8_t bDescriptorType;
    /*< Subtype of video stream MJPEG format descriptor */
    uint8_t bDescriptorSubtype;
    /*< Index of this Format Descriptor */
    uint8_t bFormatIndex;
    /*< Number of Frame Descriptors following that correspond to this format */
    uint8_t bNumFrameDescriptors;
    /*< Specifies characteristics of this format */
    uint8_t bmFlags;
    /*< Optimum Frame Index (used to select resolution) for this stream */
    uint8_t bDefaultFrameIndex;
    /*< The X dimension of the picture aspect ratio */
    uint8_t bAspectRatioX;
    /*< The Y dimension of the picture aspect ratio */
    uint8_t bAspectRatioY;
    /*< Specifies interlace information */
    uint8_t bmInterlaceFlags;
    /*< Specifies if duplication of the video stream should be restricted (0: No restrictions, 1: Restrict duplication)
     */
    uint8_t bCopyProtect;
} usb_host_video_stream_payload_mjpeg_format_desc_t;

/*! @brief video stream interface uncompressed format descriptor structure */
typedef struct _usb_host_video_stream_payload_uncompressed_format_desc
{
    /*< Total size of the video stream uncompressed format descriptor */
    uint8_t bLength;
    /*< Descriptor type of video stream uncompressed format descriptor */
    uint8_t bDescriptorType;
    /*< Subtype of video stream uncompressed format descriptor */
    uint8_t bDescriptorSubtype;
    /*< Index of this Format Descriptor */
    uint8_t bFormatIndex;
    /*< Number of Frame Descriptors following that correspond to this format */
    uint8_t bNumFrameDescriptors;
    /*< Globally Unique Identifier used to identify stream-encoding format */
    uint8_t guidFormat[16];
    /*< Number of bits per pixel used to specify color in the decoded video frame */
    uint8_t bBitsPerPixel;
    /*< Optimum Frame Index (used to select resolution) for this stream */
    uint8_t bDefaultFrameIndex;
    /*< The X dimension of the picture aspect ratio */
    uint8_t bAspectRatioX;
    /*< The Y dimension of the picture aspect ratio */
    uint8_t bAspectRatioY;
    /*< Specifies interlace information */
    uint8_t bmInterlaceFlags;
    /*< Specifies if duplication of the video stream should be restricted (0:No restrictions,1:Restrict duplication)*/
    uint8_t bCopyProtect;
} usb_host_video_stream_payload_uncompressed_format_desc_t;

/*! @brief video stream interface Motion-JPEG frame descriptor structure */
typedef struct _usb_host_video_stream_payload_mjpeg_frame_desc
{
    /*< Total size of the video stream MJPEG frame descriptor */
    uint8_t bLength;
    /*< Descriptor type of video stream MJPEG frame descriptor */
    uint8_t bDescriptorType;
    /*< Subtype of video stream MJPEG frame descriptor */
    uint8_t bDescriptorSubtype;
    /*< Index of this Frame Descriptor */
    uint8_t bFrameIndex;
    /*< D0 specifies whether still images are supported at this frame setting. D1 specifies whether the device provides
     * a fixed frame rate on a stream associated with this frame descriptor */
    uint8_t bmCapabilities;
    /*< Width of decoded bitmap frame in pixels */
    uint8_t wWitd[2];
    /*< Height of decoded bitmap frame in pixels */
    uint8_t wHeight[2];
    /*< Specifies the minimum bit rate at default compression quality and longest frame interval in Units of bps at
     * which the data can be transmitted */
    uint8_t dwMinBitRate[4];
    /*< Specifies the maximum bit rate at default compression quality and shortest frame interval in Units of bps at
     * which the data can be transmitted */
    uint8_t dwMaxBitRate[4];
    /*< Specifies the maximum number of bytes for a video (or still image) frame the compressor will produce */
    uint8_t dwMaxVideoFrameBufferSize[4];
    /*< Specifies the frame interval the device would like to indicate for use as a default. This must be a valid frame
     * interval */
    uint8_t dwDefaultFrameInterval[4];
    /*< Indicates how the frame interval can be programmed */
    uint8_t bFrameIntervalType;
    /*< Shortest frame interval supported (at highest frame rate), in 100ns units */
    uint8_t dwMinFrameInterval[4];
} usb_host_video_stream_payload_mjpeg_frame_desc_t;

/*! @brief video stream interface uncompressed frame descriptor structure */
typedef struct _usb_host_video_stream_payload_uncompressed_frame_desc
{
    /*< Total size of the video stream uncompressed frame descriptor */
    uint8_t bLength;
    /*< Descriptor type of video stream uncompressed frame descriptor */
    uint8_t bDescriptorType;
    /*< Subtype of video stream uncompressed frame descriptor */
    uint8_t bDescriptorSubtype;
    /*< Index of this Frame Descriptor */
    uint8_t bFrameIndex;
    /*< D0 specifies whether still images are supported at this frame setting. D1 specifies whether the device provides
     * a fixed frame rate on a stream associated with this frame descriptor */
    uint8_t bmCapabilities;
    /*< Width of decoded bitmap frame in pixels */
    uint8_t wWitd[2];
    /*< Height of decoded bitmap frame in pixels */
    uint8_t wHeight[2];
    /*< Specifies the minimum bit rate at default compression quality and longest frame interval in Units of bps at
     * which the data can be transmitted */
    uint8_t dwMinBitRate[4];
    /*< Specifies the maximum bit rate at default compression quality and shortest frame interval in Units of bps at
     * which the data can be transmitted */
    uint8_t dwMaxBitRate[4];
    /*< Specifies the maximum number of bytes for a video (or still image) frame the compressor will produce */
    uint8_t dwMaxVideoFrameBufferSize[4];
    /*< Specifies the frame interval the device would like to indicate for use as a default. This must be a valid frame
     * interval */
    uint8_t dwDefaultFrameInterval[4];
    /*< Indicates how the frame interval can be programmed */
    uint8_t bFrameIntervalType;
    /*< Shortest frame interval supported (at highest frame rate), in 100ns units */
    uint8_t dwMinFrameInterval[4];
} usb_host_video_stream_payload_uncompressed_frame_desc_t;

/*! @brief video stream interface probe and commit controls descriptor structure */
typedef struct _usb_host_video_probe_commit_controls
{
    /*< Bitfield control indicating to the function what fields shall be kept fixed */
    uint16_t bmHint;
    /*< Video format index from a Format descriptor for this video interface */
    uint8_t bFormatIndex;
    /*< Video frame index from a Frame descriptor */
    uint8_t bFrameIndex;
    /*< Frame interval in 100 ns units */
    uint8_t dwFrameInterval[4];
    /*< Key frame rate in key-frame per video-frame units */
    uint8_t wKeyFrameRate[2];
    /*< PFrame rate in PFrame/key frame units */
    uint8_t wPFrameRate[2];
    /*< Compression quality control in abstract units 1 (lowest) to 10000 (highest) */
    uint8_t wCompQuality[2];
    /*< Window size for average bit rate control */
    uint8_t wCompWindowSize[2];
    /*< Internal video streaming interface latency in ms from video data capture to presentation on the USB */
    uint8_t wDelay[2];
    /*< Maximum video frame or codec-specific segment size in bytes */
    uint8_t dwMaxVideoFrameSize[4];
    /*< Specifies the maximum number of bytes that the device can transmit or receive in a single payload transfer. This
     * field must be supported */
    uint8_t dwMaxPayloadTransferSize[4];
    /*< The device clock frequency in Hz for the specified format */
    uint8_t dwClockFrequency[4];
    /*< Bitfield control */
    uint8_t bmFramingInfo;
    /*< The preferred payload format version supported by the host or device for the specified bFormatIndex value */
    uint8_t bPreferedVersion;
    /*< The minimum payload format version supported by the device for the specified bFormatIndex value */
    uint8_t bMinVersion;
    /*< The maximum payload format version supported by the device for the specified bFormatIndex value */
    uint8_t bMaxVersion;
} usb_host_video_probe_commit_controls_t;

/*! @brief video stream interface paylaod header descriptor structure */
typedef struct _usb_host_video_payload_header
{
    /*< Length of the payload header in bytes including this field */
    uint8_t bHeaderLength;
    /*< Provides information on the sample data following the header, as well as the availability of optional header
     * fields in this header */
    union {
        uint8_t bmHeaderInfo;
        struct
        {
            uint8_t frame_id : 1;
            uint8_t end_of_frame : 1;
            uint8_t presentation_time : 1;
            uint8_t source_clock : 1;
            uint8_t reserved : 1;
            uint8_t still_image : 1;
            uint8_t error : 1;
            uint8_t end_of_header : 1;
        } bitMap;
    } HeaderInfo;
    /*< Presentation Time Stamp (PTS) */
    uint8_t dwPresentationTime[4];
    /*< The device clock frequency in Hz */
    uint8_t dwClockFrequency[4];
    /*< A two-part Source Clock Reference (SCR) value */
    uint8_t scrSourceClock[2];
} usb_host_video_payload_header_t;

/*! usb video class Common Descriptor */
typedef struct _usb_host_video_common_desc
{
    /*< Total size of the usb video common descriptor */
    uint8_t bLength;
    /*< Descriptor type of usb video common descriptor */
    uint8_t bDescriptorType;
    /*< Subtype of video usb video common descriptor */
    uint8_t bDescriptorSubtype;
} usb_host_video_common_desc_t;

/*! @brief video stream interface format descriptor structure common*/
typedef struct _usb_host_video_stream_payload_format_common_desc
{
    /*< Total size of the video stream format descriptor */
    uint8_t bLength;
    /*< Descriptor type of the video stream format descriptor */
    uint8_t bDescriptorType;
    /*< Subtype of the video stream format descriptor */
    uint8_t bDescriptorSubtype;
    /*< Index of this format Descriptor */
    uint8_t bFormatIndex;
    /*< Number of Frame Descriptors following that correspond to this format */
    uint8_t bNumFrameDescriptors;
} usb_host_video_stream_payload_format_common_desc_t;

/*! @brief video stream common frame descriptor structure*/
typedef struct _usb_host_video_stream_payload_frame_common_desc
{
    /*< Total size of the video stream frame descriptor */
    uint8_t bLength;
    /*< Descriptor type of the video stream frame descriptor */
    uint8_t bDescriptorType;
    /*< Subtype of the video stream frame descriptor */
    uint8_t bDescriptorSubtype;
    /*< Index of this Frame Descriptor */
    uint8_t bFrameIndex;
    /*< D0 specifies whether still images are supported at this frame setting. D1 specifies whether the device provides
    * a fixed frame rate on a stream associated with this frame descriptor */
    uint8_t bmCapabilities;
    /*< Width of decoded bitmap frame in pixels */
    uint8_t wWitd[2];
    /*< Height of decoded bitmap frame in pixels */
    uint8_t wHeight[2];
} usb_host_video_stream_payload_frame_common_desc_t;

/*! @brief video descriptor uinon */
typedef union _usb_host_video_descriptor_union {
    /*< common word */
    uint32_t word;
    /*< common buffer pointer */
    uint8_t *bufr;
    /*< usb common descriptor */
    usb_descriptor_common_t *common;
    /*< usb video common descriptor */
    usb_host_video_common_desc_t *video_common;
    /*< usb interface common descriptor */
    usb_descriptor_interface_t *interface;
    /*< usb endpoint common descriptor */
    usb_descriptor_endpoint_t *endpoint;
    /*< usb video mjpeg frame common descriptor */
    usb_host_video_stream_payload_frame_common_desc_t *video_frame_common;
} usb_host_video_descriptor_union_t;

/*! @brief Video instance structure, Video usb_host_class_handle pointer to this structure */
typedef struct _usb_host_video_instance_struct
{
    usb_host_handle hostHandle;                          /*!< The handle of the USB host*/
    usb_device_handle deviceHandle;                      /*!< The handle of the USB device structure */
    usb_host_interface_handle streamIntfHandle;          /*!< This instance's video stream interface handle */
    usb_host_interface_handle controlIntfHandle;         /*!< This instance's control stream interface handle */
    usb_host_pipe_handle controlPipe;                    /*!< Video class control pipe */
    usb_host_pipe_handle interruptPipe;                  /*!< Video class interrupt pipe */
    usb_host_pipe_handle streamIsoInPipe;                /*!< Video class stream iso in pipe */
    usb_host_video_ctrl_header_desc_t *vcHeaderDesc;     /*!< Video class control header descriptor pointer */
    usb_host_video_ctrl_it_desc_t *vcInputTerminalDesc;  /*!< Video class control input terminal descriptor pointer */
    usb_host_video_ctrl_ot_desc_t *vcOutputTerminalDesc; /*!< Video class control output terminal descriptor pointer */
    usb_host_video_ctrl_pu_desc_t *vcProcessingUnitDesc; /*!< Video class control processing unit descriptor pointer */
    usb_host_video_stream_input_header_desc_t
        *vsInputHeaderDesc;                    /*!< Video class stream input header descriptor pointer */
    transfer_callback_t controlCallbackFn;     /*!< Video control transfer callback function */
    void *controlCallbackParam;                /*!< Video control transfer callback parameter */
    usb_host_transfer_t *controlTransfer;      /*!< On-going control transfer */
    transfer_callback_t streamIsoInCallbackFn; /*!< Video stream ISO in transfer callback function */
    void *streamIsoInCallbackParam;            /*!< Video stream ISO in transfer callback parameter */
    uint16_t interruptInPacketSize;            /*!< Video Interrupt in maximum packet size */
    uint16_t isoInPacketSize;                  /*!< Video ISO in maximum packet size */
    uint8_t interruptInEpNum;                  /*!< Video control interrupt in endpoint number */
    uint8_t isoInEpNum;                        /*!< Video stream ISO in endpoint number */

} usb_host_video_instance_struct_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief set video class stream interface.
 *
 * This function bind the interface with the video instance.
 *
 * @param classHandle       The class handle.
 * @param interfaceHandle   The interface handle.
 * @param alternateSetting  The alternate setting value.
 * @param callbackFn        This callback is called after this function completes.
 * @param callbackParam     The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          send transfer fail, please reference to USB_HostSendSetup.
 * @retval kStatus_USB_Busy           callback return status, there is no idle pipe.
 * @retval kStatus_USB_TransferStall  callback return status, the transfer is stall by device.
 * @retval kStatus_USB_Error          callback return status, open pipe fail, please reference to USB_HostOpenPipe.
 */
usb_status_t USB_HostVideoStreamSetInterface(usb_host_class_handle classHandle,
                                             usb_host_interface_handle interfaceHandle,
                                             uint8_t alternateSetting,
                                             transfer_callback_t callbackFn,
                                             void *callbackParam);

/*!
 * @brief set control interface.
 *
 * This function bind the control interface with the video instance.
 *
 * @param classHandle      the class handle.
 * @param interfaceHandle  the control interface handle.
 * @param alternateSetting the alternate setting value.
 * @param callbackFn       this callback is called after this function completes.
 * @param callbackParam    the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          send transfer fail, please reference to USB_HostSendSetup.
 * @retval kStatus_USB_Busy           callback return status, there is no idle pipe.
 * @retval kStatus_USB_TransferStall  callback return status, the transfer is stall by device.
 * @retval kStatus_USB_Error          callback return status, open pipe fail, please reference to USB_HostOpenPipe.
 */
usb_status_t USB_HostVideoControlSetInterface(usb_host_class_handle classHandle,
                                              usb_host_interface_handle interfaceHandle,
                                              uint8_t alternateSetting,
                                              transfer_callback_t callbackFn,
                                              void *callbackParam);

/*!
 * @brief video stream receive data.
 *
 * This function implements video receiving data.
 *
 * @param classHandle    The class handle.
 * @param buffer         The buffer pointer.
 * @param bufferLen      The buffer length.
 * @param callbackFn     This callback is called after this function completes.
 * @param callbackParam  The first parameter in the callback function.
 *
 * @retval kStatus_USB_Success        Receive request successfully.
 * @retval kStatus_USB_InvalidHandle  The classHandle is NULL pointer.
 * @retval kStatus_USB_Busy           There is no idle transfer.
 * @retval kStatus_USB_Error          pipe is not initialized.
 *                                    Or, send transfer fail, please reference to USB_HostRecv.
 */
usb_status_t USB_HosVideoStreamRecv(usb_host_class_handle classHandle,
                                    uint8_t *buffer,
                                    uint32_t bufferLen,
                                    transfer_callback_t callbackFn,
                                    void *callbackParam);

/*!
 * @brief initialize the video instance.
 *
 * This function allocate the resource for video instance.
 *
 * @param deviceHandle   the device handle.
 * @param classHandle    return class handle.
 *
 * @retval kStatus_USB_Success        The device is initialized successfully.
 * @retval kStatus_USB_AllocFail      Allocate memory fail.
 */
usb_status_t USB_HostVideoInit(usb_device_handle deviceHandle, usb_host_class_handle *classHandle);

/*!
 * @brief de-initialize the video instance.
 *
 * This function release the resource for video instance.
 *
 * @param deviceHandle   the device handle.
 * @param classHandle    the class handle.
 *
 * @retval kStatus_USB_Success    The device is de-initialized successfully.
 */
usb_status_t USB_HostVideoDeinit(usb_device_handle deviceHandle, usb_host_class_handle classHandle);

/*!
 * @brief get video stream format descriptor.
 *
 * This function implements get video stream format descriptor.
 *
 * @param classHandle   The class handle.
 * @param subType       The descriptor subtype.
 * @param descriptor    The pointer of specific format descriptor.
 *
 * @retval kStatus_USB_Success            Get video stream format descriptor request successfully.
 * @retval kStatus_USB_InvalidHandle     The classHandle is NULL pointer.
 * @retval kStatus_USB_InvalidParameter  The descriptor is NULL pointer.
 *
 */
usb_status_t USB_HostVideoStreamGetFormatDescriptor(usb_host_class_handle classHandle,
                                                    uint8_t subType,
                                                    void **descriptor);

/*!
 * @brief get specific video stream frame descriptor.
 *
 * This function implements get specific video stream frame descriptor.
 *
 * @param classHandle        The class handle.
 * @param formatDescriptor   The frame descriptor pointer.
 * @param index              The specific frame descriptor id
 * @param descriptor         The pointer of specific frame descriptor.
 *
 * @retval kStatus_USB_Success           Get video stream frame descriptor request successfully.
 * @retval kStatus_USB_InvalidHandle     The classHandle is NULL pointer.
 * @retval kStatus_USB_InvalidParameter  The descriptor is NULL pointer.
 *
 */
usb_status_t USB_HostVideoStreamGetFrameDescriptor(
    usb_host_class_handle classHandle, void *formatDescriptor, uint8_t subType, uint8_t frameIndex, void **descriptor);

/*!
 * @brief video set probe.
 *
 * This function implements the Video class-specific request (set probe).
 *
 * @param classHandle    the class handle.
 * @param request        setup packet request value.
 * @param probe          video probe data
 * @param callbackFn     this callback is called after this function completes.
 * @param callbackParam  the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success           Request successful.
 * @retval kStatus_USB_InvalidHandle     The classHandle is NULL pointer.
 * @retval kStatus_USB_InvalidParameter  The interface descriptor is NULL pointer.
 */
usb_status_t USB_HostVideoSetProbe(usb_host_class_handle classHandle,
                                   uint8_t request,
                                   uint8_t *probe,
                                   transfer_callback_t callbackFn,
                                   void *callbackParam);

/*!
 * @brief video get probe.
 *
 * This function implements the Video class-specific request (get probe).
 *
 * @param classHandle   the class handle.
 * @param request       setup packet request value.
 * @param probe         video probe data
 * @param callbackFn    this callback is called after this function completes.
 * @param callbackParam the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success           Request successful.
 * @retval kStatus_USB_InvalidHandle     The classHandle is NULL pointer.
 * @retval kStatus_USB_InvalidParameter  The interface descriptor is NULL pointer.
 */
usb_status_t USB_HostVideoGetProbe(usb_host_class_handle classHandle,
                                   uint8_t request,
                                   uint8_t *probe,
                                   transfer_callback_t callbackFn,
                                   void *callbackParam);

/*!
 * @brief video get commit.
 *
 * This function implements the Video class-specific request (get commit).
 *
 * @param classHandle    the class handle.
 * @param request        setup packet request value.
 * @param probe          video probe data
 * @param callbackFn     this callback is called after this function completes.
 * @param callbackParam  the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success           Request successful.
 * @retval kStatus_USB_InvalidHandle     The classHandle is NULL pointer.
 * @retval kStatus_USB_InvalidParameter  The interface descriptor is NULL pointer.
 * @retval kStatus_USB_InvalidRequest    The request is invaild.
 */
usb_status_t USB_HostVideoGetCommit(usb_host_class_handle classHandle,
                                    uint8_t brequest,
                                    uint8_t *probe,
                                    transfer_callback_t callbackFn,
                                    void *callbackParam);

/*!
 * @brief video set commit.
 *
 * This function implements the Video class-specific request (set commit).
 *
 * @param classHandle    the class handle.
 * @param request        setup packet request value.
 * @param probe          video probe data
 * @param callbackFn     this callback is called after this function completes.
 * @param callbackParam  the first parameter in the callback function.
 *
 * @retval kStatus_USB_Success           Request successful.
 * @retval kStatus_USB_InvalidHandle     The classHandle is NULL pointer.
 * @retval kStatus_USB_InvalidParameter  The interface descriptor is NULL pointer.
 */
usb_status_t USB_HostVideoSetCommit(usb_host_class_handle classHandle,
                                    uint8_t brequest,
                                    uint8_t *probe,
                                    transfer_callback_t callbackFn,
                                    void *callbackParam);
/*! @}*/

#ifdef __cplusplus
}
#endif
/*! @}*/

#endif /* __USB_HOST_VIDEO_H__ */
