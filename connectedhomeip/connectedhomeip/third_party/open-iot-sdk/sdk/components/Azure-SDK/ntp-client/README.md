# NTP client

This library allows you to fetch time information from a NTP server.

## Dependencies

NTP client requires others components to build:
 - CMSIS RTOS API
 - CMSIS SOCKET API
 - IoT SDK IP Network API
 - Azure C shared utility

Before building makes sure that these targets are accessible.

## Config

`iotNtpClientConfig_t` structure contains setting parameters:
 - *hostname* - time server name
 - *port* - time server port
 - *interval_ms* - time update interval in miliseconds. The time is updated immediately when the client starts and again every time this interval.

A user can define their own NTP configuration by filling an `iotNtpClientConfig_t` structure and passing it to the initialization function: `iotNtpClientInit()`.

The default configuration values are:
 - *hostname* - pool.ntp.org
 - *port* - 123
- *interval_ms* - 30000

## Usage

Applications must link against `ntp-client` to use NTP client.

## API

`iotNtpClientResult_t iotNtpClientInit(const iotNtpClientConfig_t *config)`

Initialise the NTP client. Set all resources and run NTP thread. You can provide custom NTP client configuration with config argument.

`iotNtpClientResult_t iotNtpClientDeinit()`

Stop the NTP client if run and free all held resources.

`iotNtpClientResult_t iotNtpClientStart()`

Start the NTP client. The time server connection will be established and the time value will be updated with the set interval.

`iotNtpClientResult_t iotNtpClientStop()`

Stop the NTP client. Stop collection data colletion and connection with time server.

`iotNtpClientResult_t iotNtpClientAddListener(iotNtpCb_t callback, void *context)`

Add listener callback to the NTP client. The callback function will be called after time value update.

`iotNtpClientResult_t iotNtpClientClearListeners()`

Remove all NTP client listeners.

`iotNtpClientState_t iotNtpClientGetState()`

Get the current status of the NTP client.

`time_t iotNtpClientGetTime()`

Get the last set time. If the client has not been started or has not had time to contact the server, the value will be 0.

__Example usage:__

```c
static void ntp_listener_callback(time_t time, void *ctx)
{
    printf("Current time: %d", time);
}

int main(void)
{
    iotNtpClientState_t res = iotNtpClientInit(NULL);
    if (res != IOT_NTP_CLIENT_OK) {
        return res;
    }

    res = iotNtpClientAddListener(ntp_listener_callback, NULL);
    if (res != IOT_NTP_CLIENT_OK) {
        return res;
    }

    res = iotNtpClientStart();
    if (res != IOT_NTP_CLIENT_OK) {
        return res;
    }

    sleep(30);

    res = iotNtpClientClearListeners();
    if (res != IOT_NTP_CLIENT_OK) {
        return res;
    }

    res = iotNtpClientDeinit();
    if (res != IOT_NTP_CLIENT_OK) {
        return res;
    }

    return 0;
}
```
