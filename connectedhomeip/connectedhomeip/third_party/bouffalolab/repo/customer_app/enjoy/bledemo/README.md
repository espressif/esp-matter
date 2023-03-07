# BLE Demo user guide

This demo code button pin is IO12. Please add the following setting to corresponding dts file ：this node feature is " button" and mode is " multipress".  this node will trigger three event , customer can set "press time" setting.

 The central device will connect peripheral device when two devices is close to 5 cm to 10 cm ( Rssi < -20 ). 

```c
gpio2 {
    status = "okay";
    pin = <12>;
    feature = "button";
    active = "Hi";
    mode = "multipress";
    button {
        debounce = <10>;
        short_press_ms {
            start = <100>;
            end = <2000>;
            kevent = <2>;
        };
        long_press_ms {
            start = <3000>;
            end = <5000>;
            kevent = <3>;
        };
        longlong_press_ms {
            start = <10000>;
            kevent = <4>;
        };
        trig_level = "Lo";
    };
    hbn_use = "disable";
};
```



## 1. Peripheral Function

Short press button : Start Bluetooth Advertising.  

Longlong press button : Send notification when device state is connected.

## 2. Central Function

Long press button : Start Bluetooth Scanning. 

Longlong press button : Write data without response when device state is connected.



   