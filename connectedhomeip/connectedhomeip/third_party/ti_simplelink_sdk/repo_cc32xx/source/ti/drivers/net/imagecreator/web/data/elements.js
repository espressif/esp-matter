var pages={
    'page': [
    {   'id':'advanced',
        'title':'Advanced',
        'load':'localStorage.setItem(\'project\', "");pressAdvanced();',
        'submit':'alert ("submit"); return false;',
        'connect':true,
        'objects':[]
    },
    //General
    {   'id':'general',
        'title':'General',
        'load':'localStorage.setItem(\'project\', "");loadGeneralSum();',
        'submit':'alert ("submit"); return false;',
        'connect':true,
        'objects':[
                    {'type':'fieldset',
                        'id':'fieldsetGeneralSum',
                        'title':'General Summary',
                        'class': 'wrapSection',
                        'objects':[
                                       {
                                           'type':'div',
                                           'title':'Name',
                                           'class':'wrapDRS floatLeft'                                                         
                                       },
                                       {
                                           'type':'div',
                                           'id':'Name_SUM',
                                           'class':'wrapDRS floatRight'                                                         
                                       },
                                
                                       {
                                           'type':'div',
                                           'class': 'clearM'
                                       },
                                
                                       {
                                           'type':'div',
                                           'title':'Mode',
                                           'class':'wrapDRS floatLeft'                                                         
                                       },
                                       {
                                           'type':'div',
                                           'id':'Mode_SUM',
                                           'class':'wrapDRS floatRight'                                                         
                                       },
                                
                                       {
                                           'type':'div',
                                           'class': 'clearM'
                                       },
                                       {
                                           'type':'div',
                                           'title':'Device Type',
                                           'class':'wrapDRS floatLeft'                                                         
                                       },
                                       {
                                           'type':'div',
                                           'id':'DeviceType_SUM',
                                           'class':'wrapDRS floatRight'                                                         
                                       },
                                
                                       {
                                           'type':'div',
                                           'class': 'clearM'
                                       },
                                       {
                                           'type':'div',
                                           'title':'Capacity',
                                           'class':'wrapDRS floatLeft'                                                         
                                       },
                                       {
                                           'type':'div',
                                           'id':'Capacity_SUM',
                                           'class':'wrapDRS floatRight'                                                         
                                       },
                                
                                       {
                                           'type':'div',
                                           'class': 'clearM'
                                       }
                        ]
                    }
        ]
    },
        //Settings
        {   'id':'generalsettings',
            'title':'General-> Settings',
            'load':'localStorage.setItem(\'project\', "");loadGeneralSettings(); dropFile(\'#keyFileInput\');',
            'submit':'alert ("submit"); return false;',
            'connect':true,
            'objects': [
                    {'type':'fieldset',
                        'id':'fieldsetGeneralSettingsModuleType',
                        'title':'IC / Module',
                        'class': 'wrapSection',
                        'objects': [

                                        {
                                            'id': 'generalSettingsModuleTypeDiv',
                                            'type': 'div',
                                            'class': 'wrapDRS floatLeft',
                                            'objects': [
                                                        {
                                                            'type': 'div',
                                                            'class': 'fieldName',
                                                            'title': ' &nbsp;'
                                                        },
                                                        {
                                                            'id': 'ModuleTypeSelect',
                                                            'class': '',
                                                            'type': 'select',
                                                            'values': [{ 'value': 0, 'text': 'I/C' }, { 'value': 1, 'text': 'Antenna Module' }, { 'value': 2, 'text': 'Non Ant Module' }],
                                                            'change': 'changeModuleSelect();'
                                                        }
                                            ]
                                        }
                                    ]
                        },
                        {'type':'fieldset',
                            'id':'fieldsetGeneralSettingsMode',
                            'title':'Image Mode',
                            'class': 'wrapSection',
                            'objects':[
                                
                                           {'id':'generalSettingsDevelopMacAddressText',
                                               'type':'div',
                                               'class': 'wrapDRS floatRight',
                                               'objects':[
                                                          {
                                                              'type':'div',
                                                              'class': 'fieldName',
                                                              'title':' Original Mac Address'
                                                          },
                                                          {
                                                              'id':'DevMA1',
                                                              'class':'',
                                                              'type':'text',
                                                              'value':'',
                                                              'change':'changeDevelopeMacAddress();'
                                                   
                                                          }]
                                           },
                                           {
                                               'id':'generalSettingsModeDiv',
                                               'type':'div',
                                               'class': 'wrapDRS floatLeft',
                                               'objects':[
                                                           {
                                                               'type':'div',
                                                               'class': 'fieldName',
                                                               'title':' &nbsp;'
                                                           },
                                                           {
                                                               'id':'ModeSelect',
                                                               'class':'',
                                                               'type':'select',
                                                               'values': [{'value':1,'text':'Production'},{'value':2,'text':'Development'}],
                                                               'change':'changeMode();'
                                                           }
                                               ]				
                                           }
                            ]
                        },
                   
                        {'type':'fieldset',
                            'id':'fieldsetGeneralSettingsCapacity',
                            'title':'Capacity',
                            'class': 'wrapSection',
                            'objects':[
                                       {
                                           'id':'generalSettingsCapacitySelectDiv',
                                           'type':'div',
                                           'class': 'wrapDRS floatLeft',
                                           'objects':[
                                                       {
                                                           'type':'div',
                                                           'class': 'fieldName',
                                                           'title':'&nbsp;'
                                                       },
                                                       {
                                                           'id':'capacitySelect',
                                                           'class':'',
                                                           'type':'select',
                                                           'values': [{'value':'1048576','text':'1M BYTE'},{'value':'2097152','text':'2M BYTE'}, {'value':'4194304','text':'4M BYTE'},{'value':'8388608','text':'8M BYTE'},{'value':'16777216','text':'16M BYTE'}],
                                                           'change':'changeCapacity();'
                                                       }
                                           ]				
                                       }/*,
                                {
                                'id':'generalSettingsCapacityBtnDiv',
                                'type':'div',
                                'class': 'wrapDRS floatRight',
                                'objects':[
                                            {
                                                'id':'checkDeviceSize',
                                                'type':'div',
                                                //'class': 'regButton btnDisabled',
                                                'class': 'regButton floatRight btnDisabled',
                                                'tooltip':'Check device size',
                                                //'title':'<i class="fa fa-arrows-h fa-1x"></i> Check device size',
                                                //'title':'&nbsp; Check device size &nbsp; &nbsp; &nbsp; ',
                                                'title':'Check device size',
                                                //'click':'checkDeviceSizeClicked();'
                                            }
                                            ]
                                } */           
                            ]
                        },
                        {'type':'fieldset',
                            'id':'fieldsetGeneralSettingsCapacity3200Security',
                            'title':'',
                            'class': 'wrapSection',
                            'objects':[
                                      /* {
                                       'id':'generalSettingsCapacityDiv',
                                       'type':'div',
                                       'class': 'wrapDRS floatLeft',
                                       'objects':[
                                                   {
                                                       'type':'div',
                                                       'class': 'fieldName',
                                                       'title':'Capacity'
                                                   },
                                                   {
                                                       'id':'capacitySelect',
                                                       'class':'',
                                                       'type':'select',
                                                       'values': [{'value':'1048576','text':'1M BYTE'},{'value':'2097152','text':'2M BYTE'}, {'value':'4194304','text':'4M BYTE'},{'value':'8388608','text':'8M BYTE'},{'value':'16777216','text':'16M BYTE'}],
                                                       'change':'changeCapacity();'
                                                   }
                                                 ]				
                                       },*/          
                                      /* {
                                           'id':'generalSettingsSecureDeviceDiv',
                                           'type':'div',
                                           'class': 'wrapDRS floatLeft',
                                           'objects':[
                                                       {
                                                           'type':'div',
                                                           'class': 'fieldName',
                                                           'title':'Secured Device'
                                                       },
                                                       {
                                                           'id':'3200Select',
                                                           'class':'',
                                                           'type':'select',
                                                           'values': [{'value':'2','text':'True'},{'value':'1','text':'False'}],
                                                           'change':'changeSecureDevice();'
                                                       }
                                                     ]				
                                       },*/
                                                        
                                       {
                                           'id':'generalSettingsFactoryConfigDiv',
                                           'type':'div',
                                           'class': 'wrapDRS floatLeft',
                                           'objects':[
                                                       {
                                                           'type':'div',
                                                           'class': 'fieldName',
                                                           'title':'Restore To Factory Configuration'
                                                       },
                                                       {
                                                           'id':'FACTORY',
                                                           'class':'',
                                                           'type':'select',
                                                           'values': [{'value':'defaults_and_image','text':'Defaults and Image'},{'value':'none','text':'Disable'}, {'value':'defaults_only','text':'Defaults Only'}],
                                                           'change':'changeFactoryConfig();'
                                                       }
                                           ]				
                                       },//Restore To Factory Configuration
                                       {
                                           'id': 'generalSettingsMacAddress',
                                           'type': 'div',
                                           'class': 'wrapDRS floatRight',
                                           'objects': [
                                                       {
                                                           'id': 'USE_DEF_MAC',
                                                           'class': '',
                                                           'type': 'checkbox',
                                                           'label': 'bot 2',
                                                           'values': [{ 'value': 1, 'text': 'Use device MAC Address' }]
                                                       },
                                                       {
                                                           'id': 'generalSettingsMacAddressText',
                                                           'type': 'div',
                                                           'class': 'wrapDRS floatRight',
                                                           'objects': [
                                                                       {
                                                                           'id': 'MA1',
                                                                           'class': '',
                                                                           'type': 'text',
                                                                           'value': '11:22:33:44:55:66'

                                                                       }]
                                                       },
                                           ]
                                       },//Use device MAC Address
                                       {
                                           'type':'div',
                                           'class': 'clearM'
                                       },//Clear
                                       {
                                           'id':'generalSettingsFactorySOPDiv',
                                           'type':'div',
                                           'class': 'wrapDRS floatLeft',
                                           'objects':[
                                                       {
                                                           'type':'div',
                                                           'class': 'fieldName',
                                                           'title':'Restore To Factory Image SOP'
                                                       },
                                                       {
                                                           'id':'IGNORE_FORCE_AP',
                                                           'class':'',
                                                           'type':'select',
                                                           'values': [{'value':0,'text':'Enable'},{'value':1,'text':'Disable'}],
                                                           'change':'changeFactorySOP();'
                                                       }
                                           ]				
                                       },//Restore To Factory Image SOP
                                       {
                                           'id':'generalSettingsProductionDeviceDiv',
                                           'type':'div',
                                           'class': 'wrapDRS floatRight',
                                           'objects':[
                                                       {
                                                           'type':'div',
                                                           'class': 'fieldName',
                                                           //'title':'Production NWP'
                                                           'title':'Target Device'
                                                       },
                                                       {
                                                           'id':'ProductionNWP',
                                                           'class':'',
                                                           'type':'select',
                                                           'values': [{'value':'1','text':'Production Device'},{'value':'0','text':'Preproduction Device'}],
                                                           'change':'changeProductionDevice();'
                                                       }]				
                                       } //Target Device
                            ]
                        },
                        {'type':'fieldset',
                            'id':'fieldsetKeySourceFile',
                            'title':'Key Source File Name',
                            'class': 'wrapSection',
                            'objects':[
                                           {'id':'USE_KEY',
                                               'class':'',
                                               'type':'checkbox',
                                               'label': 'bot 2',
                                               'values': [{'value':1,'text':'Use Encryption Key'}]
                                           },
                                           {
                                               'type':'div',
                                               'class': 'clearM'
                                           },
                                           /*{
                                               'id':'KEY_FILE',
                                               'type':'file',
                                               'change':'postFile(\'KEY_FILE\', \'uploadProjectKSFile\', \'3\');',
                                               'class': ''
                                               //'class': 'regButton'
                                           }*/
                                           {
                                               'id':'fieldsetKeySourceFileBrowse',
                                               'type':'div',
                                               'class': 'wrapDRS floatLeft',
                                               'objects':[
                                    
                                                           /*{
                                                           'id':'keySourceStubb1',
                                                           'type':'div',
                                                           'class': 'fieldName',
                                                           'title':'&nbsp;'
                                                           },*/
                                                           {
                                                               'id':'keyFileInput',
                                                               'type':'text',
                                                               //'class':'floatLeft keyWidth'
                                                               'class':'floatLeft'
                                                           },
                                                           {
                                                               'id':'browseKeyFile',
                                                               'type':'div',
                                                               //'class': 'regButton rbGray mtop0 mLeftS',
                                                               //'class': 'regButton mtop0 mLeftS',
                                                               'class': 'regButtonBr mtop0 mLeftS',
                                                               'title':'Browse',
                                                               //'click':'openKeySourceBrowse();'
                                                               'click':'$(\'#KEY_FILE\').val(""); $(\'#KEY_FILE\').click();'
                                                           },
                                                           {
                                                               'id':'clearEncrKeyFile',
                                                               'type':'div',
                                                               'class': 'regButton mtop0 mLeftS floatRight',
                                                               'title':'&nbspClear&nbsp&nbsp',
                                                               'tooltip':'Clear Encryption Key from project',
                                                               'click':'clearEncrKeyFile()',
                                                           },
                                                           {
                                                               //'id':'browseKeySourceFile',
                                                               'id':'KEY_FILE',
                                                               'type':'file',
                                                               'class':'hideFile',
                                                               'change':'postFile(\'KEY_FILE\', \'uploadProjectKSFile\', \'3\');',
                                                           }/*,
                                                    {
                                                        'type':'div',
                                                        'class': 'clear'
                                                    }*/
                                                           ,{
                                                               'type': 'div',
                                                               'id': 'encrKeyFileNote',
                                                               'text': "justtext",
                                                               'title': "Important: The key file is saved into the project's directory",
                                                               'class': 'uRed'
                                                           }
                                               ]
                                           }
                            ]
                        }/*,
                    
                    {
                        'id':'ConnectBtn',
                        'type':'div',
                        'class': 'regButton',
                        'title':'Connect',
                        'click':'connectDeviceAPI()'
                    }*/
                     
            ]
        },
    //System Settings
    {   'id':'systemsettings',
        'title':'System Settings',
        'load':'localStorage.setItem(\'project\', "");loadDevice();loadDeviceRoleSettingsSum();',
        'submit':'alert ("submit"); return false;',
        'connect':true,
        'objects':[
                     {'type'    :'fieldset',
                         'id'      :'fieldsetSystemSettingsDeviceRadioSettingsSum',
                         'title'   :'<b>Device</b> Radio Settings',
                         'class'   : 'wrapSection',
                         'objects' :[
                      
                                           {
                                               'type':'div',
                                               'title':'Station Tx Power Level',
                                               'class':'wrapDRS floatLeft'                                                         
                                           },
                                           {
                                               'type':'div',
                                               'id':'STA_TX_PL_SUM',
                                               'class':'wrapDRS floatRight'                                                         
                                           },

                                           {
                                               'type':'div',
                                               'class': 'clearM'
                                           },

                                           {
                                               'type':'div',
                                               'title':'Access Point Tx Power Level',
                                               'class':'wrapDRS floatLeft'                                                         
                                           },
                                           {
                                               'type':'div',
                                               'id':'AP_TX_PL_SUM',
                                               'class':'wrapDRS floatRight'                                                         
                                           },
                                           {
                                               'type':'div',
                                               'class': 'clearM'
                                           },

                                           {
                                               'type':'div',
                                               'title':'PHY Calibration Mode',
                                               'class':'wrapDRS floatLeft'                                                         
                                           },
                                           {
                                               'type':'div',
                                               'id':'PHY_CAL_MODE_SUM',
                                               'class':'wrapDRS floatRight'                                                         
                                           },
                                           {
                                               'type':'div',
                                               'class': 'clearM'
                                           },

                                           {
                                               'type':'div',
                                               'title':'Phy Tx Power',
                                               'class':'wrapDRS floatLeft hideFile'                                                         
                                           },
                                           {
                                               'type':'div',
                                               'id':'PHY_DEVICE_HIGH_TX_POWER_SUM',
                                               'class':'wrapDRS floatRight hideFile'                                                         
                                           }
                         
                         ]
                     },
                     {'type':'fieldset',
                         'id':'fieldsetSystemSettingsDeviceRoleSettingsGeneralSum',
                         'title':'<b>Device Role</b> General Settings',
                         'class': 'wrapSection',
                         'objects':[    {   'type':'fieldset',
                             'id':'fieldsetDeviceRoleSettingsGeneralModeSum',
                             'title':'Device Mode',
                             'class': 'wrapSection',
                             'objects':[
                                        {
                                            'type':'div',
                                            'title':'Start Role',
                                            'class':'wrapDRS floatLeft'                                                         
                                        },
                                        {
                                            'type':'div',
                                            'id':'START_ROLE_SELECT_SUM',
                                            'class':'wrapDRS floatRight'                                                         
                                        },

                                        {
                                            'type':'div',
                                            'class': 'clearM'
                                        },

                                        {
                                            'type':'div',
                                            'title':'Country Code',
                                            'class':'wrapDRS floatLeft'                                                         
                                        },
                                        {
                                            'type':'div',
                                            'id':'COUNTRY_CODE_SUM',
                                            'class':'wrapDRS floatRight'                                                         
                                        },
                             ]
                         }

                         ]
                     }
        ]
    }, 
        //Device 
        {   'id':'systemsettingsdevice',
            'title':'System Settings-> Device',
            'load':'localStorage.setItem(\'project\', ""); loadDevice();',
            'submit':'alert ("submit"); return false;',
            'connect':true,
            'objects':[
                        {'type':'fieldset',
                            'id':'fieldsetDeviceRadioSettingsSum',
                            'title':'Radio Settings',
                            'class': 'wrapSection',
                            'objects':[

                                           {
                                               'type':'div',
                                               'title':'Station Tx Power Level',
                                               'class':'wrapDRS floatLeft'                                                         
                                           },
                                           {
                                               'type':'div',
                                               'id':'STA_TX_PL_SUM',
                                               'class':'wrapDRS floatRight'                                                         
                                           },
                                        
                                           {
                                               'type':'div',
                                               'class': 'clearM'
                                           },
                                                     
                                           {
                                               'type':'div',
                                               'title':'Access Point Tx Power Level',
                                               'class':'wrapDRS floatLeft'                                                         
                                           },
                                           {
                                               'type':'div',
                                               'id':'AP_TX_PL_SUM',
                                               'class':'wrapDRS floatRight'                                                         
                                           },
                                           {
                                               'type':'div',
                                               'class': 'clearM'
                                           },
                                                     
                                           {
                                               'type':'div',
                                               'title':'PHY Calibration Mode',
                                               'class':'wrapDRS floatLeft'                                                         
                                           },
                                           {
                                               'type':'div',
                                               'id':'PHY_CAL_MODE_SUM',
                                               'class':'wrapDRS floatRight'                                                         
                                           },
                                           {
                                               'type':'div',
                                               'class': 'clearM'
                                           },
                                                     
                                           {
                                               'type':'div',
                                               'title':'Phy Tx Power',
                                               'class':'wrapDRS floatLeft hideFile'                                                         
                                           },
                                           {
                                               'type':'div',
                                               'id':'PHY_DEVICE_HIGH_TX_POWER_SUM',
                                               'class':'wrapDRS floatRight hideFile'                                                         
                                           }
                         
                            ]
                        },
                        /*{'type':'fieldset',
                             'id':'fieldsetDevicePoliciesSum',
                             'title':'Policies',
                             'class': 'wrapSection',
                             'objects':[]
                        }*/
            ]
        },
            //Radio Settings
            {   'id':'systemsettingsdeviceRadioSettings',
                'title':'System Settings-> Device -> Radio Settings',
                'load': 'localStorage.setItem(\'project\', ""); loadRadioSettings(); loadCoex(); loadAntSel();',   // add2_4gCal_Mode();
                'submit':'alert ("submit"); return false;',
                'connect':true,
                'objects':[
                            {'type':'fieldset',
                                'id':'fieldsetDeviceRadioSettings',
                                'title':'RF 2.4G',
                                'class': 'wrapSection',
                                'objects':[
                                            {   'id':'staTxPowLevel',
                                                'type':'div',
                                                'class': 'wrapDRS floatLeft',
                                                'objects':[
                                                     
                                                            {
                                                                'type':'div',
                                                                'class': 'fieldName',
                                                                'title':'Station Tx Power Level (dBm)'
                                                            },                                  
                                                            {
                                                                'id':'STA_TX_PL',
                                                                'class':'',
                                                                'type':'number',
                                                                'max':15,
                                                                'min':0,
                                                                'value':0,
                                                                //'change':'changeStationTxPowerLevel();'
                                                         
                                                            },
                                                            {
                                                                'type':'div',
                                                                'class': 'clearS'
                                                            },
                                                            {
                                                                'type':'div',
                                                                'class': 'descSmallText',
                                                                'title':'* 0- Max Tx Power, 15- Min Tx Power'
                                                            }]
                                            },/*staTxPowLevel*/
                                            {   'id':'apTxPowLevel',
                                                'type':'div',
                                                'class': 'wrapDRS floatRight',
                                                'objects':[
                                                     
                                                            {
                                                                'type':'div',
                                                                'class': 'fieldName',
                                                                'title':'Access Point Tx Power Level (dBm)&nbsp;'
                                                            },
                                                     
                                                            {
                                                                'id':'AP_TX_PL',
                                                                'class':'',
                                                                'type':'number',
                                                                'max':15,
                                                                'min':0,
                                                                'value':0
                                                         
                                                            },
                                                            {
                                                                'type':'div',
                                                                'class': 'clearS'
                                                            },
                                                            {
                                                                'type':'div',
                                                                'class': 'descSmallText',
                                                                'title':'* 0- Max Tx Power, 15- Min Tx Power'
                                                            }]
                                            },/*apTxPowLevel*/
                                            {
                                                'type':'div',
                                                'class': 'clearM'
                                            }, /*ClearM*/                            
                                            {
                                                'id':'phyCalibModeDiv',
                                                'type':'div',
                                                'class': 'wrapDRS floatLeft',
                                                'objects':[
                                                            {
                                                                'type':'div',
                                                                'class': 'fieldName',
                                                                'tooltip': 'For low power applications, TI recommends choosing Triggered mode over One-Time calibration mode, unless current peak limit is an absolute constraint.&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;   Triggered mode does not issue calibrations unless absolutely necessary, or manually triggered. Normal calibration mode is used to achieve the best RF performance, or when the environment of the device is prone to changes (temperature changes).',
                                                                'title':'PHY Calibration Mode'
                                                            },
                                                            {
                                                                'id':'PHY_CAL_MODE',
                                                                'class':'',
                                                                'type': 'select',
                                                                'values': [{'value':0,'text':'Normal'},{'value':1,'text':'Trigger'},{'value':2,'text':'Onetime'} ]
                                                            }
                                                ]				
                                            }, /*Calibration Mode*/
                                            {
                                                'id':'phyDeviceHihgTxPwdiv',
                                                'type':'div',
                                                'class': 'wrapDRS floatRight',
                                                'objects':[
                                                            {
                                                                'type':'div',
                                                                'class': 'fieldName',
                                                                'title':'PHY Tx Power',
                                                                'class': 'hideFile'
                                                            },
                                                            {
                                                                'id':'PHY_DEVICE_HIGH_TX_POWER',
                                                                'class':'',
                                                                'type':'select',
                                                                'values': [{'value':0,'text':'Normal'},{'value':1,'text':'High'}],
                                                                'class': 'hideFile'
                                                            }
                                                ]				
                                            }, /*PHY Tx Power 2.4G*/
                                            {
                                                'type': 'div',
                                                'class': 'clearM'
                                            }, /*ClearM*/
                                            {
                                                'type': 'fieldset',
                                                'id': 'fieldsetDeviceAdvancedRadio24Settings',
                                                'title': 'Advanced 2.4G',
                                                'class': 'wrapSection',
                                                'objects': [
                                                            {
                                                                'type': 'div',
                                                                'class': 'descSmallTextRed',
                                                                'title': '* Warning! Changing these values may defect regulatory radio certification or system performance !'
                                                            }, /*Warning*/
                                                            {
                                                                'id': 'btnRegDomain24Open',
                                                                'type': 'div',
                                                                'class': 'regButton floatRight',
                                                                'tooltip': 'Open advanced RF configuration',
                                                                'title': 'Configure',
                                                                'click': 'btnRegDomain24Open();'

                                                            } /*RegDomain*/
                                                ]
                                            }
                                     
                                ]
                            },/*radio 2.4G settings*/
                            {
                                'type': 'fieldset',
                                'id': 'fieldsetDeviceRadioSettingsF',
                                'title': 'RF 5G',
                                'class': 'wrapSection',
                                'objects': [
                                             {
                                                 'id': 'phyCalibModeDiv5G',
                                                 'type': 'div',
                                                 'class': 'wrapDRS floatLeft',
                                                 'objects': [
                                                             {
                                                                 'type': 'div',
                                                                 'class': 'fieldName',
                                                                 'title': 'PHY 5G Calibration Mode'
                                                             },
                                                             {
                                                                 'id': 'PHY_5G_CAL_MODE',
                                                                 'class': '',
                                                                 'type': 'select',
                                                                 'values': [{ 'value': 0, 'text': 'Normal' }]
                                                             }
                                                 ]
                                             }, /*Calibration Mode*/
                                             {
                                                 'id': 'Is5GEnabled',
                                                 'type': 'div',
                                                 'class': 'wrapDRS floatRight',
                                                 'objects': [
                                                             {
                                                                 'type': 'div',
                                                                 'class': 'fieldName',
                                                                 'title': '5 Ghz Support'
                                                             },
                                                             {
                                                                 'id': 'Is5GEnabledSelect',
                                                                 'class': '',
                                                                 'type': 'select',
                                                                 'values': [{ 'value': false, 'text': 'Disable' }, { 'value': true, 'text': 'Enable' }]
                                                             }
                                                 ]
                                             }, /*5G Enabled*/
                                             {
                                                 'type':'div',
                                                 'class': 'clearM'
                                             }, /*ClearM*/
                                             {
                                                 'type': 'fieldset',
                                                 'id': 'fieldsetDeviceAdvancedRadioSettings',
                                                 'title': 'Advanced 5G',
                                                 'class': 'wrapSection',
                                                 'objects': [
                                                             {
                                                                 'type': 'div',
                                                                 'class': 'descSmallTextRed',
                                                                 'title': '* Warning! Changing these values may defect regulatory radio certification or system performance !'
                                                             }, /*Warning*/
                                                             {
                                                                 'id': 'btnRegDomainOpen',
                                                                 'type': 'div',
                                                                 'class': 'regButton floatRight',
                                                                 'tooltip': 'Open advanced RF configuration',
                                                                 'title': 'Configure',
                                                                 'click': 'btnRegDomainOpen();'

                                                             } /*RegDomain*/
                                                 ]
                                             }
                                ]
                            },/*5G advanced - regdomain*/
                            {
                                'type': 'fieldset',
                                'id': 'fieldsetDeviceCoExist',
                                'title': 'Coexistence',
                                'class': 'wrapSection',
                                'objects': [
                                            {
                                                'id': 'coexModeDiv',
                                                'type': 'div',
                                                'class': 'wrapDRS floatLeft',
                                                'objects': [
                                                            {
                                                                'type': 'div',
                                                                'class': 'fieldName',
                                                                'title': 'Mode'
                                                            },
                                                            {
                                                                'id': 'COEX_MODE',
                                                                'class': '',
                                                                'type': 'select',
                                                                'values': [{ 'value': 0, 'text': 'Disable' }, { 'value': 1, 'text': 'Single Ant' }, { 'value': 2, 'text': 'Dual Ant' }],
                                                                'change': 'changeCoex();'
                                                            }
                                                ]
                                            },
                                            {
                                                'id': 'coexInputPadDiv',
                                                'type': 'div',
                                                'class': 'wrapDRS floatRight',
                                                'objects': [
                                                            {
                                                                'type': 'div',
                                                                'class': 'fieldName',
                                                                'title': 'Input Pad(GPIO)(PIN)'
                                                            },
                                                            {
                                                                'id': 'INPUT_PAD',
                                                                'class': '',
                                                                'type': 'select',
                                                                'values': []
                                                            }
                                                ]
                                            },
                                            {
                                                'id': 'coexOutputPadDiv',
                                                'type': 'div',
                                                'class': 'wrapDRS floatRight',
                                                'objects': [
                                                            {
                                                                'type': 'div',
                                                                'class': 'fieldName',
                                                                'title': 'Output Pad(GPIO)(PIN)'
                                                            },
                                                            {
                                                                'id': 'OUTPUT_PAD',
                                                                'class': '',
                                                                'type': 'select',
                                                                'values': []
                                                            }
                                                ]
                                            }
                                ]
                            },/*coex*/
                            {
                                'type': 'fieldset',
                                'id': 'fieldsetDeviceAntSelect',
                                'title': 'Antenna Selection',
                                'class': 'wrapSection',
                                'objects': [
                                            {
                                                'id': 'antselModeDiv',
                                                'type': 'div',
                                                'class': 'wrapDRS floatLeft',
                                                'objects': [
                                                            {
                                                                'type': 'div',
                                                                'class': 'fieldName',
                                                                'title': 'Mode'
                                                            },
                                                            {
                                                                'id': 'ANT_SEL_MODE',
                                                                'class': '',
                                                                'type': 'select',
                                                                'values': [{ 'value': 0, 'text': 'Disable' }, { 'value': 1, 'text': 'Ant 1' }, { 'value': 2, 'text': 'Ant 2' }, { 'value': 3, 'text': 'Autoselect' }],
                                                                'change': 'changeAntSel();'
                                                            }
                                                ]
                                            },
                                            {
                                                'id': 'antselAnt1PadDiv',
                                                'type': 'div',
                                                'class': 'wrapDRS floatRight',
                                                'objects': [
                                                            {
                                                                'type': 'div',
                                                                'class': 'fieldName',
                                                                'title': 'Ant1 Pad(GPIO)(PIN)'
                                                            },
                                                            {
                                                                'id': 'ANT1_PAD',
                                                                'class': '',
                                                                'type': 'select',
                                                                'values': []
                                                            }
                                                ]
                                            },
                                            {
                                                'id': 'antselAnt2PadDiv',
                                                'type': 'div',
                                                'class': 'wrapDRS floatRight',
                                                'objects': [
                                                            {
                                                                'type': 'div',
                                                                'class': 'fieldName',
                                                                'title': 'Ant2 Pad(GPIO)(PIN)'
                                                            },
                                                            {
                                                                'id': 'ANT2_PAD',
                                                                'class': '',
                                                                'type': 'select',
                                                                'values': []
                                                            }
                                                ]
                                            }
                                ]
                            } /*antenna selection*/

                ]
            },
            //Dice
            {
                'id': 'systemsettingsdeviceDiceSettings',
                'title':'System Settings-> Device-> Device Identity',
                'load': 'localStorage.setItem(\'project\', ""); loadDice(); addCountryCode5G("#csrCountryCodeSelect"); autocompleteEnable("input#csrCountryCodeInput");',//autocompleteCSRYearEnable("input#csr_starts_year");autocompleteCSRYearEnable("input#csr_ends_year");autocompleteCSRMonthEnable("input#csr_starts_month");addCSRMonth(csr_starts_month_select);addCSRYear(csr_starts_year_select);addCSRYear(csr_ends_year_select)',
                'submit':'alert ("submit"); return false;',
                'connect':true,
                'objects': [
                            {
                                'type': 'fieldset',
                                'id': 'fieldsetDiceSettings',
                                'title': 'Device Identity Configuration',
                                'class': 'wrapSection',
                                'objects': [
                                            {
                                                'id': 'enableDiceTable',
                                                'type': 'div',
                                                'class': 'floatLeft padright10',
                                                'objects': [               
                                                            {
                                                                'id': 'enableDiceDiv',
                                                                'type': 'div',
                                                                'class': 'wrapDRS floatLeft',
                                                                'objects': [
                                                                            {
                                                                                'type': 'div',
                                                                                'class': 'fieldName'/*,
                                                                            'tooltip': 'Enable Device Identity',
                                                                            'title': 'DICE'*/
                                                                            },
                                                                            {
                                                                                'id': 'ENABLE_DICE',
                                                                                'class': '',
                                                                                'type': 'select',
                                                                                'values': [{ 'value': 0, 'text': 'Disable DICE' }, { 'value': 1, 'text': 'Enable DICE' }],
                                                                                'change': 'updateDiceGui();'
                                                                            }
                                                                ]
                                                            },
                                                            {
                                                                'type': 'div',
                                                                'class': 'clearM'
                                                            },//ClearM
                                                            {
                                                                'id': 'randomNumDiceTable',
                                                                'type': 'div',
                                                                'class': '',
                                                                'objects': [
                                                                            {
                                                                                'id': 'RANDOM_NUM_DICE',
                                                                                'class': '',
                                                                                'type': 'checkbox',
                                                                                'label': 'bot 2',
                                                                                'tooltip': 'Use Random Number',
                                                                                'values': [{ 'value': 1, 'text': 'Use Random Number' }],
                                                                                'checked': 0
                                                                            }
                                                                ]
                                                            }        
                                                ]
                                            }
                                ]
                            },/*ENABLE_DICE*/
                            {
                                'type': 'fieldset',
                                'id': 'fieldsetSelSigned',
                                'title': 'Certificate Configuration',
                                'class': 'wrapSection',
                                'objects': [ 
                                               {
                                                   'id': 'useSelfCertDiceTable',
                                                   'type': 'div',
                                                   'class': 'floatLeft padright10',
                                                   'objects': [
                                                                   {
                                                                       'id': 'csrConfigDiv',
                                                                       'type': 'div',
                                                                       'class': 'wrapDRS floatLeft',
                                                                       'objects': [
                                                                                   {
                                                                                       'type': 'div',
                                                                                       'class': 'fieldName'/*,
                                                                                    'tooltip': 'Certificate Options',
                                                                                    'title': 'Self Certificate'*/
                                                                                   },
                                                                                   {
                                                                                       'id': 'CREATE_CSR',
                                                                                       'class': '',
                                                                                       'type': 'select',
                                                                                       'values': [{ 'value': 2, 'text': 'Disable' }, { 'value': 0, 'text': 'Certificate Sign Request' },{ 'value': 1, 'text': 'Self Signed Certificate' } ],
                                                                                       'change': 'updateCSRGUI();'
                                                                                   }
                                                                       ]
                                                                   }
                                                   ]
                                               },
                                               {
                                                   'type': 'div',
                                                   'class': 'clearS'
                                               },//ClearS
                                               {
                                                   'id': 'csrVendorDiv',
                                                   'type': 'div',
                                                   'class': 'floatLeft padright10',
                                                   'objects': [
                                                                   {
                                                                       'id': 'CSR_VENDOR',
                                                                       'class': '',
                                                                       'type': 'checkbox',
                                                                       'label': 'bot 2',
                                                                       'tooltip': 'Secured Vendor',
                                                                       'values': [{ 'value': 1, 'text': 'Vendor' }],
                                                                       'checked': 0
                                                                   }
                                                   ]
                                               },//CSR VENDOR
                                               {
                                                   'id': 'csrTokenDiv',
                                                   'type': 'div',
                                                   'class': 'floatRight',
                                                   'objects': [
                                                                   {
                                                                       'title': 'File Token:',
                                                                       'type': 'div',
                                                                       'class': 'fieldName'
                                                                   },
                                                                   {
                                                                       'id': 'CSR_TOKEN',
                                                                       'class': 'fieldName',
                                                                       'type': 'number'
                                                                   }
                                                   ]
                                               } //CSR TOKEN

                                ]
                            },
                            {
                                'type': 'fieldset',
                                'id': 'fieldsetCSRSettings',
                                'title': 'Certificate Info',
                                'class': 'wrapSection',
                                'objects': [
                                            {
                                                'id': 'csrCertSerialNameDiv',
                                                'type':'div',
                                                'class': 'wrapDRS',
                                                'objects':[
                                                                {
                                                                    'id': 'csrCertSerialName',
                                                                    'title': 'Certificate serial number',
                                                                    'type': 'div',
                                                                    'class': 'fieldName',
                                                                    'tooltip': 'Up to 8 digits'
                                                                },
                                                                {
                                                                    'id': 'csrCertSerialNameText',
                                                                    'class': 'fieldName',
                                                                    'type': 'number'
                                                                },//CSR SERIAL
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                }//ClearM     
                                                ]
                                            },//CSR SERIAL
                                            {
                                                'id': 'csrValidityNameStartDiv',
                                                'type':'div',
                                                'class': 'wrapDRS floatLeft',
                                                'objects':[
                                                            {
                                                                'id': 'csrValidityName',
                                                                'title': 'Certification validity start',
                                                                'type': 'div',
                                                                'class': 'fieldName',
                                                                'tooltip': 'YYYY/MM/DD - DD/MM/YYYY'
                                                            },
                                                            {
                                                                'id': 'csrValidityNameeTextSY',
                                                                'type': 'div',
                                                                'class': 'floatLeft  padright22 wrapDRSmallY select-editablecsr_y',
                                                                'title': 'Y',
                                                                'objects': [
                                                                            {
                                                                                'id': 'csr_starts_year_select',
                                                                                'class': '',
                                                                                'type': 'select',
                                                                                'values': [
                                                                                    { 'value': '2013', 'text': '2013' },
                                                                                    { 'value': '2014', 'text': '2014' },
                                                                                    { 'value': '2015', 'text': '2015' },
                                                                                    { 'value': '2016', 'text': '2016' },
                                                                                    { 'value': '2017', 'text': '2017' },
                                                                                    { 'value': '2018', 'text': '2018' },
                                                                                    { 'value': '2019', 'text': '2019' },
                                                                                    { 'value': '2020', 'text': '2020' },
                                                                                    { 'value': '2021', 'text': '2021' },
                                                                                    { 'value': '2022', 'text': '2022' },
                                                                                    { 'value': '2023', 'text': '2023' },
                                                                                    { 'value': '2024', 'text': '2024' },
                                                                                    { 'value': '2025', 'text': '2025' },
                                                                                    { 'value': '2026', 'text': '2026' },
                                                                                    { 'value': '2027', 'text': '2027' },
                                                                                    { 'value': '2028', 'text': '2028' },
                                                                                    { 'value': '2029', 'text': '2029' },
                                                                                    { 'value': '2030', 'text': '2030' },
                                                                                    { 'value': '2031', 'text': '2031' },
                                                                                    { 'value': '2032', 'text': '2032' },
                                                                                    { 'value': '2033', 'text': '2033' },
                                                                                    { 'value': '2034', 'text': '2034' },
                                                                                    { 'value': '2035', 'text': '2035' },
                                                                                    { 'value': '2036', 'text': '2036' },
                                                                                    { 'value': '2037', 'text': '2037' }
                                                                                ]
                                                                            },
                                                               

                                                                            {
                                                                                'id': 'csr_starts_year',
                                                                                'class': '',
                                                                                'type': 'input',
                                                                                'value': '2013'
                                                                            }]

                                                            },//Start Year
                                                            {
                                                                'id': 'csrValidityNameeTextSM',
                                                                'type': 'div',
                                                                'class': 'floatLeft  padright22 wrapDRSmallY select-editablecsr_y',
                                                                'title': 'M',
                                                                'objects': [{
                                                                    'id': 'csr_starts_month_select',
                                                                    'class': '',
                                                                    'type': 'select',
                                                                    'values': [
                                                                               { 'value': '01', 'text': '01' },
                                                                               { 'value': '02', 'text': '02' },
                                                                               { 'value': '03', 'text': '03' },
                                                                               { 'value': '04', 'text': '04' },
                                                                               { 'value': '05', 'text': '05' },
                                                                               { 'value': '06', 'text': '06' },
                                                                               { 'value': '07', 'text': '07' },
                                                                               { 'value': '08', 'text': '08' },
                                                                               { 'value': '09', 'text': '09' },
                                                                               { 'value': '10', 'text': '10' },
                                                                               { 'value': '11', 'text': '11' },
                                                                               { 'value': '12', 'text': '12' }   ]
                                                                },
                                                                            {
                                                                                'id': 'csr_starts_month',
                                                                                'class': '',
                                                                                'type': 'input',
                                                                                'value': '01'
                                                                            }]

                                                            },//Start Month
                                                            {
                                                                'id': 'csrValidityNameeTextSD',
                                                                'type': 'div',
                                                                'class': 'floatLeft  padright22 wrapDRSmallY select-editablecsr_y',
                                                                'title': 'D',
                                                                'objects': [
                                                                            {
                                                                                'id': 'csr_starts_day_select',
                                                                                'class': '',
                                                                                'type': 'select',
                                                                                'values': []
                                                                            },
                                                                                    {
                                                                                        'id': 'csr_starts_day',
                                                                                        'class': '',
                                                                                        'type': 'input',
                                                                                        'value': '01'
                                                                                    }]

                                                            },//Start Day
                                                            {
                                                                'type': 'div',
                                                                'class': 'clearM'
                                                            },//ClearM
                                                            {
                                                                'id': 'csr_starts_date_warning',
                                                                'type': 'div',
                                                                'class': 'descSmallTextRed',
                                                                'title': '&nbsp;&#8196;&#8196;'
                                                            }, /*Warning*/
                                                            {
                                                                'id': 'csr_starts_date_clearB',
                                                                'type': 'div',
                                                                'class': 'clearB'
                                                            },//ClearB
                                                            {
                                                                'id': 'csr_starts_date_clearM',
                                                                'type': 'div',
                                                                'class': 'clearM'
                                                            },//ClearM
                                                ]
                                            },//Validity Start
                                            {
                                                'id': 'csrValidityNameEndDiv',
                                                'type': 'div',
                                                'class': 'wrapDRS floatRight',
                                                'objects': [
                                                            {
                                                                'id': 'csrValidityNameEnd',
                                                                'title': 'Certification validity end',
                                                                'type': 'div',
                                                                'class': 'fieldName padleft10',
                                                                'tooltip': 'YYYY/MM/DD - DD/MM/YYYY'
                                                            },
                                                            {
                                                                'id': 'csrValidityNameeTextEY',
                                                                'type': 'div',
                                                                'class': 'floatLeft  padright22 wrapDRSmallY select-editablecsr_y',
                                                                'title': 'Y',
                                                                'objects': [{
                                                                    'id': 'csr_ends_year_select',
                                                                    'class': '',
                                                                    'type': 'select',
                                                                    'values': [
                                                                                { 'value': '2013', 'text': '2013' },
                                                                                { 'value': '2014', 'text': '2014' },
                                                                                { 'value': '2015', 'text': '2015' },
                                                                                { 'value': '2016', 'text': '2016' },
                                                                                { 'value': '2017', 'text': '2017' },
                                                                                { 'value': '2018', 'text': '2018' },
                                                                                { 'value': '2019', 'text': '2019' },
                                                                                { 'value': '2020', 'text': '2020' },
                                                                                { 'value': '2021', 'text': '2021' },
                                                                                { 'value': '2022', 'text': '2022' },
                                                                                { 'value': '2023', 'text': '2023' },
                                                                                { 'value': '2024', 'text': '2024' },
                                                                                { 'value': '2025', 'text': '2025' },
                                                                                { 'value': '2026', 'text': '2026' },
                                                                                { 'value': '2027', 'text': '2027' },
                                                                                { 'value': '2028', 'text': '2028' },
                                                                                { 'value': '2029', 'text': '2029' },
                                                                                { 'value': '2030', 'text': '2030' },
                                                                                { 'value': '2031', 'text': '2031' },
                                                                                { 'value': '2032', 'text': '2032' },
                                                                                { 'value': '2033', 'text': '2033' },
                                                                                { 'value': '2034', 'text': '2034' },
                                                                                { 'value': '2035', 'text': '2035' },
                                                                                { 'value': '2036', 'text': '2036' },
                                                                                { 'value': '2037', 'text': '2037' }]
                                                                },
                                                                            {
                                                                                'id': 'csr_ends_year',
                                                                                'class': '',
                                                                                'type': 'input',
                                                                                'value': '2013'
                                                                            }]

                                                            },//End Year
                                                            {
                                                                'id': 'csrValidityNameeTextEM',
                                                                'type': 'div',
                                                                'class': 'floatLeft  padright22 wrapDRSmallY select-editablecsr_y',
                                                                'title': 'M',
                                                                'objects': [{
                                                                    'id': 'csr_ends_month_select',
                                                                    'class': '',
                                                                    'type': 'select',
                                                                    'values': [
                                                                               { 'value': '01', 'text': '01' },
                                                                               { 'value': '02', 'text': '02' },
                                                                               { 'value': '03', 'text': '03' },
                                                                               { 'value': '04', 'text': '04' },
                                                                               { 'value': '05', 'text': '05' },
                                                                               { 'value': '06', 'text': '06' },
                                                                               { 'value': '07', 'text': '07' },
                                                                               { 'value': '08', 'text': '08' },
                                                                               { 'value': '09', 'text': '09' },
                                                                               { 'value': '10', 'text': '10' },
                                                                               { 'value': '11', 'text': '11' },
                                                                               { 'value': '12', 'text': '12' }   ]
                                                                },
                                                                            {
                                                                                'id': 'csr_ends_month',
                                                                                'class': '',
                                                                                'type': 'input',
                                                                                'value': '01'
                                                                            }]
                                                            },//End Month
                                                            {
                                                                'id': 'csrValidityNameeTextED',
                                                                'type': 'div',
                                                                'class': 'floatLeft  padright10 wrapDRSmallY select-editablecsr_y',
                                                                'title': 'D',
                                                                'objects': [
                                                                            {
                                                                                'id': 'csr_ends_day_select',
                                                                                'class': '',
                                                                                'type': 'select',
                                                                                'values': []
                                                                            },
                                                                                    {
                                                                                        'id': 'csr_ends_day',
                                                                                        'class': '',
                                                                                        'type': 'input',
                                                                                        'value': '01'
                                                                                    }]
                                                            },//End Day
                                                            {
                                                                'type': 'div',
                                                                'class': 'clearM'
                                                            },//ClearS
                                                            {
                                                                'id': 'csr_ends_date_warning',
                                                                'type': 'div',
                                                                'class': 'descSmallTextRed',
                                                                'title': '* wrong date value!'
                                                            }, /*Warning*/
                                                            {
                                                                'id': 'csr_ends_date_clearB',
                                                                'type': 'div',
                                                                'class': 'clearB'
                                                            },//ClearB
                                                            {
                                                                'id': 'csr_ends_date_clearM',
                                                                'type': 'div',
                                                                'class': 'clearM'
                                                            },//ClearM
                                                ]
                                            },//Validity End
                                            {
                                                'id': 'csrCertCA',
                                                'type': 'div',
                                                //'class': 'wrapDRS floatLeft padtop17',
                                                'class': 'wrapDRS floatLeft',
                                                'objects': [
                                                            {
                                                                'id': 'IS_CSR_ISCA',
                                                                'type': 'div',
                                                                'class': 'fieldName',
                                                                'title': 'Is certificate CA?'
                                                            },
                                                            {
                                                                'id': 'CSR_ISCA',
                                                                'class': '',
                                                                'type': 'select',
                                                                'values': [{ 'value': false, 'text': 'No' }, { 'value': true, 'text': 'Yes' }]
                                                            }
                                                ]
                                            },//CERT-ISCA
                                            {
                                                'id': 'certCountryDiv',
                                                'type': 'div',
                                                'class': 'wrapDRS floatRight', //mright40
                                                'objects': [
                                                            {
                                                                'type': 'div',
                                                                'class': 'fieldName',
                                                                'title': 'Subject Country Code'
                                                            },
                                                            {
                                                                'id': 'csrCountryCodeInnerDiv',
                                                                'type': 'div',
                                                                'class': 'select-editable wrapDRS floatRight',
                                                                'objects': [
                                                                                {
                                                                                    'id': 'csrCountryCodeSelect',
                                                                                    'class': '',
                                                                                    'type': 'select',
                                                                                    'values': [{ 'value': 'EU', 'text': 'Europe' },
                                                                                                { 'value': 'US', 'text': 'USA' },
                                                                                                { 'value': 'JP', 'text': 'Japan' }]
                                                                                },
                                                                                {
                                                                                    'id': 'csrCountryCodeInput',
                                                                                    'type': 'input',
                                                                                    'class': '',
                                                                                    'value': ''
                                                                                }]
                                                            }
                                                ]
                                            }, //CSR Country Code
                                            {
                                                'type': 'div',
                                                'class': 'clearM'
                                            },
                                            {
                                                'id': 'csrStateNameDiv',
                                                'type': 'div',
                                                'class': 'wrapDRS floatLeft',
                                                'objects': [
                                                                {
                                                                    'id': 'csrStateName',
                                                                    'title': 'State',
                                                                    'type': 'div',
                                                                    'class': 'fieldName',
                                                                    'tooltip': 'Max size is 64'
                                                                },
                                                                {
                                                                    'id': 'csrStateNameText',
                                                                    'class': 'fieldName',
                                                                    'type': 'text'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                }//ClearM
                                                ]
                                            },//csr STATE
                                            {
                                                'id': 'csrLocalityNameDiv',
                                                'type': 'div',
                                                'class': 'wrapDRS floatRight',
                                                'objects': [
                                                            {
                                                                'id': 'csrLocalityName',
                                                                'title': 'Locality',
                                                                'type': 'div',
                                                                'class': 'fieldName',
                                                                'tooltip': 'Max size is 64'
                                                            },
                                                            {
                                                                'id': 'csrLocalityNameText',
                                                                'class': 'fieldName',
                                                                'type': 'text'
                                                            },
                                                            {
                                                                'type': 'div',
                                                                'class': 'clearM'
                                                            }
                                                ]
                                            },//csr Locality
                                            {
                                                'id': 'csrSurnameNameDiv',
                                                'type': 'div',
                                                'class': 'wrapDRS floatLeft',
                                                'objects': [
                                                            {
                                                                'id': 'csrSurnameName',
                                                                'title': 'Surname',
                                                                'type': 'div',
                                                                'class': 'fieldName',
                                                                'tooltip': 'Max size is 64'
                                                            },
                                                            {
                                                                'id': 'csrSurnameNameText',
                                                                'class': 'fieldName',
                                                                'type': 'text'
                                                            },
                                                            {
                                                                'type': 'div',
                                                                'class': 'clearM'
                                                            }
                                                ]
                                            },//csr Surname
                                            {
                                                'id': 'csrOrganizationNameDiv',
                                                'type': 'div',
                                                'class': 'wrapDRS floatRight',
                                                'objects': [
                                                                {
                                                                    'id': 'csrOrganizationName',
                                                                    'title': 'Organization',
                                                                    'type': 'div',
                                                                    'class': 'fieldName',
                                                                    'tooltip': 'Max size is 64'
                                                                },
                                                                {
                                                                    'id': 'csrOrganizationNameText',
                                                                    'class': 'fieldName',
                                                                    'type': 'text'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                }
                                                ]
                                            },//csr Organization
                                            {
                                                'id': 'csrOrganizationUnitNameDiv',
                                                'type': 'div',
                                                'class': 'wrapDRS floatLeft',
                                                'objects': [
                                                                {
                                                                    'id': 'csrOrganizationUnitName',
                                                                    'title': 'Organization Unit',
                                                                    'type': 'div',
                                                                    'class': 'fieldName',
                                                                    'tooltip': 'Max size is 64'
                                                                },
                                                                {
                                                                    'id': 'csrOrganizationUnitNameText',
                                                                    'class': 'fieldName',
                                                                    'type': 'text'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                }
                                                ]
                                            },//csr Organization Unit    
                                            {
                                                'id': 'csrEmailDiv',
                                                'type': 'div',
                                                'class': 'wrapDRS floatRight',
                                                'objects': [
                                                                {
                                                                    'id': 'csrEmail',
                                                                    'title': 'Email',
                                                                    'type': 'div',
                                                                    'class': 'fieldName',
                                                                    'tooltip': 'Max size is 64'
                                                                },
                                                                {
                                                                    'id': 'csrEmailText',
                                                                    'class': 'fieldName',
                                                                    'type': 'text'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                }
                                                ]
                                            },//csr Email*/
                                            {
                                                'id': 'csrNameDiv',
                                                'type': 'div',
                                                'class': 'wrapDRS floatLeft',
                                                'objects': [
                                                                {
                                                                    'id': 'csrName',
                                                                    'title': 'Common Name',
                                                                    'type': 'div',
                                                                    'class': 'fieldName',
                                                                    'tooltip': 'Max size is 64'
                                                                },
                                                                {
                                                                    'id': 'csrNameText',
                                                                    'class': 'fieldName',
                                                                    'type': 'text'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                }
                                                ]
                                            },//csr Name
                                            {
                                                'id': 'UDID_name_div',
                                                'type': 'div',
                                                'class': 'wrapDRS floatRight padtop20 padright35',
                                            
                                                'objects': [
                                                                {
                                                                    'id': 'UDID_NAME',
                                                                    'class': '',
                                                                    'type': 'checkbox',
                                                                    'label': 'bot 2',
                                                                    'tooltip': 'Use Unique device ID (UDID) as common name',
                                                                    'values': [{ 'value': 0, 'text': 'Use Unique device ID (UDID)' }],
                                                                    'checked': 0
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'padleft20',
                                                                    'title': 'as common name.'
                                                                }
                                                ]
                                            } //Use UDID         
                                        
                                ]
                            }
                ]
            },
            //Policies
            {   'id':'systemsettingsdevicepolicies',
                'title':'System Settings-> Device-> Policies',
                'load':'localStorage.setItem(\'project\', "");',
                'submit':'alert ("submit"); return false;',
                'connect':true,
                'objects':[]
            },
        //Device Role Settings
        {   'id':'devicerolesettings',
            'title':'System Settings-> Device Role Settings',
            'load':'localStorage.setItem(\'project\', ""); loadDeviceRoleSettingsSum()',
            'submit':'alert ("submit"); return false;',
            'connect':true,
            'objects':[
                        {   'type':'fieldset',
                            'id':'fieldsetDeviceRoleSettingsGeneralSum',
                            'title':'General Settings',
                            'class': 'wrapSection',
                            'objects':[    {   'type':'fieldset',
                                'id':'fieldsetDeviceRoleSettingsGeneralModeSum',
                                'title':'Device Mode',
                                'class': 'wrapSection',
                                'objects':[
                                           {
                                               'type':'div',
                                               'title':'Start Role',
                                               'class':'wrapDRS floatLeft'                                                         
                                           },
                                           {
                                               'type':'div',
                                               'id':'START_ROLE_SELECT_SUM',
                                               'class':'wrapDRS floatRight'                                                         
                                           },
                                                        
                                           {
                                               'type':'div',
                                               'class': 'clearM'
                                           },
                                                        
                                           {
                                               'type':'div',
                                               'title':'Country Code',
                                               'class':'wrapDRS floatLeft'                                                         
                                           },
                                           {
                                               'type':'div',
                                               'id':'COUNTRY_CODE_SUM',
                                               'class':'wrapDRS floatRight'                                                         
                                           },
                                ]
                            }
                                    
                            ]
                        }
            ]
        }, 
            //General Settings
            {'id':'devicerolesettingsgeneralsettings',
                'title':'Role Settings > General Settings',
                'load': 'localStorage.setItem(\'project\', ""); loadDeviceRoleGeneralSettings(); addCountryCode5G("#CountryCodeSelect"); autocompleteEnable("input#CountryCodeInput");',
                'submit':'alert ("submit"); return false;',
                'connect':true,
                'objects':[ {'type':'fieldset',
                    'id':'fieldsetDeviceMode',
                    'title':'Device Mode',
                    'class': 'wrapSection',
                    'objects':[
                                    {
                                        'id':'SecurityTypeDiv',
                                        'type':'div',
                                        'class': 'wrapDRS floatLeft',
                                        'objects':[
                                                    {
                                                        'type':'div',
                                                        'class': 'fieldName',
                                                        'title':'Start Role'
                                                    },
                                                    {
                                                        'id':'StartRoleSelect',
                                                        'class':'',
                                                        'type': 'select',
                                                        'change':'changeRoleSelect()',
                                                        'values': [{'value':2,'text':'Access Point'},{'value':3,'text':'P2P'},{'value':0,'text':'Station'} ]
                                                    }
                                        ]				
                                    },
                                    {
                                        'id': 'CountryCodeDiv',
                                        'type': 'div',
                                        'class': 'wrapDRS floatRight',
                                        'objects': [
                                                    {
                                                        'type': 'div',
                                                        'class': 'fieldName',
                                                        'title': 'Country Code'
                                                    },
                                                    {
                                                        'id': 'CountryCodeInnerDiv',
                                                        'type': 'div',
                                                        'class': 'select-editable wrapDRS floatRight',
                                                        'objects': [
                                                        {
                                                            'id': 'CountryCodeSelect',
                                                            'class': '',
                                                            'type': 'select',
                                                            'values': [{ 'value': 'EU', 'text': 'Europe' },
                                                                        { 'value': 'US', 'text': 'USA'      },
                                                                        { 'value': 'JP', 'text': 'Japan'    } ]
                                                        },
                                                        {
                                                            'id': 'CountryCodeInput',
                                                            'type': 'input',
                                                            'class': '',
                                                            'value': ''
                                                        }]
                                                    }
                                        ]
                                    }
                    ], 
                },
                            {'type':'fieldset',
                                'id':'fieldsetRoleSettingsGeneralSettingsGeneral',
                                'title':'General',
                                'class': 'wrapSection',
                                'objects':[
                                
                                            {
                                                'id':'deviceName',
                                                'title':'Device Name',
                                                'type':'div',
                                                'class': 'fieldName',
                                            },
                                            {
                                                'id':'deviceNameText',
                                                'class':'fieldName',
                                                'type':'text'
                                            },
                                            {
                                                'type':'div',
                                                'class': 'clearM'
                                            }
                                ]
                            },
                            {'type':'fieldset',
                                'id':'fieldsetConnectPolicy',
                                'title':'Connection Policy',
                                'class': 'wrapSection',
                                'objects':[
                                               {
                                                   'id':'conPolAutoStart',
                                                   'type':'div',
                                                   'class': 'wrapDRS floatLeft',
                                                   'objects':[
                                                               {
                                                                   'type' : 'div',
                                                                   'class': 'fieldName',
                                                                   'title': 'Auto Connect'
                                                               },
                                                               {
                                                                   'id':'conPolAutoStartSelect',
                                                                   'class':'',
                                                                   'type':'select',
                                                                   'values': [{'value':'0','text':'Disable'},{'value':'1','text':'Enable'}]
                                                               }
                                                   ]				
                                               },
                                               {
                                                   'id':'conPolFastConnect',
                                                   'type':'div',
                                                   'class': 'wrapDRS floatRight',
                                                   'objects':[
                                                               {
                                                                   'type' : 'div',
                                                                   'class': 'fieldName',
                                                                   'title': 'Fast Connect'
                                                               },
                                                               {
                                                                   'id':'conPolFastConnectSelect',
                                                                   'class':'',
                                                                   'type':'select',
                                                                   'values': [{'value':'0','text':'Disable'},{'value':'1','text':'Enable'}]
                                                               }
                                                   ]				
                                               },
                                               {
                                                   'type':'div',
                                                   'class': 'clearM'
                                               },
                                               {
                                                   'id':'conAnyWFDirect',
                                                   'type':'div',
                                                   'class': 'wrapDRS floatLeft',
                                                   'objects':[
                                                               {
                                                                   'type' : 'div',
                                                                   'class': 'fieldName',
                                                                   'title': 'Any Wi-Fi® Direct'
                                                               },
                                                               {
                                                                   'id':'conAnyWFDirectSelect',
                                                                   'class':'',
                                                                   'type':'select',
                                                                   'values': [{'value':'0','text':'Disable'},{'value':'1','text':'Enable'}]
                                                               }
                                                   ]				
                                               },
                                               {
                                                   'id':'conPolAutoProvisioning',
                                                   'type':'div',
                                                   'class': 'wrapDRS floatRight',
                                                   'objects':[
                                                               {
                                                                   'type' : 'div',
                                                                   'class': 'fieldName',
                                                                   'title': 'Auto Provisioning'
                                                               },
                                                               {
                                                                   'id':'conPolAutoProvisioningSelect',
                                                                   'class':'',
                                                                   'type':'select',
                                                                   'values': [{'value':'0','text':'Disable'},{'value':'1','text':'Enable'}],
                                                                   'change':'changeAutoProvisioningSelect();'
                                                               }
                                                   ]				
                                               }
                                    
                                ]
                            },
                            {'type':'fieldset',
                                'id':'fieldsetProvisioning',
                                'title':'Auto Provisioning',
                                'class': 'wrapSection',
                                'objects':[
                                    
                                            {
                                                'id':'AutoProvExternalConfirm',
                                                'type':'div',
                                                'class': 'wrapDRS floatLeft',
                                                'objects':[
                                                            {
                                                                'type' : 'div',
                                                                'class': 'fieldName',
                                                                'title': 'Auto Provisioning External Confirmation'
                                                            },
                                                            {
                                                                'id':'AutoProvExternalConfSelect',
                                                                'class':'',
                                                                'type':'select',
                                                                'values': [{'value':0,'text':'Disable'},{'value':1,'text':'Enable'}]
                                                            }
                                                ]				
                                            }
                                ]
                            },
                ]
            },
            //STA/P2P Device
            {   'id':'devicerolesettingsstadevice',
                'title':'System Settings -> Device Role Settings-> STA/Wi-Fi® Direct Device',
                'load':'localStorage.setItem(\'project\', ""); loadSTADirectDeviceSum();',
                'submit':'alert ("submit"); return false;',
                'connect':true,
                'objects':[
                            /*{   'type':'fieldset',
                                'id':'fieldsetSTADirectDeviceProfileSum',
                                'title':'Profiles',
                                'class': 'wrapSection',
                                'objects':[]
                            },*/
                            {
                                'type':'div',
                                'class': 'clearM'
                            },
                            {   'type':'fieldset',
                                'id':'fieldsetSTADirectDeviceNWSettingsSum',
                                'title':'Network Settings',
                                'class': 'wrapSection',
                                'objects':[
                                            {
                                                'type':'div',
                                                'title':'DHCP Client Enable',
                                                'class':'wrapDRS floatLeft'                                                         
                                            },
                                            {
                                                'type':'div',
                                                'id':'DHCP_CLIENT_ENABLE_SUM',
                                                'class':'wrapDRS floatRight'                                                         
                                            },
                                        
                                            {
                                                'type':'div',
                                                'class': 'clearM'
                                            },
                                        
                                            {
                                                'type':'div',
                                                'title':'IP Address',
                                                'class':'wrapDRS floatLeft'                                                         
                                            },
                                            {
                                                'type':'div',
                                                'id':'STA_IP_ADDRESS_SUM',
                                                'class':'wrapDRS floatRight'                                                         
                                            },
                                        
                                            {
                                                'type':'div',
                                                'class': 'clearM'
                                            },
                                        
                                            {
                                                'type':'div',
                                                'title':'Subnet Mask',
                                                'class':'wrapDRS floatLeft'                                                         
                                            },
                                            {
                                                'type':'div',
                                                'id':'STA_SUBNET_MASK_SUM',
                                                'class':'wrapDRS floatRight'                                                         
                                            },
                                        
                                            {
                                                'type':'div',
                                                'class': 'clearM'
                                            },
                                        
                                            {
                                                'type':'div',
                                                'title':'Default Gateway',
                                                'class':'wrapDRS floatLeft'                                                         
                                            },
                                            {
                                                'type':'div',
                                                'id':'STA_DEFAULT_GW_SUM',
                                                'class':'wrapDRS floatRight'                                                         
                                            },
                                        
                                            {
                                                'type':'div',
                                                'class': 'clearM'
                                            },
                                            {
                                                'type':'div',
                                                'title':'DNS Server',
                                                'class':'wrapDRS floatLeft'                                                         
                                            },
                                            {
                                                'type':'div',
                                                'id':'STA_DNS_SRVR_SUM',
                                                'class':'wrapDRS floatRight'                                                         
                                            },
                                        
                                            {
                                                'type':'div',
                                                'class': 'clearM'
                                            }
                                ]
                            }
                ]
            }, 
                //Profiles
                {'id':'devicerolesettingsstap2p',
                    'title':'Device Role Settings > STA/P2P Device',
                    'load':'localStorage.setItem(\'project\', ""); loadDeviceRoleSettingsSTA();',
                    'submit':'alert ("submit"); return false;',
                    'connect':true,
                    'objects':[
                        {
                            'type':'fieldset',
                            'id':'fieldsetAppProfile',
                            'title':'Add Profile',
                            'class': 'wrapSection',
                            'objects':[
                                    {
                                        'id':'SSIDDiv',
                                        'type':'div',
                                        'class': 'wrapDRS floatLeft',
                                        'objects':[
                                            {
                                                'type':'div',
                                                'class': 'fieldName',
                                                'title':'SSID'
                                            },
                                            {
                                                'id':'SSIDText',
                                                'class':'',
                                                'type':'text'
                                            
                                            }
                                        ]
                                    
                                    },
                                    {
                                        'id':'SecurityDiv',
                                        'type':'div',
                                        'class': 'wrapDRS floatRight',
                                        'objects':[
                                            {
                                                'type':'div',
                                                'class': 'fieldName',
                                                'title':'Security Key',
                                                'tooltip':'Security key info'
                                            },
                                            {
                                                'id':'SecurityText',
                                                'class':'',
                                                'type':'text'
                                            
                                            }
                                        ]
                                    
                                    },
                                    {
                                        'type':'div',
                                        'class': 'clearM'
                                    },
                                    {
                                        'id':'SecurityTypeDiv1',
                                        'type':'div',
                                        'class': 'wrapDRS floatLeft',
                                        'objects':[
                                            {
                                                'type':'div',
                                                'class': 'fieldName',
                                                'title':'Security Type'
                                            },
                                            {
                                                'id':'SecurityTypeSelect',
                                                'class':'',
                                                'type':'select',
                                                'values': [{'value':'Open','text':'Open'},{'value':'Other','text':'Other'}]
                                            }
                                        ]								
                                    },
                                    {
                                        'id':'ProfilePriorityDiv',
                                        'type':'div',
                                        'class': 'wrapDRS floatRight noMargin',
                                        'objects':[
                                            {
                                                'type':'div',
                                                'class': 'fieldName',
                                                'title':'Profile Priority'
                                            },
                                            {
                                                'id':'ProfilePriorityText',
                                                'class':'sMargin',
                                                'type':'number',
                                                'max':8,
                                                'min':0,
                                                'value':0
                                            
                                            
                                            },
                                            {
                                                'type':'div',
                                                'class': 'descSmallText',
                                                'title':'* Value between 1- 8 (8 = highest)'
                                            }
                                        ]								
                                    },
                                    {
                                        'type':'div',
                                        'class': 'clear'
                                    },
                                    {
                                        'id':'addProfileButtom',
                                        'type':'div',
                                        'class': 'regButton',
                                        'title':'Add Profile',
                                        'click':'addProfileClicked()'
                                    }
                            ]
                        },
                        {
                            'type':'fieldset',
                            'id':'ProfileListDiv',
                            'title':'Profiles List',
                            'class': 'wrapSection mbot0"',
                            'objects':[
                                {
                                    'id':'ProfileListDetailsDiv',
                                    'type':'div',
                                    'class': '',
                                    'objects':[
                                        {
                                            'id':'listNotes',
                                            'type':'div',
                                            'class': 'pLnote',
                                            'title':'No Profiles Created Yet...'
                                        }
                                    ]
                                }
                            ]
                        },
                        {
                            'type':'div',
                            'class': 'mtop75'
                        }
                    ]
                },
                //WLAN Settings
                {'id': 'devicerolesettingsstap2pwlansettings',
                'title':'Station & Wi-Fi® Direct Client Configuration',
                'load': 'localStorage.setItem(\'project\', ""); loadSTAWlanSettings(); ', //enableDisableNoPspoll();
                'submit':'alert ("submit"); return false;',
                'connect':true,
                'objects':[
                            {   
                                'type':'fieldset',
                                'id':'fieldsetSTAWlanSettings',
                                'title':'WLAN Settings',
                                'class': 'wrapSection',
                                'objects': [
                                    {
                                        'id': 'staNoPspollEnable',
                                        'class': 'c1',
                                        'type': 'checkbox',
                                        'label': 'bot 2',
                                        'values': [{ 'value': 1, 'text': '”No PS POLL” mode' }],
                                        'checked': 0,
                                    }
                                ]
                            }
                ]
            },
            //Network Settings
            {'id':'devicerolesettingsstap2pnetworksettings',
            'title':'Station & Wi-Fi® Direct Client Configuration',
            'load':'localStorage.setItem(\'project\', ""); loadSTANWSettings(); enableDisableSTAP2P();',
            'submit':'alert ("submit"); return false;',
            'connect':true,
            'objects':[
                        {   
                        'type':'fieldset',
                        'id':'fieldsetNetworkSettings',
                        'title':'Network Settings',
                        'class': 'wrapSection',
                        'objects':[
                                    {
                                        'id':'stap2pDHCPEnable',
                                        'class':'c1',
                                        'type':'checkbox',
                                        'label': 'bot 2',
                                        'values': [{'value':1,'text':'DHCP Client Enable'}],
                                        'checked':1,
                                    },
                                    {
                                    'type':'div',
                                    'class': 'clearM'
                                    },
                                    {'id':'stap2pipaddr',
                                    'type':'div',
                                    'class': 'wrapDRS floatLeft',
                                    'objects':[
                                                {
                                                    'type':'div',
                                                    'class': '',
                                                    'title':'IP Address'
                                                },
                                                {
                                                    'id':'staipAddrText',
                                                    'class':'',
                                                    'type':'text',
                                                    'value':'192.168.1.100'
                                                    
                                                }]
                                    },
                                    {
                                        'id':'stap2pipsubnet',
                                        'type':'div',
                                        'class': 'wrapDRS floatRight',
                                        'objects':[
                                                    {
                                                        'type':'div',
                                                        'class': '',
                                                        'title':'Subnet Mask'
                                                    },
                                                    {
                                                        'id':'stasubnetMaskText',
                                                        'class':'',
                                                        'type':'text',
                                                        'value':'255.255.255.0'
                                                        
                                                    }
                                                ]
                                    
                                    },
                                    {
                                    'type':'div',
                                    'class': 'clearM'
                                    },
                                    {
                                        'id':'stap2pdefaultgw',
                                        'type':'div',
                                        'class': 'wrapDRS floatLeft',
                                        //'class': '',
                                        'objects':[
                                                    {
                                                        'type':'div',
                                                        'class': '',
                                                        'title':'Default Gateway'
                                                    },
                                                    {
                                                        'id':'stadgwText',
                                                        'class':'',
                                                        'type':'text',
                                                        'value':'192.168.1.31'
                                                        
                                                    }]
                                    },
                                    {
                                        'id':'stap2pipdns',
                                        'type':'div',
                                        'class': 'wrapDRS floatRight',
                                        //'class': '',
                                        'objects':[{
                                                        'type':'div',
                                                        'class': '',
                                                        'title':'DNS Server'
                                                    },
                                                    {
                                                        'id':'stadnsText',
                                                        'class':'',
                                                        'type':'text',
                                                        'value':'192.168.1.31'
                                                        
                                                    }]
                                    
                                    },
                                    {
                                    'type':'div',
                                    'class': 'clearM'
                                    }/*,
                                    {
                                        'id':'addSTANWSettingsBtn',
                                        'type':'div',
                                        'class': 'regButton',
                                        'title':'Update',
                                        'click':'addSTANWSettings()'
                                    } */
                                    ]
                        }]
            },
        //AP/P2P Go
        {   'id':'devicerolesettingsapdevice',
            'title':'System Settings -> Device Role Settings-> AP/Wi-Fi® Direct GO',
            'load':'localStorage.setItem(\'project\', "");loadAPDirectDeviceSum();',
            'submit':'alert ("submit"); return false;',
            'connect':true,
            'objects':[
                        {   'type':'fieldset',
                            'id':'fieldsetSTADirectDeviceNWSettingsSum',
                            'title':'Network Settings',
                            'class': 'wrapSection',
                            'objects':[
                                        {
                                         'type':'div',
                                         'title':'IP Address',
                                         'class':'wrapDRS floatLeft'                                                         
                                        },
                                        {
                                         'type':'div',
                                         'id':'AP_IP_ADDRESS_SUM',
                                         'class':'wrapDRS floatRight'                                                         
                                        },
                                        
                                        {
                                         'type':'div',
                                         'class': 'clearM'
                                        },
                                                                               
                                        {
                                         'type':'div',
                                         'title':'Default Gateway',
                                         'class':'wrapDRS floatLeft'                                                         
                                        },
                                        {
                                         'type':'div',
                                         'id':'AP_DEFAULT_GW_SUM',
                                         'class':'wrapDRS floatRight'                                                         
                                        },
                                        
                                        {
                                         'type':'div',
                                         'class': 'clearM'
                                        },
                                        {
                                         'type':'div',
                                         'title':'DNS Server',
                                         'class':'wrapDRS floatLeft'                                                         
                                        },
                                        {
                                         'type':'div',
                                         'id':'AP_DNS_SRVR_SUM',
                                         'class':'wrapDRS floatRight'                                                         
                                        },
                                        
                                        {
                                         'type':'div',
                                         'class': 'clearM'
                                        },


                                                        {
                                                         'type':'div',
                                                         'title':'DHCP Start Address',
                                                         'class':'wrapDRS floatLeft'                                                         
                                                        },
                                                        {
                                                         'type':'div',
                                                         'id':'apDhcpStartAddressTextSum',
                                                         'class':'wrapDRS floatRight'                                                         
                                                        },
                                                        {
                                                         'type':'div',
                                                         'class': 'clearM'
                                                        },
                                                        {
                                                         'type':'div',
                                                         'title':'DHCP Last Address',
                                                         'class':'wrapDRS floatLeft'                                                         
                                                        },
                                                        {
                                                         'type':'div',
                                                         'id':'apDhcpLastAddressTextSum',
                                                         'class':'wrapDRS floatRight'                                                         
                                                        },
                                                        
                                                        {
                                                         'type':'div',
                                                         'class': 'clearM'
                                                        },
                                                        {
                                                         'type':'div',
                                                         'title':'DHCP Lease Time',
                                                         'class':'wrapDRS floatLeft'                                                         
                                                        },
                                                        {
                                                         'type':'div',
                                                         'id':'apDhcpLeaseTimeSum',
                                                         'class':'wrapDRS floatRight'                                                         
                                                        },
                                                        
                                                        {
                                                         'type':'div',
                                                         'class': 'clearM'
                                                        }

                                    
                                      ]
                        },
                        {
                            'type':'div',
                            'class': 'clearM'
                        },
                        /*{   'type':'fieldset',
                            'id':'fieldsetAPDirectDeviceProfileSum',
                            'title':'Black List',
                            'class': 'wrapSection',
                            'objects':[]
                        }*/

                      ]
        },    
            //WLan Settings
            {'id':'devicerolesettingsapp2pwlansettings',
            'title':'AP & Wi-Fi® Direct Group Owner WLAN Configuration',
            'load': 'localStorage.setItem(\'project\', "");loadAPWLanSettings();',
            'submit':'alert ("submit"); return false;',
            'connect':true,
            'objects':[
                        {   
                        'type':'fieldset',
                        'id':'fieldsetAPWlanSettings',
                        //'title': 'AP & Wi-Fi® Direct GO WLAN Settings',
                        'title': 'WLAN Settings',
                        'class': 'wrapSection',
                        'objects':[ 
                                    {'id':'app2pssid',
                                    'type':'div',
                                    'class': 'wrapDRS floatLeft',
                                    'objects':[
                                                {
                                                    'type':'div',
                                                    'class': 'fieldName',
                                                    'title':'SSID'
                                                },
                                                {
                                                    'id':'apSSIDText',
                                                    'class':'fieldName',
                                                    'type':'text'/*,
                                                    'value':''/*,
                                                    'change':'changeAPStaticIP(e);'*/
                                                    
                                                }   
                                                ]
                                    },
                                    {'id':'app2maxStations',
                                    'type':'div',
                                    'class': 'wrapDRS floatRight',
                                    'objects':[
                                                {
                                                    'type':'div',
                                                    'class': 'fieldName',
                                                    'title':'Max Stations'
                                                },
                                                {
                                                    'id':'apMaxStaNum',
                                                    'class':'fieldName',
                                                    'type':'number',
                                                    'max':4,
                                                    'min':1,
                                                    'value':1/*
                                                    'change':'changeAPStaticIP(e);'*/
                                                    
                                                }]
                                    },
                                    {
                                    'type':'div',
                                    'class': 'clearM'
                                    },
                                    
                                    {'id':'app2pssidhidden',
                                     'type':'div',
                                     'class': 'wrapDRS floatLeft',
                                     'objects':[
                                                {
                                                    'type':'div',
                                                    'class': 'fieldName',
                                                    'title':'Hidden SSID'
                                                },
                                                {
                                                    'id':'hiddenSSIDSelect',
                                                    'class':'',
                                                    'type':'select',
                                                    //'values': [{'value':'0','text':'Disabled'},{'value':'1','text':'Empty'},{'value':'2','text':'Clear'}],
                                                    'values': [{'value':'0','text':'Disabled'},{'value':'1','text':'Enabled'}],
                                                    'change':'changeHiddenSSID();'
                                                }
                                              ]				
                                    },
                                    {'id':'apSecurity',
                                     'type':'div',
                                     'class': 'wrapDRS floatRight',
                                     'objects':[    
                                                    {
                                                        'type':'div',
                                                        'class': 'fieldName',
                                                        'title':'Security'
                                                    },
                                                    {
                                                        'id':'apSecuritySelect',
                                                        'class':'',
                                                        'type':'select',
                                                        'values': [{'value':'0','text':'Open'},{'value':'1','text':'WEP'},{'value':'2','text':'WPAv2'}]/*,
                                                        'change':'changeHiddenSSID();'*/
                                                    }
                                                ]
                                    },
                                    {
                                    'type':'div',
                                    'class': 'clearM'
                                    },
                                    {'id':'app2pchannel',
                                     'type':'div',
                                     'class': 'wrapDRS floatLeft',
                                     'objects':[
                                                {
                                                    'type':'div',
                                                    'class': 'fieldName',
                                                    'title':'Channel'
                                                },
                                                {
                                                    'id': 'apChannelNum',
                                                    'class':'',
                                                    'type':'select',
                                                    'values': [ {'value':'1','text':'1'},
                                                                {'value':'2','text':'2'},
                                                                {'value':'3','text':'3'},
                                                                {'value':'4','text':'4'},
                                                                {'value':'5','text':'5'},
                                                                {'value':'6','text':'6'},
                                                                {'value':'7','text':'7'},
                                                                {'value':'8','text':'8'},
                                                                {'value':'9','text':'9'},
                                                                {'value':'10','text':'10'},
                                                                {'value':'11','text':'11'},
                                                                {'value':'12','text':'12'},
                                                                {'value':'13','text':'13'}],
                                                    'change': 'changeApChannelNum()'
                                                }
                                                /*{
                                                         'id':'apChannelNum',
                                                         'class':'fieldName',
                                                         'type':'number',
                                                         'max':13,
                                                         'min':1,
                                                         'value':1
                                                         
                                                },*/
                                              ]				
                                    },
                                    {'id':'app2ppw',
                                    'type':'div',
                                    'class': 'wrapDRS floatRight',
                                    'objects':[
                                                {
                                                    'type':'div',
                                                    'class': 'fieldName',
                                                    'title':'Password',
                                                    'tooltip':'Wi-Fi® password in AP role.For WEP, it is either 5 bytes or 13 bytes in ASCII format.For WPA it is 8-63 bytes'
                                                },
                                                {
                                                    'id':'apPWText',
                                                    'class':'wrapDRS',
                                                    //'type':'password',
                                                    'type':'text'
                                                    
                                                }]
                                    },                                    
                                    {
                                    'type':'div',
                                    'class': 'clearM'
                                    }
                                  ]
                        }]
            },
            //Network Settings
            {'id':'devicerolesettingsapp2pnetworksettings',
            'title':'AP & Wi-Fi® Direct Group Owner Network Configuration',
            'load':'localStorage.setItem(\'project\', ""); loadAPNWSettings(); enableDisableAPP2P();',
            'submit':'alert ("submit"); return false;',
            'connect':true,
            'objects':[
                        {   
                        'type':'fieldset',
                        'id':'fieldsetAPNetworkSettings',
                        //'title': 'AP & Wi-Fi® Direct GO Network Settings',
                        'title': 'Network Settings',
                        'class': 'wrapSection',
                        'objects':[
                                    {
                                    'type':'div',
                                    'class': 'clearM'
                                    },
                                    {'id':'app2pipaddr',
                                    'type':'div',
                                    'class': 'wrapDRS floatLeft',
                                    'objects':[
                                                {
                                                    'type':'div',
                                                    'class': '',
                                                    'title':'IP Address'
                                                },
                                                {
                                                    'id':'apipAddrText',
                                                    'class':'',
                                                    'type':'text',
                                                    'value':'10.123.45.1'/*,
                                                    'change':'changeAPStaticIP(e);'*/
                                                    
                                                }]
                                    },
                                    {'id':'app2pdg',
                                     'type':'div',
                                     'class': 'wrapDRS floatRight',
                                     'objects':[
                                                    {
                                                        'type':'div',
                                                        'class': '',
                                                        'title':'Default Gateway'
                                                    },
                                                    {
                                                        'id':'apdgText',
                                                        'class':'',
                                                        'type':'text',
                                                        'value':'10.123.45.1'
                                                        
                                                    }
                                                ]
                                    
                                    },
                                    {
                                    'type':'div',
                                    'class': 'clearM'
                                    },
                                    {'id':'apDNSServer',
                                     'type':'div',
                                     'class': 'wrapDRS floatLeft',
                                     'objects':[
                                                    {
                                                        'type':'div',
                                                        'class': '',
                                                        'title':'DNS Server'
                                                    },
                                                    {
                                                        'id':'apDNSText',
                                                        'class':'',
                                                        'type':'text',
                                                        'value':'10.123.45.1'
                                                        
                                                    }
                                                ]
                                    },
                                                                    {
                                    'type':'div',
                                    'class': 'clearM'
                                    },
                                    {
                                        'type':'fieldset',
                                        'id':'fieldsetAPNetworkSettingsDHCPServer',
                                        'title':'DHCP Server',
                                        'class': 'wrapSection',
                                        'objects':[
                                                    {   'id':'app2pdhcpstartaddr',
                                                        'type':'div',
                                                        'class': 'wrapDRS floatLeft',
                                                        'objects':[
                                                                {
                                                                    'type':'div',
                                                                    'class': '',
                                                                    'title':'Start Address'
                                                                },
                                                                {
                                                                    'id':'apDhcpStartAddressText',
                                                                    'class':'',
                                                                    'type':'text',
                                                                    'value':'10.123.45.2'
                                                                    
                                                                }]
                                                    },
                                                    {   'id':'app2pdhcplastaddr',
                                                        'type':'div',
                                                        'class': 'wrapDRS floatRight',
                                                        'objects':[
                                                                    {
                                                                        'type':'div',
                                                                        'class': '',
                                                                        'title':'Last Address'
                                                                    },
                                                                    {
                                                                        'id':'apDhcpLastAddressText',
                                                                        'class':'',
                                                                        'type':'text',
                                                                        'value':'10.123.45.254'
                                                                        
                                                                    },
                                                                ]
                                                    
                                                    },
                                                    {   'id':'app2pdhcpstartaddr',
                                                        'type':'div',
                                                        'class': 'wrapDRS floatLeft',
                                                        'objects':[
                                                                    {
                                                                        'type':'div',
                                                                        'class': 'clearM'
                                                                    },
                                                                    {
                                                                        'type':'div',
                                                                        'class': '',
                                                                        'title':'Lease Time'
                                                                    },
                                                                    
                                                                    {
                                                                        'id':'apDhcpLeaseTimeText',
                                                                        'class':'',
                                                                        'type':'text',
                                                                        'value':'24'
                                                                        
                                                                    },
                                                                    {
                                                                        'type':'div',
                                                                        'class': 'descSmallText',
                                                                        'title':'* hours'
                                                                    }]
                                                    },
                                                    
                                                  ]
                                    
                                    },
                                    {
                                    'type':'div',
                                    'class': 'clearM'
                                    }/*,
                                    {
                                        'id':'addAPNWSettingsBtn',
                                        'type':'div',
                                        'class': 'regButton',
                                        'title':'Update',
                                        'click':'addAPNWSettings()'
                                    } */
                                    ]
                        }]
            },
            //Black List
            {'id':'devicerolesettingsapblacklist',
			'title':'Device Role Settings > AP/P2P - GO',
			'load':'localStorage.setItem(\'project\', ""); loadDeviceRoleSettingsAP();',
			'submit':'alert ("submit"); return false;',
			'connect':true,
			'objects':[
				{
					'type':'fieldset',
					'id':'addAddressDiv',
					'title':'Add Address',
					'class': 'wrapSection',
					'objects':[
						{
							'id':'addressDiv',
							'type':'div',
							'class': 'fieldName',
							'title':'Address'
						},
						{
							'id':'addressText',
							'class':'',
							'type':'text'
						},
						{
							'type':'div',
							'class': 'clear'
						},
						{
							'id':'addAddressButtom',
							'type':'div',
							'class': 'regButton',
							'title':'Add Address',
							'click':'addAddressClicked()'
						}   
					]
				},
				{
					'type':'fieldset',
					'id':'denyListDiv',
					'title':'Deny List',
					'class': 'wrapSection mbot0"',
					'objects':[
						{
							'id':'denyListDetailsDiv',
							'type':'div',
							'class': '',
							'objects':[
								{
									'id':'listNotes',
									'type':'div',
									'class': 'pLnote',
									'title':'No Addresses Added Yet...'
								}
							]
						}
					]
				},
				{
					'type':'div',
					'class': 'mtop75'
				}
				
			]
		},
        //Network Applications
        {   'id':'networkapplications',
			'title':'Network Applications',
			'load':'localStorage.setItem(\'project\', "");loadNetworkApp();',
			'submit':'alert ("submit"); return false;',
			'connect':true,
			'objects':[
                        {'type':'fieldset',
                        'id':'fieldsetNetworkApplicationConfiguration',
                        /*'title':'Configuration',*/
                        'title':'Start Applications',
                        'class': 'wrapSection',
                        'objects':[ {   'type':'fieldset',
                                        'id':'fieldsetStationStartApp',
                                        'title':'Station&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;',
                                        'class': 'wrapDRS floatLeft',
                                        'objects':[
                                                    {   'id':'STA_HTTP_SID',
                                                        'class':'c1',
                                                        'type':'checkbox',
                                                        'label': 'bot 2',
                                                        'values': [{'value':1,'text':'HTTP Server'}],
                                                        'checked':1
                                                    },
                                                    /*{   'id':'STA_DHCP_SID',
                                                        'class':'c1',
                                                        'type':'checkbox',
                                                        'label': 'bot 2',
                                                        'values': [{'value':2,'text':'DHCP Client'}],
                                                        'checked':0,
                                                    },*/
                                                    {   'id':'STA_MDNS_ID',
                                                        'class':'c1',
                                                        'type':'checkbox',
                                                        'label': 'bot 2',
                                                        'values': [{'value':4,'text':'MDNS'}],
                                                        'checked':4,
                                                    },
                                                    /*{   'id':'STA_DNS_SID',
                                                        'class':'c1',
                                                        'type':'checkbox',
                                                        'label': 'bot 2',
                                                        'values': [{'value':8,'text':'DNS Client'}],
                                                        'checked':0,
                                                    },*/
                                                    {
                                                        'type':'div',
                                                        'class': 'clearM'
                                                    }/*,
                                                    {   'id':'STA_DC_ID',
                                                        'class':'c1',
                                                        'type':'checkbox',
                                                        'label': 'bot 2',
                                                        'values': [{'value':16,'text':'Device Config'}],
                                                        'checked':0,
                                                    }*/
                                                  ]

                                    },
                                    {   'type':'fieldset',
                                        'id':'fieldsetP2PCLSStartApp',
                                        'title':'Wi-Fi® Direct Client&#8196;&#8196;&#8196;&nbsp;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;',
                                        'class': 'wrapDRS floatRight',

                                        'objects':[
                                                    {   'id':'CLS_HTTP_SID',
                                                        'class':'c1',
                                                        'type':'checkbox',
                                                        'label': 'bot 2',
                                                        'values': [{'value':1,'text':'HTTP Server'}],
                                                        'checked':1,
                                                    },
                                                    /*{   'id':'CLS_DHCP_SID',
                                                        'class':'c1',
                                                        'type':'checkbox',
                                                        'label': 'bot 2',
                                                        'values': [{'value':2,'text':'DHCP Client'}],
                                                        'checked':0,
                                                    },*/
                                                    {   'id':'CLS_MDNS_ID',
                                                        'class':'c1',
                                                        'type':'checkbox',
                                                        'label': 'bot 2',
                                                        'values': [{'value':4,'text':'MDNS'}],
                                                        'checked':4,
                                                    },
                                                    /*{   'id':'CLS_DNS_SID',
                                                        'class':'c1',
                                                        'type':'checkbox',
                                                        'label': 'bot 2',
                                                        'values': [{'value':8,'text':'DNS Client'}],
                                                        'checked':0,
                                                    },*/
                                                    {
                                                        'type':'div',
                                                        'class': 'clearM'
                                                    }
                                                    /*,
                                                    {   'id':'CLS_DC_ID',
                                                        'class':'c1',
                                                        'type':'checkbox',
                                                        'label': 'bot 2',
                                                        'values': [{'value':16,'text':'Device Config'}],
                                                        'checked':0,
                                                    }*/
                                                    
                                                  ]

                                    },
                                    {
                                        'type':'div',
                                        'class': 'clearM'
                                    },
                                    {   'type':'fieldset',
                                        'id':'fieldsetAppStartApp',
                                        'title':'Access Point&#8196;&#8196;&#8196;&#8196;&nbsp;&nbsp;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&nbsp;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;',
                                        'class': ' floatLeft',
                                        'objects':[
                                                    {   'id':'AP_HTTP_SID',
                                                        'class':'c1',
                                                        'type':'checkbox',
                                                        'label': 'bot 2',
                                                        'values': [{'value':1,'text':'HTTP Server'}],
                                                        'checked':1,
                                                    },
                                                    {   'id':'AP_DHCP_SID',
                                                        'class':'c1',
                                                        'type':'checkbox',
                                                        'label': 'bot 2',
                                                        'values': [{'value':2,'text':'DHCP Server'}],
                                                        'checked':2,
                                                    },
                                                    {   'id':'AP_MDNS_ID',
                                                        'class':'c1',
                                                        'type':'checkbox',
                                                        'label': 'bot 2',
                                                        'values': [{'value':4,'text':'MDNS'}],
                                                        'checked':4,
                                                    },
                                                    {   'id':'AP_DNS_SID',
                                                        'class':'c1',
                                                        'type':'checkbox',
                                                        'label': 'bot 2',
                                                        'values': [{'value':8,'text':'DNS Server'}],
                                                        'checked':8,
                                                    }/*,
                                                    {   'id':'AP_DC_ID',
                                                        'class':'c1',
                                                        'type':'checkbox',
                                                        'label': 'bot 2',
                                                        'values': [{'value':16,'text':'Device Config'}],
                                                        'checked':0,
                                                    }*/
                                                  ]

                                    },
                                    
                                    {   'type':'fieldset',
                                        'id':'fieldsetP2PGOStartApp',
                                        'title':'Wi-Fi® Direct GO&nbsp;&#8196;&#8196;&#8196;&#8196;&#8196;&nbsp;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;',
                                        'class': 'wrapDRS floatRight',
                                        'objects':[
                                                    {   'id':'GO_HTTP_SID',
                                                        'class':'c1',
                                                        'type':'checkbox',
                                                        'label': 'bot 2',
                                                        'values': [{'value':1,'text':'HTTP Server'}],
                                                        'checked':1,
                                                    },
                                                    {   'id':'GO_DHCP_SID',
                                                        'class':'c1',
                                                        'type':'checkbox',
                                                        'label': 'bot 2',
                                                        'values': [{'value':2,'text':'DHCP Server'}],
                                                        'checked':2,
                                                    },
                                                    {   'id':'GO_MDNS_ID',
                                                        'class':'c1',
                                                        'type':'checkbox',
                                                        'label': 'bot 2',
                                                        'values': [{'value':4,'text':'MDNS'}],
                                                        'checked':4,
                                                    },
                                                    {   'id':'GO_DNS_SID',
                                                        'class':'c1',
                                                        'type':'checkbox',
                                                        'label': 'bot 2',
                                                        'values': [{'value':8,'text':'DNS Server'}],
                                                        'checked':8,
                                                    }/*,
                                                    {   'id':'GO_DC_ID',
                                                        'class':'c1',
                                                        'type':'checkbox',
                                                        'label': 'bot 2',
                                                        'values': [{'value':16,'text':'Device Config'}],
                                                        'checked':0,
                                                    }*/
                                                  ]

                                    },
                                    {
                                        'type':'div',
                                        'class': 'clearM'
                                    }/*,
                                    {
                                        'id':'addNetworkAppBtn',
                                        'type':'div',
                                        'class': 'regButton',
                                        'title':'Update',
                                        'click':'addNetworkApp()'
                                    }*/
                                  ]
                        }
                      ]
        },
        {
            'id': 'networkapplications',
            'title': 'Network Applications',
            'load': 'localStorage.setItem(\'project\', "");loadNetworkApp();',
            'submit': 'alert ("submit"); return false;',
            'connect': true,
            'objects': [
                        {
                            'type': 'fieldset',
                            'id': 'fieldsetNetworkApplicationConfiguration',
                            /*'title':'Configuration',*/
                            'title': 'Start Applications',
                            'class': 'wrapSection',
                            'objects': [{
                                'type': 'fieldset',
                                'id': 'fieldsetStationStartApp',
                                'title': 'Station&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;',
                                'class': 'wrapDRS floatLeft',
                                'objects': [
                                            {
                                                'id': 'STA_HTTP_SID',
                                                'class': 'c1',
                                                'type': 'checkbox',
                                                'label': 'bot 2',
                                                'values': [{ 'value': 1, 'text': 'HTTP Server' }],
                                                'checked': 1
                                            },
                                            /*{   'id':'STA_DHCP_SID',
                                                'class':'c1',
                                                'type':'checkbox',
                                                'label': 'bot 2',
                                                'values': [{'value':2,'text':'DHCP Client'}],
                                                'checked':0,
                                            },*/
                                            {
                                                'id': 'STA_MDNS_ID',
                                                'class': 'c1',
                                                'type': 'checkbox',
                                                'label': 'bot 2',
                                                'values': [{ 'value': 4, 'text': 'MDNS' }],
                                                'checked': 4,
                                            },
                                            /*{   'id':'STA_DNS_SID',
                                                'class':'c1',
                                                'type':'checkbox',
                                                'label': 'bot 2',
                                                'values': [{'value':8,'text':'DNS Client'}],
                                                'checked':0,
                                            },*/
                                            {
                                                'type': 'div',
                                                'class': 'clearM'
                                            }/*,
                                                    {   'id':'STA_DC_ID',
                                                        'class':'c1',
                                                        'type':'checkbox',
                                                        'label': 'bot 2',
                                                        'values': [{'value':16,'text':'Device Config'}],
                                                        'checked':0,
                                                    }*/
                                ]

                            },
                                        {
                                            'type': 'fieldset',
                                            'id': 'fieldsetP2PCLSStartApp',
                                            'title': 'Wi-Fi® Direct Client&#8196;&#8196;&#8196;&nbsp;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;',
                                            'class': 'wrapDRS floatRight',

                                            'objects': [
                                                        {
                                                            'id': 'CLS_HTTP_SID',
                                                            'class': 'c1',
                                                            'type': 'checkbox',
                                                            'label': 'bot 2',
                                                            'values': [{ 'value': 1, 'text': 'HTTP Server' }],
                                                            'checked': 1,
                                                        },
                                                        /*{   'id':'CLS_DHCP_SID',
                                                            'class':'c1',
                                                            'type':'checkbox',
                                                            'label': 'bot 2',
                                                            'values': [{'value':2,'text':'DHCP Client'}],
                                                            'checked':0,
                                                        },*/
                                                        {
                                                            'id': 'CLS_MDNS_ID',
                                                            'class': 'c1',
                                                            'type': 'checkbox',
                                                            'label': 'bot 2',
                                                            'values': [{ 'value': 4, 'text': 'MDNS' }],
                                                            'checked': 4,
                                                        },
                                                        /*{   'id':'CLS_DNS_SID',
                                                            'class':'c1',
                                                            'type':'checkbox',
                                                            'label': 'bot 2',
                                                            'values': [{'value':8,'text':'DNS Client'}],
                                                            'checked':0,
                                                        },*/
                                                        {
                                                            'type': 'div',
                                                            'class': 'clearM'
                                                        }
                                                        /*,
                                                        {   'id':'CLS_DC_ID',
                                                            'class':'c1',
                                                            'type':'checkbox',
                                                            'label': 'bot 2',
                                                            'values': [{'value':16,'text':'Device Config'}],
                                                            'checked':0,
                                                        }*/

                                            ]

                                        },
                                        {
                                            'type': 'div',
                                            'class': 'clearM'
                                        },
                                        {
                                            'type': 'fieldset',
                                            'id': 'fieldsetAppStartApp',
                                            'title': 'Access Point&#8196;&#8196;&#8196;&#8196;&nbsp;&nbsp;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&nbsp;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;',
                                            'class': ' floatLeft',
                                            'objects': [
                                                        {
                                                            'id': 'AP_HTTP_SID',
                                                            'class': 'c1',
                                                            'type': 'checkbox',
                                                            'label': 'bot 2',
                                                            'values': [{ 'value': 1, 'text': 'HTTP Server' }],
                                                            'checked': 1,
                                                        },
                                                        {
                                                            'id': 'AP_DHCP_SID',
                                                            'class': 'c1',
                                                            'type': 'checkbox',
                                                            'label': 'bot 2',
                                                            'values': [{ 'value': 2, 'text': 'DHCP Server' }],
                                                            'checked': 2,
                                                        },
                                                        {
                                                            'id': 'AP_MDNS_ID',
                                                            'class': 'c1',
                                                            'type': 'checkbox',
                                                            'label': 'bot 2',
                                                            'values': [{ 'value': 4, 'text': 'MDNS' }],
                                                            'checked': 4,
                                                        },
                                                        {
                                                            'id': 'AP_DNS_SID',
                                                            'class': 'c1',
                                                            'type': 'checkbox',
                                                            'label': 'bot 2',
                                                            'values': [{ 'value': 8, 'text': 'DNS Server' }],
                                                            'checked': 8,
                                                        }/*,
                                                    {   'id':'AP_DC_ID',
                                                        'class':'c1',
                                                        'type':'checkbox',
                                                        'label': 'bot 2',
                                                        'values': [{'value':16,'text':'Device Config'}],
                                                        'checked':0,
                                                    }*/
                                            ]

                                        },

                                        {
                                            'type': 'fieldset',
                                            'id': 'fieldsetP2PGOStartApp',
                                            'title': 'Wi-Fi® Direct GO&nbsp;&#8196;&#8196;&#8196;&#8196;&#8196;&nbsp;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;&#8196;',
                                            'class': 'wrapDRS floatRight',
                                            'objects': [
                                                        {
                                                            'id': 'GO_HTTP_SID',
                                                            'class': 'c1',
                                                            'type': 'checkbox',
                                                            'label': 'bot 2',
                                                            'values': [{ 'value': 1, 'text': 'HTTP Server' }],
                                                            'checked': 1,
                                                        },
                                                        {
                                                            'id': 'GO_DHCP_SID',
                                                            'class': 'c1',
                                                            'type': 'checkbox',
                                                            'label': 'bot 2',
                                                            'values': [{ 'value': 2, 'text': 'DHCP Server' }],
                                                            'checked': 2,
                                                        },
                                                        {
                                                            'id': 'GO_MDNS_ID',
                                                            'class': 'c1',
                                                            'type': 'checkbox',
                                                            'label': 'bot 2',
                                                            'values': [{ 'value': 4, 'text': 'MDNS' }],
                                                            'checked': 4,
                                                        },
                                                        {
                                                            'id': 'GO_DNS_SID',
                                                            'class': 'c1',
                                                            'type': 'checkbox',
                                                            'label': 'bot 2',
                                                            'values': [{ 'value': 8, 'text': 'DNS Server' }],
                                                            'checked': 8,
                                                        }/*,
                                                    {   'id':'GO_DC_ID',
                                                        'class':'c1',
                                                        'type':'checkbox',
                                                        'label': 'bot 2',
                                                        'values': [{'value':16,'text':'Device Config'}],
                                                        'checked':0,
                                                    }*/
                                            ]

                                        },
                                        {
                                            'type': 'div',
                                            'class': 'clearM'
                                        }/*,
                                    {
                                        'id':'addNetworkAppBtn',
                                        'type':'div',
                                        'class': 'regButton',
                                        'title':'Update',
                                        'click':'addNetworkApp()'
                                    }*/
                            ]
                        }
            ]
        },
//Regulatory Domain 24
                {
                    'id': 'regulatorydomain24',
                    'title': 'Advanced RF 2.4G Settings',
                    'load': 'localStorage.setItem(\'project\', ""); loadRegulatoryDomain24();',
                    'submit': 'alert ("submit"); return false;',
                    'connect': true,
                    'objects': [
                                {
                                    'type': 'fieldset',
                                    'id': 'fieldsetRegDomain24',
                                    'title': 'Regulatory Domain.',
                                    'class': 'wrapSection',
                                    'objects': [
                                                {
                                                    'id': 'regdom24FccBoTable',
                                                    'type': 'div',
                                                    'class': 'floatLeft padright10',
                                                    'objects': [
                                                                {
                                                                    'id': 'regdom24cbFCCBO',
                                                                    'class': 'mleft85',
                                                                    'type': 'checkbox',
                                                                    'values': [{ 'value': 1, 'text': 'FCC BO Offset [dB]  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;' }],
                                                                    'checked': 0
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 1&nbsp;&#8196',
                                                                    'class': 'floatLeft padright10 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC11B_channel_1',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_FCC11B_Ch1',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'value': '0'

                                                                    }]
                                                                }, 
                                                                {
                                                                    'id': 'regdomFCCBO_channel_1L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCCBO_Ch1L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_1H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                                    'id': 'rd_FCCBO_Ch1H',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 1*/
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 2&nbsp;&#8196',
                                                                    'class': 'floatLeft padright10 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC11B_channel_2',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC11B_Ch2',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_2L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_FCCBO_Ch2L',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'max': 6,
                                                                        'min': -6,
                                                                        'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_2H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCCBO_Ch2H',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel2*/
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 3&nbsp;&#8196',
                                                                    'class': 'floatLeft padright10 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC11B_channel_3',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC11B_Ch3',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_3L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCCBO_Ch3L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_3H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_FCCBO_Ch3H',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'max': 6,
                                                                        'min': -6,
                                                                        'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel3*/
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 4&nbsp;&#8196',
                                                                    'class': 'floatLeft padright10 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC11B_channel_4',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC11B_Ch4',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },                                                  
                                                                {
                                                                    'id': 'regdomFCCBO_channel_4L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCCBO_Ch4L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_4H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_FCCBO_Ch4H',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'max': 6,
                                                                        'min':-6,
                                                                        'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel4*/
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 5&nbsp;&#8196',
                                                                    'class': 'floatLeft padright10 padtop5'
                                                                },
                                                                {
                                                                     'id': 'regdomFCC11B_channel_5',
                                                                     'type': 'div',
                                                                     'class': 'floatLeft  padright2',
                                                                     'title': '11b&nbsp',
                                                                     'objects': [
                                                                                 {
                                                                                     'id': 'rd_FCC11B_Ch5',
                                                                                     'class': 'wrapDRSmall24',
                                                                                     'type': 'snumber',
                                                                                     'value': '0'

                                                                                 }]
                                                                 },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_5L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCCBO_Ch5L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_5H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_FCCBO_Ch5H',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'max': 6,
                                                                        'min':-6,
                                                                        'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 5*/
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 6&nbsp;&#8196',
                                                                    'class': 'floatLeft padright10 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC11B_channel_6',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC11B_Ch6',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_6L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCCBO_Ch6L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_6H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_FCCBO_Ch6H',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'max': 6,
                                                                        'min': -6,
                                                                        'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                },/*Channel 6*/
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 7&nbsp;&#8196',
                                                                    'class': 'floatLeft padright10 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC11B_channel_7',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC11B_Ch7',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_7L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCCBO_Ch7L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_7H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_FCCBO_Ch7H',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'max': 6,
                                                                        'min': -6,
                                                                        'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 7*/
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 8&nbsp;&#8196',
                                                                    'class': 'floatLeft padright10 padtop5'
                                                                },
                                                                {
                                                                   'id': 'regdomFCC11B_channel_8',
                                                                   'type': 'div',
                                                                   'class': 'floatLeft  padright2',
                                                                   'title': '11b&nbsp',
                                                                   'objects': [
                                                                               {
                                                                                   'id': 'rd_FCC11B_Ch8',
                                                                                   'class': 'wrapDRSmall24',
                                                                                   'type': 'snumber',
                                                                                   'value': '0'

                                                                               }]
                                                               },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_8L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCCBO_Ch8L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_8H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                                    'id': 'rd_FCCBO_Ch8H',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 8*/
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 9&nbsp;&#8196',
                                                                    'class': 'floatLeft padright10 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC11B_channel_9',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC11B_Ch9',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_9L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCCBO_Ch9L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_9H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                                    'id': 'rd_FCCBO_Ch9H',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 9*/
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 10&nbsp',
                                                                    'class': 'floatLeft padright8 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC11B_channel_10',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC11B_Ch10',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_10L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCCBO_Ch10L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_10H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                                    'id': 'rd_FCCBO_Ch10H',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 10*/
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 11&nbsp',
                                                                    'class': 'floatLeft padright8 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC11B_channel_11',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC11B_Ch11',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_11L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCCBO_Ch11L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': 0,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_11H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_FCCBO_Ch11H',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'max': 6,
                                                                        'min': 0,
                                                                        'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                },/*Channel 11*/
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 12&nbsp',
                                                                    'class': 'floatLeft padright8 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC11B_channel_12',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC11B_Ch12',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_12L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCCBO_Ch12L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_12H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                                    'id': 'rd_FCCBO_Ch12H',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 12*/
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 13&nbsp',
                                                                    'class': 'floatLeft padright8 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC11B_channel_13',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC11B_Ch13',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_13L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCCBO_Ch13L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_13H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                                    'id': 'rd_FCCBO_Ch13H',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 13*
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 14&nbsp',
                                                                    'class': 'floatLeft padright8 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC11B_channel_14',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC11B_Ch14',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_14L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCCBO_Ch14L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomFCCBO_channel_14H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_FCCBO_Ch14H',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'max': 6,
                                                                        'min': -6,
                                                                        'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                }/*Channel 14*/
                                                    ]
                                                },/*FCC*/
                                                {
                                                    'id': 'regdom24ETSITable',
                                                    'type': 'div',
                                                    'class': 'floatLeft padright10',
                                                    'objects': [
                                                                {
                                                                    'id': 'regdom24cbETSIBO',
                                                                    'class': 'mleft10',
                                                                    'type': 'checkbox',
                                                                    'values': [{ 'value': 1, 'text': 'ETSI BO Offset [dB]  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;' }],
                                                                    'checked': 0
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomETSI11B_channel_1',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_ETSI11B_Ch1',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'value': '0'

                                                                    }]
                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_1L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETSIBO_Ch1L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_1H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_ETSIBO_Ch1H',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'max': 6,
                                                                        'min': -6,
                                                                        'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 1*/
                                                                {
                                                                    'id': 'regdomETSI11B_channel_2',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETSI11B_Ch2',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_2L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETSIBO_Ch2L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_2H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                                    'id': 'rd_ETSIBO_Ch2H',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 2*/
                                                                {
                                                                    'id': 'regdomETSI11B_channel_3',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETSI11B_Ch3',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_3L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETSIBO_Ch3L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_3H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_ETSIBO_Ch3H',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'max': 6,
                                                                        'min': -6,
                                                                        'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 3*/
                                                                {
                                                                    'id': 'regdomETSI11B_channel_4',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETSI11B_Ch4',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_4L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETSIBO_Ch4L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_4H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                                    'id': 'rd_ETSIBO_Ch4H',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 4*/
                                                                {
                                                                     'id': 'regdomETSI11B_channel_5',
                                                                     'type': 'div',
                                                                     'class': 'floatLeft  padright2',
                                                                     'title': '11b&nbsp',
                                                                     'objects': [
                                                                                 {
                                                                                     'id': 'rd_ETSI11B_Ch5',
                                                                                     'class': 'wrapDRSmall24',
                                                                                     'type': 'snumber',
                                                                                     'value': '0'

                                                                                 }]
                                                                 },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_5L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETSIBO_Ch5L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_5H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                                    'id': 'rd_ETSIBO_Ch5H',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 5*/
                                                                {
                                                                    'id': 'regdomETSI11B_channel_6',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETSI11B_Ch6',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_6L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETSIBO_Ch6L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_6H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_ETSIBO_Ch6H',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'max': 6,
                                                                        'min': -6,
                                                                        'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                },/*Channel 6*/
                                                                {
                                                                    'id': 'regdomETSI11B_channel_7',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETSI11B_Ch7',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_7L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETSIBO_Ch7L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_7H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                                    'id': 'rd_ETSIBO_Ch7H',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 7*/
                                                                {
                                                                   'id': 'regdomETSI11B_channel_8',
                                                                   'type': 'div',
                                                                   'class': 'floatLeft  padright2',
                                                                   'title': '11b&nbsp',
                                                                   'objects': [
                                                                               {
                                                                                   'id': 'rd_ETSI11B_Ch8',
                                                                                   'class': 'wrapDRSmall24',
                                                                                   'type': 'snumber',
                                                                                   'value': '0'

                                                                               }]
                                                               },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_8L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETSIBO_Ch8L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_8H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_ETSIBO_Ch8H',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'max': 6,
                                                                        'min': -6,
                                                                        'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 8*/
                                                                {
                                                                    'id': 'regdomETSI11B_channel_9',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETSI11B_Ch9',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_9L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETSIBO_Ch9L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_9H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_ETSIBO_Ch9H',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'max': 6,
                                                                        'min': -6,
                                                                        'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 9*/
                                                                {
                                                                    'id': 'regdomETSI11B_channel_10',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETSI11B_Ch10',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_10L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETSIBO_Ch10L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_10H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_ETSIBO_Ch10H',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'max': 6,
                                                                        'min': -6,
                                                                        'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 10*/
                                                                {
                                                                    'id': 'regdomETSI11B_channel_11',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETSI11B_Ch11',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_11L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETSIBO_Ch11L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_11H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_ETSIBO_Ch11H',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'max': 6,
                                                                        'min': -6,
                                                                        'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                },/*Channel 11*/
                                                                {
                                                                    'id': 'regdomETSI11B_channel_12',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETSI11B_Ch12',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_12L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETSIBO_Ch12L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_12H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                                    'id': 'rd_ETSIBO_Ch12H',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 12*/
                                                                {
                                                                    'id': 'regdomETSI11B_channel_13',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETSI11B_Ch13',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_13L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETSIBO_Ch13L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_13H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_ETSIBO_Ch13H',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'max': 6,
                                                                        'min': -6,
                                                                        'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 13*
                                                                {
                                                                    'id': 'regdomETSI11B_channel_14',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETSI11B_Ch14',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_14L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETSIBO_Ch14L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomETSIBO_channel_14H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                                    'id': 'rd_ETSIBO_Ch14H',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                }/*Channel 14*/
                                                    ]
                                                },/*ETSI*/
                                                {
                                                    'id': 'regdom24JPTable',
                                                    'type': 'div',
                                                    'class': 'floatLeft padright10',
                                                    'objects': [
                                                                {
                                                                    'id': 'regdom24cbJPBO',
                                                                    'class': 'mleft10',
                                                                    'type': 'checkbox',
                                                                    'values': [{ 'value': 1, 'text': 'JP BO Offset [dB]  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;' }],
                                                                    'checked': 0
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomJP11B_channel_1',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_JP11B_Ch1',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'value': '0'

                                                                    }]
                                                                },
                                                                {
                                                                    'id': 'regdomJPBO_channel_1L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JPBO_Ch1L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomJPBO_channel_1H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_JPBO_Ch1H',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'max': 6,
                                                                        'min': -6,
                                                                        'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 1*/
                                                                {
                                                                    'id': 'regdomJP11B_channel_2',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP11B_Ch2',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomJPBO_channel_2L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JPBO_Ch2L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomJPBO_channel_2H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_JPBO_Ch2H',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'max': 6,
                                                                        'min': -6,
                                                                        'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 2*/
                                                                {
                                                                    'id': 'regdomJP11B_channel_3',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP11B_Ch3',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomJPBO_channel_3L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JPBO_Ch3L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomJPBO_channel_3H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_JPBO_Ch3H',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'max': 6,
                                                                        'min': -6,
                                                                        'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 3*/
                                                                {
                                                                    'id': 'regdomJP11B_channel_4',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP11B_Ch4',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomJPBO_channel_4L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JPBO_Ch4L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomJPBO_channel_4H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_JPBO_Ch4H',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'max': 6,
                                                                        'min': -6,
                                                                        'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 4*/
                                                                {
                                                                     'id': 'regdomJP11B_channel_5',
                                                                     'type': 'div',
                                                                     'class': 'floatLeft  padright2',
                                                                     'title': '11b&nbsp',
                                                                     'objects': [
                                                                                 {
                                                                                     'id': 'rd_JP11B_Ch5',
                                                                                     'class': 'wrapDRSmall24',
                                                                                     'type': 'snumber',
                                                                                     'value': '0'

                                                                                 }]
                                                                 },
                                                                {
                                                                    'id': 'regdomJPBO_channel_5L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JPBO_Ch5L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomJPBO_channel_5H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_JPBO_Ch5H',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'max': 6,
                                                                        'min': -6,
                                                                        'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 5*/
                                                                {
                                                                    'id': 'regdomJP11B_channel_6',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP11B_Ch6',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomJPBO_channel_6L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JPBO_Ch6L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomJPBO_channel_6H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_JPBO_Ch6H',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'max': 6,
                                                                        'min': -6,
                                                                        'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                },/*Channel 6*/
                                                                {
                                                                    'id': 'regdomJP11B_channel_7',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP11B_Ch7',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomJPBO_channel_7L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JPBO_Ch7L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomJPBO_channel_7H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_JPBO_Ch7H',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'max': 6,
                                                                        'min': -6,
                                                                        'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 7*/
                                                                {
                                                                   'id': 'regdomJP11B_channel_8',
                                                                   'type': 'div',
                                                                   'class': 'floatLeft  padright2',
                                                                   'title': '11b&nbsp',
                                                                   'objects': [
                                                                               {
                                                                                   'id': 'rd_JP11B_Ch8',
                                                                                   'class': 'wrapDRSmall24',
                                                                                   'type': 'snumber',
                                                                                   'value': '0'

                                                                               }]
                                                               },
                                                                {
                                                                    'id': 'regdomJPBO_channel_8L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JPBO_Ch8L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomJPBO_channel_8H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_JPBO_Ch8H',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'max': 6,
                                                                        'min': -6,
                                                                        'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 8*/
                                                                {
                                                                    'id': 'regdomJP11B_channel_9',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP11B_Ch9',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomJPBO_channel_9L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JPBO_Ch9L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomJPBO_channel_9H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                                    'id': 'rd_JPBO_Ch9H',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 9*/
                                                                {
                                                                    'id': 'regdomJP11B_channel_10',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP11B_Ch10',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomJPBO_channel_10L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JPBO_Ch10L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomJPBO_channel_10H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                                        'id': 'rd_JPBO_Ch10H',
                                                                                        'class': 'wrapDRSmall24',
                                                                                        'type': 'snumber',
                                                                                        'max': 6,
                                                                                        'min': -6,
                                                                                        'value': '0'
                                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 10*/
                                                                {
                                                                    'id': 'regdomJP11B_channel_11',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP11B_Ch11',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomJPBO_channel_11L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JPBO_Ch11L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomJPBO_channel_11H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_JPBO_Ch11H',
                                                                        'class': 'wrapDRSmall24',
                                                                        'type': 'snumber',
                                                                        'max': 6,
                                                                        'min': -6,
                                                                        'value': '0'
                                                                    }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                },/*Channel 11*/
                                                                {
                                                                    'id': 'regdomJP11B_channel_12',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP11B_Ch12',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomJPBO_channel_12L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JPBO_Ch12L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomJPBO_channel_12H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                                    'id': 'rd_JPBO_Ch12H',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 12*/
                                                                {
                                                                    'id': 'regdomJP11B_channel_13',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP11B_Ch13',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomJPBO_channel_13L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JPBO_Ch13L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomJPBO_channel_13H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                                    'id': 'rd_JPBO_Ch13H',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },/*Channel 13*
                                                                {
                                                                    'id': 'regdomJP11B_channel_14',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft  padright2',
                                                                    'title': '11b&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP11B_Ch14',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },    
                                                                {
                                                                    'id': 'regdomJPBO_channel_14L',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JPBO_Ch14L',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'id': 'regdomJPBO_channel_14H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                                    'id': 'rd_JPBO_Ch14H',
                                                                                    'class': 'wrapDRSmall24',
                                                                                    'type': 'snumber',
                                                                                    'max': 6,
                                                                                    'min': -6,
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                }/*Channel 14*/
                                                    ]
                                                },/*JP*/
                                                {
                                                    'type': 'div',
                                                    'class': 'clearS'
                                                },/*clearS*/
                                                {
                                                    'id': 'backRegDomain',
                                                    'type': 'div',
                                                    'class': 'regButton mbot25 floatLeft',
                                                    'title': '<< &nbsp; &nbsp; Back',
                                                    'click': 'loadRFSettings();'
                                                },/*Back Button*/
                                                {
                                                    'id': 'createProject',
                                                    'type': 'div',
                                                    'class': 'regButton floatRight',
                                                    'title': 'Done &nbsp; &nbsp;',
                                                    'click': 'loadRFSettings();'
                                                },/*Done Button*/
                                    ]
                                }/*fieldsetRegDomain*/

                    ]
                },
 //Regulatory Domain 5G
                {
                    'id': 'regulatorydomain',
                    'title': 'Advanced RF 5G Settings',
                    'load': 'localStorage.setItem(\'project\', ""); loadRegulatoryDomain();',
                    'submit': 'alert ("submit"); return false;',
                    'connect': true,
                    'objects': [
                                {
                                    'type': 'fieldset',
                                    'id': 'fieldsetRegDomain',
                                    'title': 'Regulatory Domain.',
                                    'class': 'wrapSection',
                                    'objects': [

                                                {
                                                    'id': 'regdomFccTable',
                                                    'type': 'div',
                                                    'class': 'floatLeft padright10',
                                                    'objects': [
                                                                {
                                                                    'id': 'regdomcbFCC',
                                                                    'class': 'mleft85',
                                                                    'type': 'checkbox',
                                                                    'values': [{ 'value': 1, 'text': 'FCC' }],
                                                                    'checked': 0
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'floatRight descSmallText10',
                                                                    'title': '[dBm]'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 36&nbsp',
                                                                    'class': 'floatLeft padright10 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_36',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch36',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 40&nbsp',
                                                                    'class': 'floatLeft padright10 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_40',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch40',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 44&nbsp',
                                                                    'class': 'floatLeft padright10 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_44',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch44',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 48&nbsp',
                                                                    'class': 'floatLeft padright10 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_48',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch48',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 52&nbsp',
                                                                    'class': 'floatLeft padright10 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_52',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch52',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 56&nbsp',
                                                                    'class': 'floatLeft padright10 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_56',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch56',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 60&nbsp',
                                                                    'class': 'floatLeft padright10 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_60',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch60',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 64&nbsp',
                                                                    'class': 'floatLeft padright10 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_64',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch64',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 100',
                                                                    'class': 'floatLeft padright8 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_100',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch100',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 104',
                                                                    'class': 'floatLeft padright8 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_104',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch104',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 108',
                                                                    'class': 'floatLeft padright8 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_108',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch108',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 112',
                                                                    'class': 'floatLeft padright8 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_112',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch112',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 116',
                                                                    'class': 'floatLeft padright8 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_116',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch116',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 120',
                                                                    'class': 'floatLeft padright8 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_120',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch120',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 124',
                                                                    'class': 'floatLeft padright8 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_124',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch124',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 128',
                                                                    'class': 'floatLeft padright8 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_128',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch128',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 132',
                                                                    'class': 'floatLeft padright8 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_132',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch132',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 136',
                                                                    'class': 'floatLeft padright8 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_136',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch136',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 140',
                                                                    'class': 'floatLeft padright8 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_140',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch140',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 144',
                                                                    'class': 'floatLeft padright8 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_144',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch144',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 149',
                                                                    'class': 'floatLeft padright8 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_149',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch149',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 153',
                                                                    'class': 'floatLeft padright8 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_153',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch153',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 157',
                                                                    'class': 'floatLeft padright8 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_157',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch157',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 161',
                                                                    'class': 'floatLeft padright8 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_161',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch161',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'Channel 165',
                                                                    'class': 'floatLeft padright8 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_165',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch165',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                /*{
                                                                    'type': 'div',
                                                                    'title': 'Channel 169&nbsp',
                                                                    'class': 'floatLeft padright8 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomFCC_channel_169',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_FCC_Ch169',
                                                                                    'class': 'wrapDRSmall floatLeft',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },*/
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                }
                                                    ]
                                                },/*FCC*/
                                                {
                                                    'id': 'regdomETCITable',
                                                    'type': 'div',
                                                    'class': 'floatLeft padleft50',
                                                    'objects': [
                                                                {
                                                                    'id': 'regdomcbETCI',
                                                                    'class': 'padright10',
                                                                    'type': 'checkbox',
                                                                    //'label1': 'ETSI[dBm]',
                                                                    'values': [{ 'value': 1, 'text': 'ETSI' }],
                                                                    'checked': 0
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'floatRight descSmallText10',
                                                                    'title': '[dBm]'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomETCI_channel_36',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch36',
                                                                                    'class': 'wrapDRSmall floatLeft padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomETCI_channel_40',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch40',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomETCI_channel_44',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch44',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomETCI_channel_48',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch48',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                },
                                                                {
                                                                    'id': 'regdomETCI_channel_52',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch52',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomETCI_channel_56',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch56',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomETCI_channel_60',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch60',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomETCI_channel_64',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch64',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                },
                                                                {
                                                                    'id': 'regdomETCI_channel_100',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch100',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomETCI_channel_104',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch104',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomETCI_channel_108',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch108',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomETCI_channel_112',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch112',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomETCI_channel_116',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch116',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                },
                                                                {
                                                                    'id': 'regdomETCI_channel_120',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch120',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomETCI_channel_124',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch124',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomETCI_channel_128',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch128',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomETCI_channel_132',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch132',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomETCI_channel_136',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch136',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomETCI_channel_140',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch140',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomETCI_channel_144',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch144',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                },
                                                                {
                                                                    'id': 'regdomETCI_channel_149',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch149',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomETCI_channel_153',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch153',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomETCI_channel_157',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch157',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomETCI_channel_161',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch161',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomETCI_channel_165',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch165',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                /*{
                                                                    'id': 'regdomETCI_channel_169',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_ETCI_Ch169',
                                                                                    'class': 'wrapDRSmall floatLeft',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]
                                                                },*/
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                }
                                                    ]
                                                },/*ETSI*/
                                                {
                                                    'id': 'regdomJPTable',
                                                    'type': 'div',
                                                    'class': 'floatLeft padleft50',
                                                    'objects': [
                                                                {
                                                                    'id': 'regdomcbJP',
                                                                    'class': '',
                                                                    'type': 'checkbox',
                                                                    //'label1': 'JP[dBm]',
                                                                    'values': [{ 'value': 1, 'text': 'JP' }],
                                                                    'checked': 0
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'floatRight descSmallText10',
                                                                    'title': '[dBm]'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomJP_channel_36',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch36',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomJP_channel_40',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch40',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomJP_channel_44',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch44',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomJP_channel_48',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch48',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                },
                                                                {
                                                                    'id': 'regdomJP_channel_52',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch52',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomJP_channel_56',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch56',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomJP_channel_60',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch60',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomJP_channel_64',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch64',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                },
                                                                {
                                                                    'id': 'regdomJP_channel_100',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch100',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomJP_channel_104',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch104',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomJP_channel_108',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch108',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomJP_channel_112',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch112',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomJP_channel_116',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch116',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                },
                                                                {
                                                                    'id': 'regdomJP_channel_120',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch120',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomJP_channel_124',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch124',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomJP_channel_128',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch128',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomJP_channel_132',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch132',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomJP_channel_136',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch136',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomJP_channel_140',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch140',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomJP_channel_144',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch144',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                },
                                                                {
                                                                    'id': 'regdomJP_channel_149',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch149',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomJP_channel_1553',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch153',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomJP_channel_157',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch157',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomJP_channel_161',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch161',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomJP_channel_165',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch165',
                                                                                    'class': 'wrapDRSmall floatLeft  padright5',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                /*{
                                                                    'id': 'regdomJP_channel_169',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_JP_Ch169',
                                                                                    'class': 'wrapDRSmall floatLeft',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },*/
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                }
                                                    ]
                                                },/*JP*/
                                                {
                                                    'id': 'regdomBOTable',
                                                    'type': 'div',
                                                    'class': 'floatLeft padleft50',
                                                    'objects': [
                                                                {
                                                                    'id': 'regdomcbBO',
                                                                    'class': 'mleft10',
                                                                    'type': 'checkbox',
                                                                    //'label1': 'Extra BO[dB]',
                                                                    'values': [{ 'value': 1, 'text': 'Extra BO' }],
                                                                    'checked': 0
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'padleft50 descSmallText10',
                                                                    'title': '[dB]'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_36H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_BO_Ch36H',
                                                                        'class': 'wrapDRSmall',
                                                                        'type': 'snumber',
                                                                        //'type': 'number',
                                                                        'max': 6,
                                                                        'min': 0,
                                                                        'value': '0'
                                                                    }]

                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_36L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch36L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    //'type': 'number',
                                                                                    'max': 6,
                                                                                    'min': 0,
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_40H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch40H',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_40L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch40L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_44H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch44H',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_44L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch44L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_48H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch48H',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_48L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch48L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_52H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch52H',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_52L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch52L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_56H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch56H',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_56L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch56L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_60H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch60H',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_60L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch60L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_64H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch64H',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_64L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch64L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_100H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_BO_Ch100H',
                                                                        'class': 'wrapDRSmall',
                                                                        'type': 'snumber',
                                                                        'value': '0'
                                                                    }]

                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_100L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch100L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_104H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch104H',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_104L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch104L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_108H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch108H',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_108L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch108L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_112H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch112H',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_112L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch112L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_116H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch116H',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_116L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch116L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_120H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch120H',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_120L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch120L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_124H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch124H',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_124L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch124L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_128H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch128H',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_128L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch128L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_132H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch132H',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_132L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch132L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_136H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch136H',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_136L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch136L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_140H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch140H',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_140L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch140L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_144H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch144H',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_144L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch144L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_149H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_BO_Ch149H',
                                                                        'class': 'wrapDRSmall',
                                                                        'type': 'snumber',
                                                                        'value': '0'
                                                                    }]

                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_149L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch149L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_153H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [{
                                                                        'id': 'rd_BO_Ch153H',
                                                                        'class': 'wrapDRSmall',
                                                                        'type': 'snumber',
                                                                        'value': '0'
                                                                    }]

                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_153L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch153L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'
                                                                                }]

                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_157H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch157H',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_157L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch157L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_161H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch161H',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_161L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch161L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_165H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch165H',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_165L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch165L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                               /* {
                                                                    'id': 'regdomBO_channel_169H',
                                                                    'type': 'div',
                                                                    'class': 'floatLeft padright2',
                                                                    'title': 'H&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch169H',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'id': 'regdomBO_channel_169L',
                                                                    'type': 'div',
                                                                    'class': 'floatRight padright2',
                                                                    'title': 'L&nbsp',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_BO_Ch169L',
                                                                                    'class': 'wrapDRSmall',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                               },*/    
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                }
                                                    ]
                                                },/*BO*/
                                                {
                                                    'id': 'regdomILTable',
                                                    'type': 'div',
                                                    'class': 'floatRight',
                                                    'objects': [
                                                                {
                                                                    'id': 'regdomcbIL',
                                                                    'class': 'mleft30',
                                                                    'type': 'checkbox',
                                                                    //'label1': 'Ins. Loss[dB]',
                                                                    'values': [{ 'value': 1, 'text': 'Ins.Loss' }],
                                                                    'checked': 0
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'padleft50 descSmallText10',
                                                                    'title': '[dB]'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'id': 'regdomlblIL',
                                                                    'class': 'mleft20 padtop5',
                                                                    'type': 'div',
                                                                    'title': '<b>U-NII-1</b>'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'TX',
                                                                    'class': 'floatLeft padright2 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomIL_TX_1',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_IL_TX_1',
                                                                                    'class': 'wrapDRSmall floatRight',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },

                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'RX',
                                                                    'class': 'floatLeft padright2 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomIL_RX_1',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_IL_RX_1',
                                                                                    'class': 'wrapDRSmall floatRight',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'AntG',
                                                                    'class': 'floatLeft padright2 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomIL_ANTG_1',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    //'title': 'ALG',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_IL_ANTG_1',
                                                                                    'class': 'wrapDRSmall floatRight',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                },
                                                                {
                                                                    'id': 'regdomlblIL_2A',
                                                                    'class': 'mleft20 padtop5',
                                                                    'type': 'div',
                                                                    'title': '<b>U-NII-2A</b>'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'TX',
                                                                    'class': 'floatLeft padright2 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomIL_TX_2A',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_IL_TX_2A',
                                                                                    'class': 'wrapDRSmall floatRight',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },

                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'RX',
                                                                    'class': 'floatLeft padright2 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomIL_RX_2A',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_IL_RX_2A',
                                                                                    'class': 'wrapDRSmall floatRight',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'AntG',
                                                                    'class': 'floatLeft padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomIL_ANTG_2A',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    //'title': 'ALG',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_IL_ANTG_2A',
                                                                                    'class': 'wrapDRSmall floatRight',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearM'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'padbot6'
                                                                },
                                                                {
                                                                    'id': 'regdomlblIL_2C1',
                                                                    'class': 'mleft20 padtop5 padpad',
                                                                    'type': 'div',
                                                                    'title': '<b>U-NII-2C1</b>'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'TX',
                                                                    'class': 'floatLeft padright2 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomIL_TX_2C1',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_IL_TX_2C1',
                                                                                    'class': 'wrapDRSmall floatRight',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },

                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'RX',
                                                                    'class': 'floatLeft padright2 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomIL_RX_2C1',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_IL_RX_2C1',
                                                                                    'class': 'wrapDRSmall floatRight',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'AntG',
                                                                    'class': 'floatLeft padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomIL_ANTG_2C1',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    //'title': 'ALG',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_IL_ANTG_2C1',
                                                                                    'class': 'wrapDRSmall floatRight',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                }
                                                                ,
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearB'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'padbot6'
                                                                },
                                                                {
                                                                    'id': 'regdomlblIL_2C2',
                                                                    'class': 'mleft20 padtop5',
                                                                    'type': 'div',
                                                                    'title': '<b>U-NII-2C2</b>'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'TX',
                                                                    'class': 'floatLeft padright2 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomIL_TX_2C2',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_IL_TX_2C2',
                                                                                    'class': 'wrapDRSmall floatRight',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },

                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'RX',
                                                                    'class': 'floatLeft padright2 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomIL_RX_2C2',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_IL_RX_2C2',
                                                                                    'class': 'wrapDRSmall floatRight',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'AntG',
                                                                    'class': 'floatLeft padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomIL_ANTG_2C2',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    //'title': 'ALG',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_IL_ANTG_2C2',
                                                                                    'class': 'wrapDRSmall floatRight',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearBB'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'padbot6'
                                                                },
                                                                {
                                                                    'id': 'regdomlblIL_3',
                                                                    'class': 'mleft20 padtop5',
                                                                    'type': 'div',
                                                                    'title': '<b>U-NII-3</b>'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'TX',
                                                                    'class': 'floatLeft padright2 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomIL_TX_3',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_IL_TX_3',
                                                                                    'class': 'wrapDRSmall floatRight',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },

                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'RX',
                                                                    'class': 'floatLeft padright2 padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomIL_RX_3',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_IL_RX_3',
                                                                                    'class': 'wrapDRSmall floatRight',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'title': 'AntG',
                                                                    'class': 'floatLeft padtop5'
                                                                },
                                                                {
                                                                    'id': 'regdomIL_ANTG_3',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    //'title': 'ALG',
                                                                    'objects': [
                                                                                {
                                                                                    'id': 'rd_IL_ANTG_3',
                                                                                    'class': 'wrapDRSmall floatRight',
                                                                                    'type': 'snumber',
                                                                                    'value': '0'

                                                                                }]
                                                                },
                                                                {
                                                                    'type': 'div',
                                                                    'class': 'clearS'
                                                                }
                                                    ]
                                                } /*IL*/
                                                ,
                                                {
                                                    'type': 'div',
                                                    'class': 'clearS'
                                                },
                                                {
                                                    'type': 'div',
                                                    'class': 'descSmallTextRed',
                                                    //'title': '* All values are in dBm'
                                                },                                           
                                                {
                                                    'id': 'backRegDomain',
                                                    'type': 'div',
                                                    'class': 'regButton mbot25 floatLeft',
                                                    'title': '<< &nbsp; &nbsp; Back',
                                                    'click': 'loadRFSettings();'
                                                },
                                                {
                                                    'id': 'createProject',
                                                    'type': 'div',
                                                    'class': 'regButton floatRight',
                                                    'title': 'Done &nbsp; &nbsp;',
                                                    'click': 'loadRFSettings();'
                                                },
                                    ]
                                }/*fieldsetRegDomain*/

                    ]
                },
        //HTTPD Server
        {
            'id': 'httpdserver',
            'title': 'HTTP Server',
            'load': 'localStorage.setItem(\'project\', "");loadHttpServer(); dropFile(\'#keyHttpPrimFileInput\');',
            'submit': 'alert ("submit"); return false;',
            'connect': true,
            'objects': [
                        {
                            'type': 'fieldset',
                            'id': 'fieldsethttpdprimary',
                            'title': 'Primary Port',
                            'class': 'wrapSection',
                            'objects': [
                                            {
                                                'id': 'httpserverprimaryportvalue',
                                                'type': 'div',
                                                'class': 'wrapDRS floatRight',
                                                'objects': [
                                                            {
                                                                'type': 'div',
                                                                'class': 'fieldName',
                                                                'title': 'Port Number'
                                                            },
                                                            {
                                                                'id': 'PRIM_PORT_VAL',
                                                                'class': '',
                                                                'type': 'text',
                                                                'value': '443'
                                                            }

                                                ]
                                            }, /*Port Value*/
                                            {
                                                'id': 'httpserverprimaryportsecure',
                                                'type': 'div',
                                                'class': 'floatLeft',
                                                'objects': [
                                                                            {
                                                                                'type': 'div',
                                                                                'class': 'clearM',

                                                                            },/*blanc line*/

                                                                        {
                                                                            'id': 'PRIM_PORT_SECURE',
                                                                            'class': 'wrapDRS',
                                                                            'type': 'checkbox',
                                                                            'label': 'bot 2',
                                                                            'values': [{ 'value': 0, 'text': 'Secured' }]
                                                                        }]
                                            }, /*Prim Port Secure*/
                                            {
                                                'type': 'div',
                                                'class': 'clearM',

                                            }, /*blanc line*/
                                            {
                                                'id': 'ACCESS_ROM',
                                                'class': 'wrapDRS floatLeft',
                                                'type': 'checkbox',
                                                'label': 'bot 2',
                                                'values': [{ 'value': 1, 'text': 'Enable ROM Pages' }]
                                            }  /*Access Rom Pages*/
                            ]
                        }, /*Primary*/
                        {
                            'type': 'fieldset',
                            'id': 'fieldsethttpdsecondary',
                            'title': 'Secondary Port',
                            'class': 'wrapSection',
                            'objects': [
                                             {
                                                 'id': 'httpserversecondaryportvalue',
                                                 'type': 'div',
                                                 'class': 'wrapDRS floatRight',
                                                 'objects': [
                                                             {
                                                                 'type': 'div',
                                                                 'class': 'fieldName',
                                                                 'title': 'Port Number'
                                                             },
                                                             {
                                                                 'id': 'SEC_PORT_VAL',
                                                                 'class': '',
                                                                 'type': 'text',
                                                                 'value': '443'
                                                             }

                                                 ]
                                             }, /*Secondary port number*/
                                             {
                                                 'id': 'httpserversecondaryportsecure',
                                                 'type': 'div',
                                                 'class': 'floatLeft',
                                                 'objects': [
                                                                        {
                                                                            'type': 'div',
                                                                            'class': 'clearM',

                                                                        },
                                                                        {
                                                                            'id': 'SEC_PORT_ENABLE',
                                                                            'class': 'wrapDRS',
                                                                            'type': 'checkbox',
                                                                            'label': 'bot 2',
                                                                            'values': [{ 'value': 0, 'text': 'Enable Secondary Port' }]
                                                                        }
                                                 ]
                                             }  /*Secondary port secured*/
                            ]
                        }, /*Secondary*/
                        {
                            'id': 'httpsecuritysettings',
                            'type': 'fieldset',
                            'title': 'Security settings',
                            'class': 'wrapSection ',
                            'objects': [
                                            {
                                                //'type': 'fieldset',
                                                'type': 'div',/***/
                                                'id': 'fieldsetHttpPrimCertSourceFile',
                                                'title': 'HTTP server <b>certificate</b> file name',
                                                'class': '',/***/
                                                'objects': [
                                                                /* {
                                                                    'type': 'div',
                                                                    'class': 'fieldName',
                                                                    'title': 'HTTP server private key file name'
                                                                },*/
                                                                {
                                                                    'id': 'httpserverPrimCertFileBrowse',
                                                                    'type': 'div',
                                                                    'class': '',
                                                                    'objects': [
                                                                                /*{
                                                                                    'id': 'certHttpPrimFileInput',
                                                                                    'type': 'text',
                                                                                    'class': 'floatLeft browseWidth'
                                                                                },
                                                                                {
                                                                                    'id': 'httpCertSelect',
                                                                                    'class': 'floatLeft browseWidth',
                                                                                    'type': 'select',
                                                                                    'values': []
                                                                                },*/
                                                                                {
                                                                                    'id': 'HttpCertInnerDiv',
                                                                                    'type': 'div',
                                                                                    'class': 'select-editabletext floatLeft browseWidth',
                                                                                    'objects': [
                                                                                                {
                                                                                                    'id': 'httpCertSelect',
                                                                                                    'class': 'set1',
                                                                                                    'type': 'select',
                                                                                                    'values': []
                                                                                                },
                                                                                                {

                                                                                                    'id': 'certHttpPrimFileInput',
                                                                                                    'type': 'input',
                                                                                                    'class': 'set2',
                                                                                                    'value': ''
                                                                                                }]
                                                                                },
                                                                                /* {
                                                                                     'id': 'clearHttpPrimCertFile',
                                                                                     'type': 'div',
                                                                                     //'class': 'regButton mtop0 mLeftS',
                                                                                     'class': 'floatRight regButton mtop0 mleft10 mright37',
                                                                                     'title': '&nbsp Clear &nbsp',
                                                                                     //'click': 'clearProjectFile("/api/1/flash/deleteHttpCertFile", $(\'#certHttpPrimFileInput\'));'
                                                                                     'tooltip':'Clear only from Http and not from User Files!',
                                                                                     'click': 'clearHttpCertFile();'

                                                                                 },*/
                                                                                {
                                                                                    'id': 'browseHttpPrimCertFile',
                                                                                    'type': 'div',
                                                                                    //'class': 'regButtonBr mtop0 mLeftS',
                                                                                    'class': 'floatRight regButtonBr mtop0',
                                                                                    'title': 'Browse',
                                                                                    'tooltip': 'The selected file will be programmed to the device FS and appear in the User Files list',
                                                                                    'click': '$(\'#HTTP_PRIM_CERT_FILE\').click();'
                                                                                },
                                                                               
                                                                                {
                                                                                    'id': 'HTTP_PRIM_CERT_FILE',
                                                                                    'type': 'file',
                                                                                    'class': 'hideFile',
                                                                                    'change': 'postHttpFile(\'HTTP_PRIM_CERT_FILE\', \'uploadHttpCertFile\', \'8\');'
                                                                                    //'change': 'postFile(\'HTTP_PRIM_CERT_FILE\', \'uploadHttpCertFile\', \'8\');'
                                                                                    //'change': 'updateFiles(\'8\', \'HTTP_PRIM_CERT_FILE\');'
                                                                                }
                                                                                
                                                                    ]
                                                                }
                                                ]
                                            }, /*Primary Cert*/
                                            {
                                                'type': 'div',
                                                'class': 'clearM',

                                            }, /*Blanc Line*/
                                            {
                                                //'type': 'fieldset',
                                                'type': 'div',/***/
                                                'id': 'fieldsetHttpPrimKeySourceFile',
                                                'title': 'HTTP server <b>private key</b>',
                                                //'class': 'wrapSection',
                                                'class': 'wrapDRS',/***/
                                                'objects': [
                                                                {
                                                                    'id': 'httpserverPrimKeyFileBrowse',
                                                                    'type': 'div',
                                                                    'class': 'wrapDRS floatLeft',
                                                                    'objects': [
                                                                        /*
                                                                                {
                                                                                    'id': 'keyHttpPrimFileInput',
                                                                                    'type': 'text',
                                                                                    'class': 'floatLeft browseWidth'
                                                                                },
                                                                                */
                                                                                {
                                                                                    'id': 'HttpKeyInnerDiv',
                                                                                    'type': 'div',
                                                                                    'class': 'select-editabletext floatLeft browseWidth',
                                                                                    'objects': [
                                                                                                {
                                                                                                    'id': 'httpKeySelect',
                                                                                                    'class': 'set1',
                                                                                                    'type': 'select',
                                                                                                    'values': []
                                                                                                },
                                                                                                {

                                                                                                    'id': 'keyHttpPrimFileInput',
                                                                                                    'type': 'input',
                                                                                                    'class': 'set2',
                                                                                                    'value': ''
                                                                                                }]
                                                                                },
                                                                                /*{
                                                                                    'id': 'clearHttpPrimKeyFile',
                                                                                    'type': 'div',
                                                                                    //'class': 'regButton mtop0 mLeftS',
                                                                                    'class': 'floatRight regButton mtop0 mleft10 mright37',
                                                                                    'title': '&nbsp Clear &nbsp',
                                                                                    'click': 'clearHttpKeyFile();'
                                                                                },*/
                                                                                {
                                                                                    'id': 'browseHttpPrimKeyFile',
                                                                                    'type': 'div',
                                                                                    //'class': 'regButtonBr mtop0 mLeftS',
                                                                                    'class': 'floatRight regButtonBr mtop0',
                                                                                    'title': 'Browse',
                                                                                    'tooltip': 'The selected file will be programmed to the device FS and appear in the User Files list',
                                                                                    'click': '$(\'#HTTP_PRIM_KEY_FILE\').click();'
                                                                                },
                                                                                {
                                                                                    'id': 'HTTP_PRIM_KEY_FILE',
                                                                                    'type': 'file',
                                                                                    'class': 'hideFile',
                                                                                    'change': 'postHttpFile(\'HTTP_PRIM_KEY_FILE\', \'uploadHttpKeyFile\', \'7\');'
                                                                                }/*,
                                                                                {
                                                                                    'type': 'div',
                                                                                    'id': 'httpPrimKeyFileNote',
                                                                                    'text': "justtext",
                                                                                    'title': "<b>Important: The key file is saved into the project's directory</b>",
                                                                                    'class': 'uRed'
                                                                                }*/
                                                                    ]
                                                                }
                                                ]
                                            }, /*Primary Key*/
                                            {
                                                'type': 'div',
                                                'class': 'clearM'
                                            }, /*Blanc Line*/
                                            {
                                                'id': 'ENABLE_CA_CERT',
                                                'class': 'wrapDRS floatLeft',
                                                'type': 'checkbox',
                                                'label': 'bot 2',
                                                'values': [{ 'value': 1, 'text': 'Enable Client Authentication' }]
                                            }, /*Enable client auth*/
                                            {

                                                'type': 'div',/***/
                                                'id': 'fieldsetHttpCaCertSourceFile',
                                                'title': '<b>Client Authentication certificate</b> file name',
                                                //'class': 'wrapSection',
                                                'class': 'wrapDRS',/***/
                                                'objects': [

                                                               {
                                                                   'id': 'httpserverCaCertFileBrowse',
                                                                   'type': 'div',
                                                                   'class': 'wrapDRS floatLeft',
                                                                   'objects': [
                                                                                /*
                                                                               {
                                                                                   'id': 'cacertHttpPrimFileInput',
                                                                                   'type': 'text',
                                                                                   'class': 'floatLeft browseWidth'
                                                                               },*/
                                                                               {
                                                                                   'id': 'HttpCaCertInnerDiv',
                                                                                   'type': 'div',
                                                                                   'class': 'select-editabletext floatLeft browseWidth',
                                                                                   'objects': [
                                                                                               {
                                                                                                   'id': 'httpCaCertSelect',
                                                                                                   'class': 'set1',
                                                                                                   'type': 'select',
                                                                                                   'values': []
                                                                                               },
                                                                                               {

                                                                                                   'id': 'cacertHttpPrimFileInput',
                                                                                                   'type': 'input',
                                                                                                   'class': 'set2',
                                                                                                   'value': ''
                                                                                               }]
                                                                               },
                                                                             /*  {
                                                                                   'id': 'clearHttpCaCertFile',
                                                                                   'type': 'div',
                                                                                   //'class': 'regButton mtop0 mLeftS',
                                                                                   'class': 'floatRight regButton mtop0 mleft10 mright37',
                                                                                   'title': '&nbsp Clear &nbsp',
                                                                                   'click': 'clearHttpCAFile();'
                                                                               },*/
                                                                               {
                                                                                   'id': 'browseHttpCaCertFile',
                                                                                   'type': 'div',
                                                                                   //'class': 'regButtonBr mtop0 mLeftS',
                                                                                   'class': 'floatRight regButtonBr mtop0',
                                                                                   'title': 'Browse',
                                                                                   'tooltip': 'The selected file will be programmed to the device FS and appear in the User Files list',
                                                                                   'click': '$(\'#HTTP_CA_CERT_FILE\').click();'
                                                                               },
                                                                               {
                                                                                   'id': 'HTTP_CA_CERT_FILE',
                                                                                   'type': 'file',
                                                                                   'class': 'hideFile',
                                                                                   'change': 'postHttpFile(\'HTTP_CA_CERT_FILE\', \'uploadHttpCaCertFile\', \'9\');'
                                                                               }
                                                                   ]
                                                               }
                                                ]
                                            }  /*Ca Cert*/
                            ]
                        }  /*Security settings*/
            ]
        },

//Tools
{'id':'files',
    'title':'Tools',
    'load':'localStorage.setItem(\'project\', "");',
    'submit':'alert ("submit"); return false;',
    'connect':true,
    'objects':[]
},
    {   'id':'toolscreateflashimage',
        'title':'Tools-> Create Flash Image',
        'load':'localStorage.setItem(\'project\', "");',
        'submit':'alert ("submit"); return false;',
        'connect':true,
        'objects':[
                        {'type':'fieldset',
                         'id':'fieldsettoolscreateflashimagesource',
                         'title':'MCU Image Source File',
                         'class': 'wrapSection',
                         'objects':[
                                        {
                                            'id':'keyImageSourceFileInput',
                                            'type':'text',
                                            'class':'keyWidth floatLeft'
                                        },
                                        {
                                            'id':'browseImageSourceFile',
                                            'type':'div',
                                            'class': 'regButtonBr mtop0 mLeftS',
                                            'title':'Browse',
                                            'click':'$(\'#IMAGE_SOURCE_FILE\').click();'
                                        }, 
                                        {
                                            'id':'IMAGE_SOURCE_FILE',
                                            'type':'file',
                                            'class':'hideFile',
                                            'change':'postFile(\'IMAGE_SOURCE_FILE\', \'uploadProjectCertStoreFile\', \'1\');',
                                        },
                                   ]
                        },
                        {
                            'id':'CreateFlashImageButton',
                            'type':'div',
                            'class': 'regButton floatLeft',
                            'title':'Create Flash Image',
                            'click':'createFlashImage();'
                        }
 ]
    },
    {   'id':'toolssignfile',
        'title':'Tools',
        'load': 'localStorage.setItem(\'project\', ""); dropFile(\'#toolSignFileSource\');dropFile(\'#toolSignFileKey\');dropFile(\'#toolsImgKeyFileInput\');dropFile(\'#toolsCSRFileInput\');',
        'submit':'alert ("submit"); return false;',
        'connect':true,
        'objects': [
                    {
                        'type': 'fieldset',
                        'id': 'fieldsettoolcsr',
                        'title': 'Certificate Sign Request',
                        'class': 'wrapSection',
                        'objects': [
                                    {
                                        'id': 'getCSR',
                                        'type': 'div',
                                        'class': 'regButton floatRight',
                                        'title': ' &nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp;Read CSR &nbsp; &nbsp; &nbsp;&nbsp; &nbsp;',
                                        'tooltip': 'Read CSR file from project',
                                        'click': 'getCSR()'
                                    },
                                    {
                                        'type': 'div',
                                        'class': 'clearM'
                                    },
                                       {
                                           'type': 'div',
                                           'class': 'fieldName',
                                           'title': 'Signed Certificate File Name'
                                       },
                                       {
                                           'id': 'toolsCSRFileInput',
                                           'type': 'text',
                                           'class': 'floatLeft keyWidth',
                                           'drop': 'dropFileExt(\'#toolsCSRFileInput\');'
                                       },
                                       {
                                           'id': 'toolsBrowseCSRFile',
                                           'type': 'div',
                                           'class': 'regButtonBr mtop0 mLeftS',
                                           'title': 'Browse',
                                           'click': '$(\'#TOOLS_SIGNED_CSR_FILE_NAME\').click();'

                                       },
                                       {
                                           'id': 'TOOLS_SIGNED_CSR_FILE_NAME',
                                           'type': 'file',
                                           'class': 'hideFile',
                                           'change': 'updateFiles(\'16\', \'TOOLS_SIGNED_CSR_FILE_NAME\');',

                                       },
                                       {
                                           'type': 'div',
                                           'class': 'clearM'
                                       },
                                       {
                                           'type': 'div',
                                           'id': 'lastImageLinkCSR'
                                       },
                                       {
                                           'id': 'setSigned_CSR',
                                           'type': 'div',
                                           'class': 'regButton floatRight',
                                           'title': '&nbsp;&nbsp;Write Certificate&nbsp; &nbsp;',
                                           'tooltip': 'Write certificate to project',
                                           'click': 'setCSR()'
                                       }
                        ]
                    },//CSR
                    {'type':'fieldset',
                     'id':'fieldsettoolsign',
                     'title':'Sign File',
                     'class': 'wrapSection',
                     'objects':[
                                        {'type':'div',
                                         'id':'divtoolsignfilesource',
                                         'class': '',
                                         'objects':[
                                                        {
                                                            'type':'div',
                                                            'class': 'fieldName',
                                                            'title':'Source File'
                                                        },
                                                        {
                                                            'id':'toolSignFileSource',
                                                            'type':'text',
                                                            'class':'keyWidth floatLeft',
                                                            'drop' :'dropFileExt(\'#toolSignFileSource\');'
                                                        },
                                                        {
                                                            'id':'browseToolSignFileSource',
                                                            'type':'div',
                                                            'class': 'regButtonBr mtop0 mLeftS',
                                                            'title':'Browse',
                                                            'click':'$(\'#TOOLS_SIGN_SOURCE_FILE\').click();'
                                                        }, 
                                                        {
                                                            'id':'TOOLS_SIGN_SOURCE_FILE',
                                                            'type':'file',
                                                            'class':'hideFile',
                                                            'change':'updateFiles(\'4\', \'TOOLS_SIGN_SOURCE_FILE\');',
                                                        },
                                                   ]
                                        },
                                        {
                                        'type':'div',
                                        'class': 'clearM'
                                        },
                                        {'type':'div',
                                         'id':'divtoolsignfilekey',
                                         'class': '',
                                         'objects':[
                                                         {
                                                            'type':'div',
                                                            'class': 'fieldName',
                                                            'title':'Private Key File'
                                                        },
                                                        {
                                                            'id':'toolSignFileKey',
                                                            'type':'text',
                                                            'class':'keyWidth floatLeft',
                                                            'drop' :'dropFileExt(\'#toolSignFileKey\');'
                                                        },
                                                        {
                                                            'id':'browseToolSignFileKey',
                                                            'type':'div',
                                                            'class': 'regButtonBr mtop0 mLeftS',
                                                            'title':'Browse',
                                                            'click':'$(\'#TOOLS_SIGN_KEY_FILE\').click();'
                                                        }, 
                                                        {
                                                            'id':'TOOLS_SIGN_KEY_FILE',
                                                            'type':'file',
                                                            'class':'hideFile',
                                                            'change':'updateFiles(\'5\', \'TOOLS_SIGN_KEY_FILE\');',
                                                            
                                                        },
                                                   ]
                                        },
                                        {
                                            'type':'div',
                                            'class': 'clearM'
                                        },
                                        {
                                            'id'        : 'buttonToolsSignBinary',
                                            'type'      : 'div',
                                            'class'     : 'regButton floatRight',
                                            'title'     : 'Get binary &nbsp',
                                            'tooltip'   : 'Get signed binary file',
                                            'click'     : 'signFile();'
                                        },
                                        {
                                            'id'        : 'buttonToolsSignBase64',
                                            'type'      : 'div',
                                            'class'     : 'regButton floatRight mRightM',
                                            'title'     : 'Get base64',
                                            'tooltip'   : 'Get signed base64 file',
                                            'click'     : 'signFile64();'
                                        },
                                        {
                                            'type':'div',
                                            'id':'toolSignBinaryLink'
                                        }
                                ]
                    },//Sign File
                    {
                        'type':'div',
                        'class': 'clearM'
                    },
                    {'type':'fieldset',
                     'id':'fieldsettooldecryptimage',
                     'title':'Activate Image',
                     'class': 'wrapSection',
                     'objects':[
                                    {
                                        'type':'div',
                                        'class': 'fieldName',
                                        'title':'Image Key File Name'
                                    },                   
                                    {
                                        'id':'toolsImgKeyFileInput',
                                        'type':'text',
                                        'class':'floatLeft keyWidth',
                                        'drop' :'dropFileExt(\'#toolsImgKeyFileInput\');'
                                    },
                                    {
                                        'id':'toolsBrowseImgKeyFile',
                                        'type':'div',
                                        'class': 'regButtonBr mtop0 mLeftS',
                                        'title':'Browse',
                                        'click':'$(\'#TOOLS_PROJ_IMAGE_KEY_FILE_NAME\').click();'
                                        
                                    }, 
                                    {
                                        'id':'TOOLS_PROJ_IMAGE_KEY_FILE_NAME',
                                        'type':'file',
                                        'class':'hideFile',
                                        'change':'updateFiles(\'6\', \'TOOLS_PROJ_IMAGE_KEY_FILE_NAME\');',
                                        
                                    },
                                    {
                                            'type':'div',
                                            'class': 'clearM'
                                    },
                                    {
                                        'id':'activateToolsDecryptImg',
                                        'type':'div',
                                        'class': 'regButton floatRight',
                                        'title':' &nbsp;&nbsp;Activate &nbsp; &nbsp;',
                                        'tooltip': 'Activate programmed encrypted image',
                                        'click':'decryptImage()'
                                    }
                                ]
                    },//Activate Image
                    {
                        'id':'backToolsWelcome',
                        'type':'div',
                        'class': 'regButton mbot25 floatLeft',
                        'title':'<< &nbsp; &nbsp; Back',
                        'click':'loadWelcome();'
                    } //Back Button
                    ]
    },
    {   'id':'toolsconvertfile',
        'title':'Tools-> Convert File',
        'load':'localStorage.setItem(\'project\', "");',
        'submit':'alert ("submit"); return false;',
        'connect':true,
        'objects':[
                         {'type':'fieldset',
                         'id':'fieldsettoolconvertfilesource',
                         'title':'Source File',
                         'class': 'wrapSection',
                         'tooltip': 'Source file to convert',
                         'objects':[
                                        {
                                            'id':'toolConvertFileSource',
                                            'type':'text',
                                            'class':'keyWidth floatLeft'
                                        },
                                        {
                                            'id':'browseToolConvertFileSource',
                                            'type':'div',
                                            'class': 'regButtonBr mtop0 mLeftS',
                                            'title':'Browse',
                                            'click':'$(\'#TOOLS_CONVERT_SOURCE_FILE\').click();'
                                        }, 
                                        {
                                            'id':'TOOLS_CONVERT_SOURCE_FILE',
                                            'type':'file',
                                            'class':'hideFile',
                                            'change':'postFile(\'TOOLS_CONVERT_SOURCE_FILE\', \'uploadProjectCertStoreFile\', \'1\');',
                                        },
                                   ]
                        },
                        {
                            'id':'buttonToolsConvertBase64',
                            'type':'div',
                            'class': 'regButton floatRight',
                            'title':'Convert to 64 Base',
                            'click':'convertTo64Base();'
                        }
                    ]
    },
//Files
{   'id':'files',
    'title':'Files',
    'load':'localStorage.setItem(\'project\', "");loadFilesSum();',
    'submit':'alert ("submit"); return false;',
    'connect':true,
    'objects':[
                {   'type':'fieldset',
                    'id':'fieldsetFilesSPSum',
                    'title':'Service Pack',
                    'class': 'wrapSection',
                    'objects':[
                                    {
                                     'type':'div',
                                     'title':'Service Pack File Name',
                                     'class':'wrapDRS floatLeft'                                                         
                                    },
                                    {
                                     'type':'div',
                                     'id':'SP_FILE_SUM',
                                     'class':'wrapDRS floatRight'                                                         
                                    },
                                    
                                    {
                                     'type':'div',
                                     'class': 'clearM'
                                    },

                              ]
                },
                
                {   'type':'fieldset',
                    'id':'fieldsetFilesCSSum',
                    'title':'Trusted Root-Certificate Catalog',
                    'class': 'wrapSection',
                    'objects':[
                                {
                                 'type':'div',
                                 'title':'Trusted Root-Certificate Catalog File Name',
                                 'class':'wrapDRS floatLeft'                                                         
                                },
                                {
                                 'type':'div',
                                 'id':'CS_FILE_SUM',
                                 'class':'wrapDRS floatRight'                                                         
                                },
                                
                                {
                                 'type':'div',
                                 'class': 'clearM'
                                },
                                
                                {
                                 'type':'div',
                                 'title':'Trusted Root-Certificate Catalog Signature File Name',
                                 'class':'wrapDRS floatLeft'                                                         
                                },
                                {
                                 'type':'div',
                                 'id':'CSS_FILE_SUM',
                                 'class':'wrapDRS floatRight'                                                         
                                },
                                
                                {
                                 'type':'div',
                                 'class': 'clearM'
                                }
                                ]
                }
              ]
}, 
    //User Application
    {   'id':'filesuserfiles',
			'title':'Files > User Files',
			'load':'loadFilesuserfiles();',
			'submit':'alert ("submit"); return false;',
			'connect':true,
			'objects':[
					{
						'id':'checkUnchekDiv',
						'type':'div',
						'class': 'checkUnchek',
						'title':'<span>Check All</span> | <span>Uncheck All</span>'
					},
					{
						'id':'fileActionSelectDiv',
						'type':'div',
						'class': 'wrapDRS floatRight',
						'title':'<b>Action: </b>',
						'objects':[
							     {		
                                    'id':'fileActionSelect',
                                    'class':'',
                                    'type':'select',
                                    'values': [{'value':'Select Action' ,'text':'Select Action'     },
                                               //{'value':'Program'       ,'text':'Program'           },
                                               {'value':'Remove'        ,'text':'Remove Selected'},
                                               //{'value':'NewFolder'     ,'text':'New Folder'        },
                                               //{'value':'Upload'        ,'text':'Upload File'       },
                                               //{'value':'Rename'        ,'text':'Rename Folder'     },
                                               {'value':'MCUImg'        ,'text':'Select MCU Image'  }],//,
                                               //{'value':'Properties'    ,'text':'File Properties'   }],
                                'change':'changeAction();'
                                },
                                {
                                    'id':'fileActionSelectAction',
                                    'type':'div',
                                    //'class': 'regButton mtop0 floatRight rbGray mLeftS',
                                    'class': 'regButton mtop0 floatRight mLeftS',
                                    'title':'Execute',
                                    'click':'filesDivPageAction();'
                                }
						]
					},
					{
						'id':'divclear1',
						'class':'clear mbot15',
						'type':'div'
						
					},
					{
						'id':'fileElement',
						'type':'file',
						'class':'hideFile'
					},	
                    {
						'id':'MCUImg',
						'type':'file',
						'class':'hideFile'
					},
                    /*
					{
						'id':'fileSysDiv',
						'type':'div',
						'class': 'fileSys',
						'objects':[
							{
								'id':'fsTitlesDiv',
								'type':'div',
								'class': 'fsTitles',
								'objects':[
									{
										'type':'div',
										'class': 'fs1',
										'title':'File'
									},
									{
										'type':'div',
										'class': 'fs3',
										'title':'Fail Safe'
									},
									{
										'type':'div',
										'class': 'fs4',
										'title':'Secure'
									},
									{
										'type':'div',
										'class': 'fs5',
										'title':'Max size'
									}
								]
							},
							{
								'id':'fileSysDiplayPageDiv',
								'type':'div',
								'class': 'fileSysDiplay'
							},
							//{
							//	'id':'saveButton',
							//	'type':'div',
							//	'class': 'regButton',
							//	'title':'Save',
							//	'click':'FilesuserfilesSaveClicked();'
							//}
						]
                        
					}
                    */
                    
                    {
						'id':'fileSysDiv',
						'type':'div',
						'class': 'fb',
					}
                    
			]
		},
    //User Application
    {   'id':'filesuserfilesonline',
        'title':'Files > Device File Browser',
        'load':'loadFilesuserfilesOnline();',
        'submit':'alert ("submit"); return false;',
        'connect':true,
        'objects':[
                    {
                        'id'    :'userFileRefreshBtn',
                        'type'  :'div',
                        'class' :'<i fa fa-refresh cursorDefault i/>',
                        'click' :'browse();'
                    },
                    {
                        'type':'div',
                        'class': 'clearM'
                    },  
                    {
                        'id'    :'fileSysDiv',
                        'type'  :'div',
                        'class' :'fb',
                    }
                 ]
	},

    //Service Pack
    {   'id':'binservicepack',
        'title':'Files > Service Pack',
        'load':'localStorage.setItem(\'project\', "");loadFilesSP(); dropFile(\'#spFileInput\');',
        'submit':'alert ("submit"); return false;',
        'connect':true,
        'objects':[
                    {'type':'fieldset',
                     'id':'fieldsetBinServicePack',
                     'title':'Service Pack File Name',
                     'class': 'wrapSection',
                     'objects':[
                                    {
                                        'id':'fieldsetBinServicePackFileBrowse',
                                        'type':'div',
                                        'class': 'wrapDRS floatLeft',
                                        'objects':[
                                                    {
                                                    'id':'spFileInput',
                                                    'type':'text',
                                                    'class':'floatLeft browseWidth'
                                                    },
                                                    {
                                                    'id':'browseServicePackFile',
                                                    'type':'div',
                                                    'class': 'regButtonBr mtop0 mLeftS',
                                                    'title':'Browse',
                                                    'click':'$(\'#SP_FILE\').click();'
                                                    }, 
                                                    {
                                                        'id':'SP_FILE',
                                                        'type':'file',
                                                        'class':'hideFile',
                                                        'change':'postFile(\'SP_FILE\', \'uploadProjectSPFile\', \'0\');',
                                                    }, 
                                                    {
                                                    'id':'clearServicePackFile',
                                                    'type':'div',
                                                    'class': 'regButton mtop0 mLeftS floatRight',
                                                    'title':'&nbspClear&nbsp&nbsp',
                                                    'tooltip':'Clear Service Pack from the project',
                                                    'click':'clearServicePackFile()',
                                                    
                                                    }, 
                                                   ]
                                    }
                               ]
                    }
                  ]
    },
    //Certificate Store
    {   'id':'bincertstore',
        'title':'Files > Trusted Root-Certificate Catalog',
        'load': 'localStorage.setItem(\'project\', "");loadCertStore(); dropFile(\'#keyCertStoreFileInput\'); dropFile(\'#keyCertStoreSignFileInput\'); dropFile(\'#otpFileInput\')',
        'submit':'alert ("submit"); return false;',
        'connect':true,
        'objects':[
                    {'type':'fieldset',
                     'id':'fieldsetBinCertStore',
                     'title':'Trusted Root-Certificate Catalog',
                     'class': 'wrapSection',
                     'objects':[
                                    {   'id':'USE_DEF_CERT_STORE',
                                                        'class':'',
                                                        'type':'checkbox',
                                                        'label': 'bot 2',
                                                        'tooltip': 'Default certificate store, valid for play ground dummy certificates',
                                                        'values': [{'value':1,'text':'Use default Trusted Root-Certificate Catalog'}],
                                                        'checked':1
                                    },
                                    {
                                        'type':'div',
                                        'class': 'clearM'
                                    },                                    
                                    {
                                         'type':'fieldset',
                                         'id':'fieldsetBinCertStoreSourceFile',
                                         'title':'Source File',
                                         'class': 'wrapSection',
                                         'objects':[
                                                    {
                                                    'id':'fieldsetCertStoreFileBrowse',
                                                    'type':'div',
                                                    'class': 'wrapDRS floatLeft',
                                                    'objects':[
                                                                {
                                                                'id':'keyCertStoreFileInput',
                                                                'type':'text',
                                                                'class':'floatLeft'
                                                                },
                                                                {
                                                                'id':'browseCertStoreFile',
                                                                'type':'div',
                                                                'class': 'regButtonBr mtop0 mLeftS',
                                                                'title':'Browse',
                                                                'click':'$(\'#CERT_STORE_FILE\').click();'
                                                                }, 
                                                                {
                                                                    'id':'CERT_STORE_FILE',
                                                                    'type':'file',
                                                                    'class':'hideFile',
                                                                    'change':'postFile(\'CERT_STORE_FILE\', \'uploadProjectCertStoreFile\', \'1\');',
                                                                },
                                                               ]
                                                    }
                                                   ]
                                    },
                                    {
                                        'type':'div',
                                        'class': 'clear'
                                    },
                                    {
                                         'type':'fieldset',
                                         'id':'fieldsetBinCertStoreSignSourceFile',
                                         'title':'Signature Source File',
                                         'class': 'wrapSection',
                                         'objects':[
                                                    {
                                                    'id':'fieldsetCertStoreSignSourceFileBrowse',
                                                    'type':'div',
                                                    'class': 'wrapDRS floatLeft',
                                                    'objects':[
                                                                {
                                                                'id':'keyCertStoreSignFileInput',
                                                                'type':'text',
                                                                'class':'floatLeft'
                                                                },
                                                                {
                                                                'id':'browseCertStoreSignFile',
                                                                'type':'div',
                                                                'class': 'regButtonBr mtop0 mLeftS',
                                                                'title':'Browse',
                                                                'click':'$(\'#CERT_STORE_SIGNATURE_FILE\').click();'
                                                                }, 
                                                                {
                                                                    'id':'CERT_STORE_SIGNATURE_FILE',
                                                                    'type':'file',
                                                                    'class':'hideFile',
                                                                    'change':'postFile(\'CERT_STORE_SIGNATURE_FILE\', \'uploadProjectCertStoreSigFile\', \'2\');',
                                                                },
                                                               ]
                                                    }
                                                   ]
                                    }/*,
                                    {
                                        'type':'div',
                                        'class': 'clear'
                                    },
                                    {
                                        'id': 'ADD_CLST_OTA',
                                        'class': '',
                                        'type': 'checkbox',
                                        'label': 'bot 2',
                                        'tooltip': 'Add certificate store to OTA',
                                        'values': [{ 'value': 1, 'text': 'Add certificate store to OTA' }],
                                        'checked': 0
                                    },
                                    {
                                        'type': 'div',
                                        'class': 'clearM'
                                    }*/
                               ]
                    },
                    {
                        'type': 'fieldset',
                        'id': 'fieldsetProjImgSB',
                        'title': 'Vendor Certificate Catalog',
                        'class': 'wrapSection',
                        'objects': [
                                    {
                                        'id': 'USE_SEC_BTLDR',
                                        'class': '',
                                        'type': 'checkbox',
                                        'label': 'bot 2',
                                        'tooltip': 'Enabling this option will allow using a certificate catalog that is signed by the vendor root of trust. The vendor root certificate needs to be stored in the OTP file. When using this option the  vendor should use his own certificate catalog and the official certificate catalog from TI cannot be used.',
                                        'values': [{ 'value': 1, 'text': 'Use Vendor Certificate Catalog' }],
                                        'checked': 0
                                    },
                                    {
                                        'type': 'div',
                                        'class': 'clearM'
                                    }
                        ]
                    },
                                    
					{
					    'type': 'fieldset',
					    'id': 'fieldsetOTPOut',
					    'title': 'OTP',
					    'class': 'wrapSection',
					    'objects': [
                                       {
                                           'id': 'USE_OTP',
                                           'class': '',
                                           'type': 'checkbox',
                                           'label': 'bot 2',
                                           'tooltip': 'The OTP file is stored in the OTP section of the serial flash during the first programming and cannot be changed afterwards. It holds the vendor root certificate. &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp Note: Once the OTP section is programmed, Secure files must be signed by the vendor chain of trust.',
                                           'values': [{ 'value': 1, 'text': 'Add OTP file' }],
                                           'checked': 0
                                       },
                                       {
                                           'type': 'div',
                                           'class': 'clearM'
                                       },
                                       {
                                           'id': 'fieldsetOTPInn',
                                           'type': 'div',
                                           'class': 'wrapDRS floatLeft',
                                           'objects': [
                                                       {
                                                           'id': 'otpFileInput',
                                                           'type': 'text',
                                                           'class': 'floatLeft'
                                                       },
                                                       {
                                                           'id': 'browseOTPFile',
                                                           'type': 'div',
                                                           'class': 'regButtonBr mtop0 mLeftS',
                                                           'title': 'Browse',
                                                           'click': '$(\'#OTP_FILE\').val(""); $(\'#OTP_FILE\').click();'
                                                       },
                                                       {
                                                           'id': 'clearOtpFile',
                                                           'type': 'div',
                                                           'class': 'regButton mtop0 mLeftS floatRight',
                                                           'title': '&nbspClear&nbsp&nbsp',
                                                           'tooltip': 'Clear OTP file from project',
                                                           'click': 'clearOtpFile()',
                                                       },
                                                       {

                                                           'id': 'OTP_FILE',
                                                           'type': 'file',
                                                           'class': 'hideFile',
                                                           'change': 'postFile(\'OTP_FILE\', \'uploadOtpFile\', \'13\');',
                                                       }
                                                       /*,{
                                                           'type': 'div',
                                                           'id': 'encrKeyFileNote',
                                                           'text': "justtext",
                                                           'title': "Important: The key file is saved into the project's directory",
                                                           'class': 'uRed'
                                                       }*/
                                           ]
                                       }
					    ]
					}

                  ]
    },
    {   'id':'openProjImg',
        'title':'Program Image',
        'load':'toggleFullOverlay($("#mainContent"), true); loadOpenProjImg(); ',
        'submit':'alert ("submit"); return false;',
        'connect':true,
        'objects':[
                    {'type':'fieldset',
                        'id':'fieldsetopenProjImg',
                        'title':'Image File Name',
                        'class': 'wrapSection',
                        'objects':[
                                    {
                                    'id':'fieldsetopenProjImgBrowse',
                                    'type':'div',
                                    'class': 'wrapDRS floatLeft',
                                    'objects':[
                                                {
                                                'id':'keyImgFileInput',
                                                'type':'text',
                                                'class':'floatLeft keyWidth',
                                                'drop' :'dropFileExt(\'#keyImgFileInput\');'
                                                },
                                                {
                                                'id':'browseImgFile',
                                                'type':'div',
                                                'class': 'regButtonBr mtop0 mLeftS',
                                                'title':'Browse',
                                                'click':'$(\'#IMG_FILE\').click();'
                                                }, 
                                                {
                                                    'id':'IMG_FILE',
                                                    'type':'file',
                                                    'class':'hideFile',
                                                    'change':'enableSetProjImage(false);'
                                                }
                                                ]
                                    }
                                ]
                    },
                    {
					'type':'div',
					'class': 'clear'
                    },
                    {'type':'fieldset',
                        'id':'fieldsetopenProjImgKey',
                        'title':'Image Key File Name',
                        'class': 'wrapSection',
                        'objects':[
                                    {
                                    'id':'fieldsetOpenProjImgKeyBrowse',
                                    'type':'div',
                                    'class': 'wrapDRS floatLeft',
                                    'objects':[
                                                {
                                                'id':'keyImgKeyFileInput',
                                                'type':'text',
                                                'class':'floatLeft keyWidth',
                                                'drop' :'dropFileExt(\'#keyImgKeyFileInput\');'
                                                },
                                                {
                                                'id':'browseImgKeyFile',
                                                'type':'div',
                                                'class': 'regButtonBr mtop0 mLeftS',
                                                'title':'Browse',
                                                'click':'$(\'#PROJ_IMAGE_KEY_FILE_NAME\').click();'
                                                }, 
                                                {
                                                    'id':'PROJ_IMAGE_KEY_FILE_NAME',
                                                    'type':'file',
                                                    'class':'hideFile',
                                                    'change':'enableSetProjImage(false);'
                                                }
                                                ]
                                    }
                                ]
                    },
                    {
                        'type': 'fieldset',
                        'id': 'fieldsetProjImgSB',
                        'title': 'Vendor Certificate Catalog',
                        'class': 'wrapSection',
                        'objects': [
                                    {
                                        'id': 'PROJ_IMAGE_USE_SEC_BTLDR',
                                        'class': '',
                                        'type': 'checkbox',
                                        'label': 'bot 2',
                                        'tooltip': 'Enabling this option will allow using a certificate catalog that is signed by the vendor root of trust. The vendor root certificate needs to be stored in the OTP file. When using this option the  vendor should use his own certificate catalog and the official certificate catalog from TI cannot be used.',
                                        'values': [{ 'value': 1, 'text': 'Use Vendor Certificate Catalog' }],
                                        'checked': 0
                                    },
                                    {
                                        'type': 'div',
                                        'class': 'clearM'
                                    },
                        ]
                    },
                    {
                        'type': 'fieldset',
                        'id': 'fieldsetProjImgOTPOut',
                        'title': 'OTP',
                        'class': 'wrapSection',
                        'objects': [
                                       {
                                           'id': 'PROJ_IMAGE_USE_OTP',
                                           'class': '',
                                           'type': 'checkbox',
                                           'label': 'bot 2',
                                           'tooltip': 'Use OTP',
                                           'values': [{ 'value': 1, 'text': 'Use OTP file' }],
                                           'checked': 0
                                       },
                                       {
                                           'type': 'div',
                                           'class': 'clearM'
                                       },
                                       {
                                           'id': 'fieldsetProjImgOTPInn',
                                           'type': 'div',
                                           'class': 'wrapDRS floatLeft',
                                           'objects': [
                                                       {
                                                           'id': 'prImgotpFileInput',
                                                           'type': 'text',
                                                           'class': 'floatLeft'
                                                       },
                                                       {
                                                           'id': 'prImgbrowseOTPFile',
                                                           'type': 'div',
                                                           'class': 'regButtonBr mtop0 mLeftS',
                                                           'title': 'Browse',
                                                           'click': '$(\'#PROJ_IMAGE_OTP_FILE\').val(""); $(\'#PROJ_IMAGE_OTP_FILE\').click();'
                                                       },
                                                      /* {
                                                           'id': 'prImgclearOtpFile',
                                                           'type': 'div',
                                                           'class': 'regButton mtop0 mLeftS floatRight',
                                                           'title': '&nbspClear&nbsp&nbsp',
                                                           'tooltip': 'Clear OTP file from project',
                                                           'click': 'prImgclearOtpFile()',
                                                       },*/
                                                       {

                                                           'id': 'PROJ_IMAGE_OTP_FILE',
                                                           'type': 'file',
                                                           'class': 'hideFile',
                                                           'change': 'postFile(\'PROJ_IMAGE_OTP_FILE\', \'uploadOtpFileSLI\', \'23\');',
                                                       }
                                                       /*,{
                                                           'type': 'div',
                                                           'id': 'encrKeyFileNote',
                                                           'text': "justtext",
                                                           'title': "Important: The key file is saved into the project's directory",
                                                           'class': 'uRed'
                                                       }*/
                                           ]
                                       }
                        ]
                    },
                      {
                          'type': 'div',
                          'class': 'clear'
                      },
                    {
                        'id':'backProjImage',
                        'type':'div',
                        'class': 'regButton floatLeft',
                        'title':'<< &nbsp; &nbsp; Back',
                        'click':'loadWelcome();'
                    },
                    {
                        'id':'setProjImage',
                        'type':'div',
                        'class': 'regButton floatRight',
                        'title':'Program Image'
                    }
                        
                    ]
    },
    {'id':'newproject',
        'title':'Start new project',
        'load':'newprojectLoad();',
        'submit':'alert ("submit"); return false;',
        'connect':true,
        'objects':[
                    {
                        'id':'projectName',
                        'type':'div',
                        'class': 'fieldName',
                        'title':'Project Name'
                    },
                    {
                        'id':'projectNameText',
                        'class':'mbot25',
                        'type':'text',
                        'placeholder':'Project Name',
                        'required':'required'
                    },
                    {
                        'id':'projectNameTextError',
                        'class':'mbot25',
                        'type':'div'                    
                    },
                    {
                        'id':'projectDescription',
                        'type':'div',
                        'class': 'fieldName',
                        'title':'Project Description'
                    },
                    {
                        'id':'projectDescriptionText',
                        'class':'mbot25',
                        'type':'textarea'
                    },
                
                    {
                        'id':'deviceTypeInputWrap',
                        'type':'div',
                        'class': 'inputWrap',
                        'objects':[
                                    {
                                        'id':'deviceType',
                                        'type':'div',
                                        'class': 'fieldName',
                                        'title':'Device Type'
                                    },
                                    {		
                                        'id':'deviceTypeSelect',
                                        'class': 'mright10',
                                        'type': 'select',
                                        //'change': 'deviceTypeSelectChange();',
                                        'values': [ { 'value': 'CC3220S'    , 'text': 'CC3220S' },
                                                    { 'value': 'CC3220R'    , 'text': 'CC3220R' },
                                                    { 'value': 'CC3120R'    , 'text': 'CC3120R' },
                                                    { 'value': 'CC3220SF'   , 'text': 'CC3220SF'},
                                                    { 'value': 'CC3235S'    , 'text': 'CC3235S' },
                                                    { 'value': 'CC3230S'    , 'text': 'CC3230S' },
                                                    { 'value': 'CC3235SF'   , 'text': 'CC3235SF'},
                                                    { 'value': 'CC3230SF'   , 'text': 'CC3230SF'},
                                                    { 'value': 'CC3135R'    , 'text': 'CC3135R' },
                                                    { 'value': 'CC3130R'    , 'text': 'CC3130R' },
                                        ]
                                        
                                    },

                        ]
                    },
                   /* {
                    'id':'deviceSubTypeInputWrap',
                    'type':'div',
                    'class': 'inputWrap',
                    'style': 'display: none;',
                    'objects': [
                         {
                             'id': 'deviceType',
                             'type': 'div',
                             'class': 'fieldName',
                             'title': '&nbsp;'
                         },
                        {
                            'id': 'deviceSubType',
                            'class': '',
                            'type': 'checkbox',
                            'values': [{ 'value': 0, 'text': 'Module' }],
                            'checked': 0
                            
                        }
                    ]},*/

                {
                    'type':'div',
                    'class': 'clear'
                },
                {
                    'type':'div',
                    'class': 'mtop21'
                },
                {
                    'id':'DevelopmentMode',
                    'type':'div',
                    'class': 'fieldName',
                    'title':'Device Mode'
                },
                {
                    'id':'DevelopmentModeToggle',
                    'type':'div',
                    'class': 'slideToggle',
                    'title':'<div class="devMode devModeD">Develop</div><div class="devMode devModeP">Production</div><div class="slider sliderLeft"><div class="inSlider"><i class="fa fa-bars  fa-rotate-90"></i></div></div>',
                    'click':'toogleSlider();'
                },
                {
                    'type':'div',
                    'class': 'mtop21'
                },
                {
                    'id':'backCreateProject',
                    'type':'div',
                    'class': 'regButton floatLeft',
                    'title':'<< &nbsp; &nbsp; Back',
                    'click':'loadWelcome();'
                },
                {
                    'id':'createProject',
                    'type':'div',
                    'class': 'regButton floatRight',
                    'title':'Create Project',
                    'click':'createProjectClicked();'
                },
                    
                {
                    'type':'div',
                    'class': 'clear'
                },
                {
                    'type':'div',
                    'class': 'mtop75'
                }

        ]
    },
    {'id':'projectmanagement',
    'title':'Project Management',
    'load':'projectManagementLoad();',
    'submit':'alert ("submit"); return false;',
    'connect':true,
    'objects': [
                {'type':'fieldset',
                'id':'fieldprojectmanagement',
                'title':'Available Projects',
                'class': 'wrapSection',
                'objects':[
                {
                    'id'    :'projectList',
                    'type'  :'div',
                    //'class' :'fb',
                    'class' :'fb12',
                },
                {
                    'id':'pmicons',
                    'type':'div',
                    'class': 'funcBtn floatRight',
                    'objects':[
                            
                                {
                                    'id':'pmicondelete',
                                    'type':'div',                                       
                                    'class': 'regButtonS tooltip',
                                    'iclass': 'fa fa-trash',
                                    'tooltip':'Delete Selected Project'
                                },
                                {
                                    'id':'pmiconrename',
                                    'type':'div',                                       
                                    'class': 'regButtonS tooltip',
                                    'iclass': 'fa fa-pencil-square',
                                    'tooltip':'Rename Selected Project'
                                },
                                {
                                    'id':'pmiconexport',
                                    'type':'div',                                       
                                    'class': 'regButtonS tooltip',
                                    'iclass': 'fa fa-upload',
                                    'tooltip':'Export Selected Project'
                                },
                                {
                                    'id':'pmiconimport',
                                    'type':'div',                                       
                                    'class': 'regButtonS tooltip',
                                    'iclass': 'fa fa-download',
                                    'tooltip':'Import Project from ZIP file',
                                },
                                {
                                    'id':'pmiconopen',
                                    'type':'div',                                       
                                    'class': 'regButtonS tooltip',
                                    'iclass': 'fa fa-folder-open',
                                    'tooltip':'Open Selected Project'
                                }
                                ,
                                {
                                    'id':'pmimportproject',
                                    'type':'file',
                                    'class':'hideFile'
                                },
                                {
                                    'type':'div',
                                    'id':'pmExportLink'
                                }
                    ]
                },
                {
                    'type':'div',
                    'class': 'mtop21'
                },
                {
                    'id':'backProjManagement',
                    'type':'div',
                    'class': 'regButton db floatLeft',
                    'title':'<< &nbsp; &nbsp; Back',
                    'click':'loadWelcome();'
                }
            ]
                }]
    },
	{'id':'programimageconnected',
	'title':'Generate Image',
	'load':'$(\'#progressAlertDiv\').hide(); loadGenerateImage();',
	'submit':'alert ("submit"); return false;',
	'connect':true,
	'objects':[
                {
                    'id':'createButton',
                    'type':'div',
                    'class': 'regButton mRightM',
                    'title':'Create Image',
                    'click':'createImage();',
                    'tooltip':'Create Image'
				}, 
                {
                    'id':'programImageButton',
                    'type':'div',
                    //'class': 'regButton btnWrapSection',
                    'class': 'regButton',
                    'title':'Program Image (Create & Program)',
                    'click':'createImage(programImageFromProject, true);',
                    'tooltip':'Connect, Create and Program Image'
				}, 
                /*{
					'type':'div',
					'class': 'clear'
				},*/
                {
                    'id':'createOTAButton',
                    'type':'div',
                    'class': 'regButton mleft20',
                    'title':'Create &nbsp;OTA &nbsp;',
                    'click':'createBasicOTA();',
                    'tooltip:':'Create basic OTA image'
				},
				{
					'type':'div',
					'class': 'clear mbot50'
				},
                    
                {
                    'id':'saveImageButton',
                    'type':'div',
                    'class': 'regButton mRightM mtop30',
                    'title':'&nbsp; Save Image&nbsp;',
                    'tooltip':'becomes available after first image creation'
				},
                {
                    'id':'saveImageButtonTxt',
                    'type':'div',
                    'class': 'textAreaDesc',
                    'title':'SLI, TI format, for ImageCreator programming.'
				},
                {
					'type':'div',
					'class': 'clear'
				},
                {
                    'id':'saveUCFButton',
                    'type':'div',
                    'class': 'regButton mRightM mtop50',
                    'title':'&nbsp; Save UCF&nbsp;&nbsp;&nbsp;',
                    'tooltip':'becomes available after first image creation'
				},
                {
                    'id':'saveUCFButtonTxt',
                    'type':'div',
                    'class': 'textAreaDesc',
                    'title':'UCF, TI format, for host programming.'
				},
                {
					'type':'div',
					'class': 'clear'
				},
                {
                    'id':'saveBINButton',
                    'type':'div',
                    'class': 'regButton mRightM mtop50',
                    'title':'&nbsp; Save BIN&nbsp;&nbsp;&nbsp;',
                    'tooltip':'becomes available after first image creation'
				},
                {
                    'id':'saveBINButtonTxt',
                    'type':'div',
                    'class': 'textAreaDesc',
                    'title':'&nbsp; Bin, standard binary image file for Gang programming.&nbsp; '
				},
                {
					'type':'div',
					'class': 'clear'
				},
                {
                    'id':'saveHEXButton',
                    'type':'div',
                    'class': 'regButton mRightM mtop50',
                    'title':'&nbsp; Save HEX&nbsp;&nbsp;&nbsp;',
                    'tooltip':'becomes available after first image creation'
				},
                {
                    'id':'saveHEXButtonTxt',
                    'type':'div',
                    'class': 'textAreaDesc',
                    'title':'Hex, standard intel-hex format file for Gang programming.'
				},
                {
                    'type':'div',
					'id':'lastImageLink'
                },
				{
					'type':'div',
					'id':'progressAlertDiv',
					'class': 'progressAlert',
					'title':'<div class="inProgress">In Progress...</div><div class="progInText">Image Created Successfully!</div>'
				},
         		{
					'type':'div',
					'class': 'mtop75'
				}
		]
	},
//Simplemode
{
    'id': 'simplemode',
    'title': 'Simple View',
    'load': 'localStorage.setItem(\'project\', ""); loadFilesSP(); dropFile(\'#mcuFileInputSimple\');dropFile(\'#spFileInputSimple\');loadCertStore(); loadSimpleMode();', 
    'submit': 'alert ("submit"); return false;',
    'connect': true,
    'objects': [
                 {
                    'type': 'fieldset',
                    'id': 'fieldsetSumSimple',
                    //'title': 'MCU Img',
                    'class': 'wrapSection',
                    'objects': [
                                    {
                                        'type': 'div',
                                        'title': '<b>Project Name</b>',
                                        'class': 'wrapDRS floatLeft'
                                    },
                                    {
                                        'type': 'div',
                                        'id': 'Name_SUM_Simple',
                                        'class': 'wrapDRS floatRight'
                                    },
                                    {
                                        'type': 'div',
                                        'class': 'clearM'
                                    }, /*Clear*/
                                    {
                                        'type': 'div',
                                        'title': '<b>Device Type</b>',
                                        'class': 'wrapDRS floatLeft'
                                    },
                                    {
                                    'type': 'div',
                                    'id': 'DeviceType_SUM_Simple',
                                    'class': 'wrapDRS floatRight'
                                    },
                                    {
                                        'type': 'div',
                                        'class': 'clearM'
                                    }, /*Clear*/
                                    {
                                        'type': 'div',
                                        'class': 'fieldName floatLeft',
                                        'title': '<b>Country Code</b>'
                                    },
                                     {
                                         'type': 'div',
                                         'id': 'COUNTRY_CODE_SUM_SIMPLE',
                                         'class': 'wrapDRS floatRight'
                                     },
                                    {
                                        'type': 'div',
                                        'class': 'clearM'
                                    }, /*Clear*/
                                    {
                                        'type': 'div',
                                        'class': 'fieldName floatLeft',
                                        'title': '<b>Start Role</b>'
                                    },
                                    {
                                        'id': 'SecurityTypeDivSimple',
                                        'type': 'div',
                                        'class': 'wrapDRSi floatRight',
                                        'objects': [
                                          
                                                    {
                                                        'id': 'StartRoleSelectSimple',
                                                        'class': '',
                                                        'type': 'select',
                                                        'change': 'changeRoleSimple();',
                                                        'values': [{ 'value': 2, 'text': 'Access Point' }, { 'value': 3, 'text': 'P2P' }, { 'value': 0, 'text': 'Station' }]
                                                    }
                                                ]
                                    }/*Start Role*/
                                    
                                ]
                 },/* Sum*/
                 {
                 'type': 'fieldset',
                 'id': 'fieldsetMCUImgSimple',
                 'title': 'MCU Img',
                 'class': 'wrapSection',
                 'objects': [
                            {
                                'id': 'fieldsetMCUImgTSimple',
                                'type': 'div',
                                'class': 'wrapDRS floatLeft',
                                'objects': [
                                            {
                                                'id': 'mcuFileInputSimple',
                                                'type': 'text',
                                                'class': 'floatLeft browseWidth'
                                            },
                                            {
                                                'id': 'browseMCUFileSimple',
                                                'type': 'div',
                                                'class': 'regButtonBr mtop0 mLeftS',
                                                'title': 'Browse',
                                                'tooltip':'MCU is automatically signed by TI dummy trusted certificate from SDK playground. To change it, use Advanced mode',
                                                'click': '$(\'#MCU_FILE_SIMPLE\').click();'
                                            },
                                            {
                                                'id': 'MCU_FILE_SIMPLE',
                                                'type': 'file',
                                                'class': 'hideFile',
                                                'change': 'postMCUSimpleBase( $(\'#MCU_FILE_SIMPLE\').get(0).files[0]);',
                                            },
                                            {
                                                'id': 'clearMCUFileSimple',
                                                'type': 'div',
                                                'class': 'regButton mtop0 mLeftS floatRight',
                                                'title': '&nbspClear&nbsp&nbsp',
                                                'tooltip': 'Clear MCU Image AND DUMMY TRUSTED CERTIFICATE from the project!',
                                                'click': 'clearUserFileSimpleMode()'
                                            }
                                ]
                            }, /*MCU File*/                     
                            {
                                'type': 'div',
                                'class': 'clear' 
                            }  /*Clear*/
                            ]
                 },/* MCU Img*/
                 {
                     'type': 'fieldset',
                     'id': 'fieldsetBinServicePackSimple',
                     'title': 'Service Pack File Name',
                     'class': 'wrapSection',
                     'objects': [
                                    {
                                        'id': 'fieldsetBinServicePackFileBrowseSimple',
                                        'type': 'div',
                                        'class': 'wrapDRS floatLeft',
                                        'objects': [
                                                    {
                                                        'id': 'spFileInputSimple',
                                                        'type': 'text',
                                                        'class': 'floatLeft browseWidth'
                                                    },
                                                    {
                                                        'id': 'browseServicePackFileSimple',
                                                        'type': 'div',
                                                        'class': 'regButtonBr mtop0 mLeftS',
                                                        'title': 'Browse',
                                                        'click': '$(\'#SP_FILE_SIMPLE\').click();'
                                                    },/*Browse btn*/
                                                    {
                                                        'id': 'SP_FILE_SIMPLE',
                                                        'type': 'file',
                                                        'class': 'hideFile',
                                                        'change': 'postFile(\'SP_FILE_SIMPLE\', \'uploadProjectSPFile\', \'10\');'
                                                    }, /*SP_FILE_SIMPLE*/
                                                    {
                                                        'id': 'clearServicePackFileSimple',
                                                        'type': 'div',
                                                        'class': 'regButton mtop0 mLeftS floatRight',
                                                        'title': '&nbspClear&nbsp&nbsp',
                                                        'tooltip': 'Clear Service Pack from the project',
                                                        'click': 'clearServicePackFile()'
                                                    } /* Clear btn*/
                                        ]
                                    }
                     ]
                 } /* SP Simple*/ 
               ]
}
]};
var menu = [
    {   'name': '<b>Simple</b>',
        'id': 'idmenugeneral',
        'page': '/index.htm?page=simplemode',
        'checkFunction': 'return false;',
        'visibleFunction': 'return true;',
        'child': [
             {
                 'name': '',
                 'id': 'idmenuSimpleMode',
                 'page': '/index.htm?page=simplemode',
                 'checkFunction': 'return false;',
                 'visibleFunction': 'return true;'
             }
        ]
    },
    {
        'name': '<b>Advanced</b>',
        'id': 'idmenuadvanced',
        'page': '/index.htm?page=advanced',
        'checkFunction': 'return false;',
        'visibleFunction': 'return true;',
        'child': [
            {
                'name': '<b>General - <span id="menuProjectName"></span></b>',
                'id': 'idmenugeneral',
                'page': '/index.htm?page=general',
                'checkFunction': 'return false;',
                'visibleFunction': 'return true;',
                'child': [
                    {
                        'name': 'Settings',
                        'id': 'idmenuGeneralSettings',
                        'page': '/index.htm?page=generalsettings',
                        'checkFunction': 'return false;',
                        'visibleFunction': 'return true;',
                        'child':
                                [
                                    {
                                        'name': '',
                                        'id': 'idmenuGeneralSettingsStubbin',
                                        'page': '/index.htm?page=generalsettings',
                                        'checkFunction': 'return false;',
                                        'visibleFunction': 'return true;'
                                    }
                                ]
                    }

                ]

            },
            {
                'name': '<b>System Setting</b>',
                'id': 'idmenuSystemSettingl',
                'page': '/index.htm?page=systemsettings',
                'checkFunction': 'return false;',
                'visibleFunction': 'return true;',
                'child': [
                    {
                        'name': 'Device',
                        'id': 'idmenuDevice',
                        'page': '/index.htm?page=systemsettingsdevice',
                        'checkFunction': 'return false;',
                        'visibleFunction': 'return true;',
                        'child': [
                            /*{'name' :'Supported Modes',
                            'id': 'idmenuSupportedModes',
                            'page': '/Mac.html',
                            'checkFunction':'return false;',
                            'visibleFunction':'return true;'
                            },
                            {'name' :'Service Pack',
                            'id': 'idmenuServicePack',
                            'page': '/index.htm?page=deviceservicepack',
                            'checkFunction':'return false;',
                            'visibleFunction':'return true;'
                            },*/
                            {
                                'name': 'Radio Settings',
                                'id': 'idmenuRadioSettings',
                                'page': '/index.htm?page=systemsettingsdeviceRadioSettings',
                                'checkFunction': 'return false;',
                                'visibleFunction': 'return true;',
                                'child':
                                            [
                                                {
                                                    'name': '',
                                                    'id': 'idmenuRadioSettingsStubbin',
                                                    'page': '/index.htm?page=systemsettingsdeviceRadioSettings',
                                                    'checkFunction': 'return false;',
                                                    'visibleFunction': 'return true;'
                                                }
                                            ]
                            },
                            {
                                'name': 'Device Identity',
                                'id': 'idmenuDiceSettings',
                                'page': '/index.htm?page=systemsettingsdeviceDiceSettings',
                                'checkFunction': 'return false;',
                                'visibleFunction': 'return true;',
                                //'visibleFunction': 'isGen3()',
                                'child':
                                            [
                                                {
                                                    'name': '',
                                                    'id': 'idmenuDeviceSettingsStubbin',
                                                    'page': '/index.htm?page=systemsettingsdeviceDiceSettings',
                                                    'checkFunction': 'return false;',
                                                    //'visibleFunction': 'isGen3()'
                                                    'visibleFunction': 'return true;'
                                                }
                                            ]
                            }/*,
					{'name' :'Policies',
					'id': 'idmenuPolicies',
					'page': '/index.htm?page=systemsettingsdevicepolicies',
					'checkFunction':'return false;',
					'visibleFunction':'return true;'
					}*/
                        ]
                    },
                    {
                        'name': 'Role Settings',
                        'id': 'idmenuDeviceRoleSettings',
                        'page': '/index.htm?page=devicerolesettings',
                        'checkFunction': 'return false;',
                        'visibleFunction': 'return true;',
                        'child': [
                                    {
                                        'name': 'General Settings',
                                        'id': 'idmenuDeviceRoleGeneralSettings',
                                        'page': '/index.htm?page=devicerolesettingsgeneralsettings',
                                        'checkFunction': 'return false;',
                                        'visibleFunction': 'return true;',
                                        'child':
                                                    [
                                                        {
                                                            'name': '',
                                                            'id': 'idmenuDeviceRoleGeneralSettingsStubbin',
                                                            'page': '/index.htm?page=devicerolesettingsgeneralsettings',
                                                            'checkFunction': 'return false;',
                                                            'visibleFunction': 'return true;'
                                                        }
                                                    ]
                                    },//General Settings
                                    {
                                        'name': 'STA/Wi-Fi® Direct Device',
                                        'id': 'idmenuSTAP2PDevice',
                                        'page': '/index.htm?page=devicerolesettingsstadevice',
                                        'checkFunction': 'return false;',
                                        'visibleFunction': 'return true;',
                                        'child': [
                                                   {
                                                       'name': 'WLAN Settings',
                                                       'id': 'idmenuDeviceRoleSTAWlanSettings',
                                                       'page': '/index.htm?page=devicerolesettingsstap2pwlansettings',
                                                       'checkFunction': 'return false;',
                                                       'visibleFunction': 'return true;'
                                                   },
                                                    {
                                                        'name': 'Network Settings',
                                                        'id': 'idmenuDeviceRoleSettingsSTANetworkSettings',
                                                        'page': '/index.htm?page=devicerolesettingsstap2pnetworksettings',
                                                        'checkFunction': 'return false;',
                                                        'visibleFunction': 'return true;'
                                                    }
                                        ]
                                    },//STA/Wi-Fi® Direct Device
                                    {
                                        'name': 'AP/Wi-Fi® Direct GO',
                                        'id': 'idmenuAPP2PDevice',
                                        'page': '/index.htm?page=devicerolesettingsapdevice',
                                        'checkFunction': 'return false;',
                                        'visibleFunction': 'return true;',
                                        'child': [
                                                    {
                                                        'name': 'WLAN Settings',
                                                        'id': 'idmenuDeviceRoleAPWlanSettings',
                                                        'page': '/index.htm?page=devicerolesettingsapp2pwlansettings',
                                                        'checkFunction': 'return false;',
                                                        'visibleFunction': 'return true;'
                                                    },
                                                    {
                                                        'name': 'Network Settings',
                                                        'id': 'idmenuDeviceRoleAPNetworkSettings',
                                                        'page': '/index.htm?page=devicerolesettingsapp2pnetworksettings',
                                                        'checkFunction': 'return false;',
                                                        'visibleFunction': 'return true;'
                                                    }/*,
                                {'name' :'Black List',
                                'id': 'idmenuDeviceRoleSettingsAPBlackList',
                                'page': '/index.htm?page=devicerolesettingsapblacklist',
                                'checkFunction':'return false;',
                                'visibleFunction':'return true;'
                                }*/
                                        ]
                                    } //AP/Wi-Fi® Direct GO
                        ]
                    },
                    {
                        'name': 'Network Applications',
                        'id': 'idmenuNetworkApplicationss',
                        'page': '/index.htm?page=networkapplications',
                        'checkFunction': 'return false;',
                        'visibleFunction': 'return true;',
                        'child': [
                                {
                                    'name': '',
                                    'id': 'idmenuNetworkAppStubbin',
                                    'page': '/index.htm?page=networkapplications',
                                    'checkFunction': 'return false;',
                                    'visibleFunction': 'return true;'
                                },
                                {
                                    'name': 'HTTP Server',
                                    'id': 'idmenuHTTPDServer',
                                    'page': '/index.htm?page=httpdserver',
                                    'checkFunction': 'return false;',
                                    'visibleFunction': 'return true;',
                                    'child': [
                                                {
                                                    'name': '',
                                                    'id': 'idmenuHttpServerStubbin',
                                                    'page': '/index.htm?page=httpdserver',
                                                    'checkFunction': 'return false;',
                                                    'visibleFunction': 'return true;'
                                                }]
                                }
                        ]
                    }

                ]
            },
            {
                'name': '<b>Files</b>',
                'id': 'idmenuFiles',
                'page': '/index.htm?page=files',
                'checkFunction': 'return false;',
                'visibleFunction': 'return true;',
                'child': [
                            {
                                'name': 'User Files',
                                'id': 'idmenuBinUserApp',
                                'page': '/index.htm?page=filesuserfiles',
                                'checkFunction': 'return false;',
                                'visibleFunction': 'return checkDeviceType();',
                            },
                            {
                                'name': 'Service Pack',
                                'id': 'idmenuBinSP',
                                'page': '/index.htm?page=binservicepack',
                                'checkFunction': 'return false;',
                                'visibleFunction': 'return true;',
                            },
                            {
                                'name': 'Trusted Root-Certificate Catalog',
                                'id': 'idmenuDevice',
                                'page': '/index.htm?page=bincertstore',
                                'checkFunction': 'return false;',
                                'visibleFunction': 'return true;',
                            }
                ]
            },
        ]
    }
];