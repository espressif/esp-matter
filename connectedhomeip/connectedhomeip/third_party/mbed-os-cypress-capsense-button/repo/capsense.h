/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
 *    Copyright (c) 2019 Google LLC.
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "cy_pdl.h"
#include "cybsp.h"
#include "cycfg.h"
#include "cycfg_capsense.h"
#include "cyhal.h"
#include "events/mbed_events.h"
#include "platform/Callback.h"
#include "rtos/rtos.h"

class Capsense
{
public:
    struct EventHandler
    {
        virtual void on_button_pressed(int button_id) {}
        virtual void on_button_released(int button_id) {}
        virtual void on_slider(int slider_pos) {}

    private:
        friend Capsense;
        struct EventHandler * next;
    };
    void register_event_handler(EventHandler * event_handler)
    {
        if (head == nullptr)
        {
            head = event_handler;
        }
        else
        {
            event_handler->next = head;
            head                = event_handler;
        }
    }

    void init();
    static Capsense & getInstance()
    {
        static Capsense instance;
        return instance;
    }

private:
    Capsense() {}
    ~Capsense() {}
    Capsense(const Capsense &) = delete;
    Capsense & operator=(const Capsense &) = delete;

    void initialize_capsense_tuner();
    static void RunCapSenseScan();
    static void ProcessTouchStatus();
    static void CapSense_InterruptHandler();
    static void CapSenseEndOfScanCallback(cy_stc_active_scan_sns_t * ptrActiveScan);
    EventHandler * head = nullptr;

    void on_button_pressed(int id) { dispatch_call(&EventHandler::on_button_pressed, id); }
    void on_button_released(int id) { dispatch_call(&EventHandler::on_button_released, id); }
    void on_slider(int pos) { dispatch_call(&EventHandler::on_slider, pos); }

    template <typename F, typename... Ts>
    void dispatch_call(F member_function, Ts... args)
    {
        auto node = head;
        while (node)
        {

            (node->*member_function)(args...);
            node = node->next;
        }
    }
};

namespace mbed {
class CapsenseButton : private Capsense::EventHandler
{
public:
    CapsenseButton(Capsense & manager, int button_id) : _manager(manager), _button_id(button_id)
    {
        _manager.register_event_handler(this);
    }

    ~CapsenseButton(){};
    CapsenseButton(const CapsenseButton &) = delete;
    CapsenseButton & operator=(const CapsenseButton &) = delete;
    //  int getStatus();
    void rise(Callback<void()> func);
    void fall(Callback<void()> func);

private:
    void on_button_pressed(int button_id) override
    {
        if (_button_id == button_id)
        {
            if (_fall)
            {
                _fall();
            }
        }
    }
    void on_button_released(int button_id) override
    {
        if (_button_id == button_id)
            if (_rise)
            {
                _rise();
            }
    }

    Callback<void()> _rise;
    Callback<void()> _fall;
    Capsense & _manager;
    int _button_id;
};

class CapsenseSlider : private Capsense::EventHandler
{
public:
    CapsenseSlider(Capsense & manager) : _manager(manager), _slider_pos(0) { _manager.register_event_handler(this); }

    ~CapsenseSlider(){};
    CapsenseSlider(const CapsenseSlider &) = delete;
    CapsenseSlider & operator=(const CapsenseSlider &) = delete;
    int getStatus() { return _slider_pos; };
    void on_move(Callback<void(int)> func);

private:
    void on_slider(int slider_pos) override
    {
        _slider_pos = slider_pos;
        if (_on_move)
        {
            _on_move(slider_pos);
        }
    }

    Callback<void(int)> _on_move;
    Capsense & _manager;
    int _slider_pos;
};
} // namespace mbed