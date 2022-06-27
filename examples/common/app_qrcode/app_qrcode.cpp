/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string>

#include <core/CHIPError.h>
#include <esp_log.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/CommissionableDataProvider.h>
#include <platform/DeviceInstanceInfoProvider.h>
#include <setup_payload/ManualSetupPayloadGenerator.h>
#include <setup_payload/QRCodeSetupPayloadGenerator.h>
#include <setup_payload/SetupPayload.h>

#include <app_qrcode.h>
#include <qrcode.h>

using namespace ::chip;
using namespace ::chip::DeviceLayer;

#define QRCODE_BASE_URL "https://dhrishi.github.io/connectedhomeip/qrcode.html"
static const char *TAG = "app_qrcode";

esp_err_t app_qrcode_get_payload(char **qrcode_text, char **short_manual_code_text, char **long_manual_code_text)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    esp_err_t ret = ESP_OK;
    uint16_t discriminator;
    uint32_t setup_pin_code;
    uint16_t vendor_id;
    uint16_t product_id;
    SetupPayload payload;
    std::string qrcode_payload;
    std::string short_manual_code_payload;
    std::string long_manual_code_payload;

    /* Get details */
    err = GetCommissionableDataProvider()->GetSetupDiscriminator(discriminator);
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Couldn't get discriminator: %s", ErrorStr(err));
        return ESP_FAIL;
    }

    err = GetCommissionableDataProvider()->GetSetupPasscode(setup_pin_code);
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Couldn't get setup_pin_code: %s", ErrorStr(err));
        return ESP_FAIL;
    }

    err = GetDeviceInstanceInfoProvider()->GetVendorId(vendor_id);
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Couldn't get vendor_id: %s", ErrorStr(err));
        return ESP_FAIL;
    }

    err = GetDeviceInstanceInfoProvider()->GetProductId(product_id);
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Couldn't get product_id: %s", ErrorStr(err));
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Setup discriminator: %u (0x%x)", discriminator, discriminator);
    ESP_LOGI(TAG, "Setup PIN code: %u (0x%x)", setup_pin_code, setup_pin_code);
    ESP_LOGI(TAG, "Vendor ID: %u (0x%x)", vendor_id, vendor_id);
    ESP_LOGI(TAG, "Product ID: %u (0x%x)", product_id, product_id);

    /* Set details */
    payload.version = 0;
    payload.discriminator = discriminator;
    payload.setUpPINCode = setup_pin_code;
    payload.rendezvousInformation = RendezvousInformationFlags(chip::RendezvousInformationFlag::kBLE);
    payload.vendorID = vendor_id;
    payload.productID = product_id;

    /* Change format and alloc for qrcode */
    QRCodeSetupPayloadGenerator qrcode_generator(payload);
    err = qrcode_generator.payloadBase38Representation(qrcode_payload);
    if (err == CHIP_NO_ERROR) {
        *qrcode_text = (char *)calloc(1, qrcode_payload.length() + 1);
        if (*qrcode_text) {
            strcpy(*qrcode_text, qrcode_payload.c_str());
        }
    } else {
        ESP_LOGE(TAG, "Couldn't get qrcode payload string %s", ErrorStr(err));
        ret = ESP_FAIL;
    }

    /* Change format and alloc for short manual code. This can be used for 'non qrcode' devices. */
    ManualSetupPayloadGenerator short_manual_code_generator(payload);
    err = short_manual_code_generator.payloadDecimalStringRepresentation(short_manual_code_payload);
    if (err == CHIP_NO_ERROR) {
        *short_manual_code_text = (char *)calloc(1, short_manual_code_payload.length() + 1);
        if (*short_manual_code_text) {
            strcpy(*short_manual_code_text, short_manual_code_payload.c_str());
        }
    } else {
        ESP_LOGE(TAG, "Couldn't get short manual code payload string %s", ErrorStr(err));
        ret = ESP_FAIL;
    }

    /* Change format and alloc for long manual code. This can be used for 'non qrcode' devices. */
    /* Just adding this extra commissioning flow to the payload. */
    payload.commissioningFlow = CommissioningFlow::kCustom;
    ManualSetupPayloadGenerator long_manual_code_generator(payload);
    err = long_manual_code_generator.payloadDecimalStringRepresentation(long_manual_code_payload);
    if (err == CHIP_NO_ERROR) {
        *long_manual_code_text = (char *)calloc(1, long_manual_code_payload.length() + 1);
        if (*long_manual_code_text) {
            strcpy(*long_manual_code_text, long_manual_code_payload.c_str());
        }
    } else {
        ESP_LOGE(TAG, "Couldn't get long manual code payload string %s", ErrorStr(err));
        ret = ESP_FAIL;
    }

    return ret;
}

esp_err_t app_qrcode_print()
{
    /* Get */
    char *qrcode_text = NULL;
    char *short_manual_code_text = NULL;
    char *long_manual_code_text = NULL;
    bool manual_code_exists = false;
    esp_err_t err = app_qrcode_get_payload(&qrcode_text, &short_manual_code_text, &long_manual_code_text);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error getting payload %d", err);
    }

    /* Manual code */
    if (short_manual_code_text || long_manual_code_text) {
        /* This flag is just used for the print at the end */
        manual_code_exists = true;
    }

    /* Short manual code */
    if (short_manual_code_text) {
        /* Print */
        ESP_LOGI(TAG, "Short manual code: %s", short_manual_code_text);

        /* Free */
        free(short_manual_code_text);
    } else {
        ESP_LOGE(TAG, "Error getting short manual code text");
        err = ESP_FAIL;
    }

    /* Long manual code */
    if (long_manual_code_text) {
        /* Print */
        ESP_LOGI(TAG, "Long manual code: %s", long_manual_code_text);

        /* Free */
        free(long_manual_code_text);
    } else {
        ESP_LOGE(TAG, "Error getting long manual code text");
        err = ESP_FAIL;
    }

    /* QR code */
    if (qrcode_text) {
        /* Print */
        ESP_LOGI(TAG, "Scan this QR code from the Matter phone app for Commissioning.");
        esp_qrcode_config_t cfg = ESP_QRCODE_CONFIG_DEFAULT();
        /* Changing the error tolerance to MED. This increases the size of the qr code and makes it more detailed. The
         * default is set to LOW and sometimes the qr code scanner is not able to recognize that. */
        cfg.qrcode_ecc_level = ESP_QRCODE_ECC_MED;
        esp_qrcode_generate(&cfg, qrcode_text);
        ESP_LOGI(TAG, "If QR code is not visible, copy paste the URL in a browser: %s?data=%s", QRCODE_BASE_URL,
                 qrcode_text);

        /* Free */
        free(qrcode_text);
    } else {
        ESP_LOGE(TAG, "Error getting qrcode text");
        err = ESP_FAIL;
    }

    if (manual_code_exists) {
        ESP_LOGI(TAG,
                 "In case QR code is not supported, manual code mentioned above can be entered in the Matter phone app "
                 "for Commissioning");
    }

    return err;
}
