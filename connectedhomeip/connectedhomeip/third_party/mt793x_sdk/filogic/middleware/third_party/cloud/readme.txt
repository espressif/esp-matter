Brief:          This module is the implementation of accessing to cloud platform, include Alibaba's alink, Jingdong's joylink and Wechat's airkiss.
Usage:          GCC: For alink feature, please include the module with "include $(SOURCE_DIR)/middleware/third_party/cloud/ali_alink/module.mk in your GCC project Makefile and set MTK_ALINK_ENABLE to "y".
                For joylink feature, please include the module with "include $(SOURCE_DIR)/middleware/third_party/cloud/jd_joylink/module.mk in your GCC project Makefile and set MTK_JOYLINK_ENABLE to "y".
                For airkiss feature, please include the module with "include $(SOURCE_DIR)/middleware/third_party/cloud/tencent_weixin/module.mk in your GCC project Makefile and set MTK_AIRKISS_ENABLE to "y".
                KEIL: Not support.
                IAR: Not support.
Dependency:     Please also include LwIP module.
Notice:         If you want support alink feature, please set MTK_ALINK_ENABLE to "y" in the "feature_alink.mk".
                If you want to support original alink cloud feature. please set MTK_ALINK_SDS_ENABLE to "n" in the "feature_alink.mk".
                If you want to support alink cloud SDS feature. please set MTK_ALINK_SDS_ENABLE to "y" in the "feature_alink.mk".
Relative doc:   Alink official website - https://open.alink.aliyun.com.
                Alink develop document - http://smart.aliyun.com/business/help/contract.htm?spm=0.0.0.0.X9I3z2.
                Joylink official website - http://devsmart.jd.com
                Joylink develop document - http://devsmart.jd.com/dev/index
                Airkiss official website - http://iot.weixin.qq.com/
                Airkiss develop document - http://iot.weixin.qq.com/wiki/new/index.html
Example project:Please find the project under project folder with cloud_ prefix.



