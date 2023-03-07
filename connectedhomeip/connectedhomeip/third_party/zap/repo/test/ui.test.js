/**
 *
 *    Copyright (c) 2020 Silicon Labs
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
 *
 *
 * @jest-environment jsdom
 */

import {
  Quasar,
  QBtn,
  QFile,
  QSplitter,
  QSpace,
  QPageContainer,
  QDrawer,
  QToolbar,
  QToolbarTitle,
  QHeader,
  QTable,
  QLayout,
  QInput,
  QIcon,
  QToggle,
  QCardSection,
  QCard,
  QTabs,
  QTab,
  QForm,
  QSelect,
  QSeparator,
  QList,
  QBreadcrumbs,
  QBreadcrumbsEl,
  QTooltip,
  QField,
  QCardActions,
  QDialog,
  QItem,
  QTree,
  ClosePopup,
  QImg,
  QTabPanel,
  QTabPanels,
  QExpansionItem,
  QBtnDropdown,
  QScrollArea,
  QScrollObserver,
  QCheckbox,
} from 'quasar'
import Vue from 'vue'

import { shallowMount } from '@vue/test-utils'
import ZapStore from '../src/store/index.js'

import ZclAttributeManager from '../src/components/ZclAttributeManager.vue'
import ZclAttributeReportingManager from '../src/components/ZclAttributeReportingManager.vue'
import ZclClusterManager from '../src/components/ZclClusterManager.vue'
import ZclClusterView from '../src/components/ZclClusterView.vue'
import ZclCommandManager from '../src/components/ZclCommandManager.vue'
import ZclCreateModifyEndpoint from '../src/components/ZclCreateModifyEndpoint.vue'
import ZclDomainClusterView from '../src/components/ZclDomainClusterView.vue'
import ZclEndpointCard from '../src/components/ZclEndpointCard.vue'
import ZclEndpointManager from '../src/components/ZclEndpointManager.vue'
import ZclGeneralOptionsBar from '../src/components/ZclGeneralOptionsBar.vue'
import ZclInformationSetup from '../src/components/ZclInformationSetup.vue'
import ZclClusterLayout from '../src/layouts/ZclClusterLayout.vue'
import ZclConfiguratorLayout from '../src/layouts/ZclConfiguratorLayout.vue'
import Error404 from '../src/pages/Error404.vue'
import Preference from '../src/pages/Preference.vue'
import PreferenceGeneration from '../src/pages/PreferenceGeneration.vue'
import PreferenceUser from '../src/pages/PreferenceUser.vue'
import PreferencePackage from '../src/pages/PreferencePackage.vue'
import ZclSettings from '../src/pages/ZclSettings.vue'
import UcComponentSetup from '../src/components/UcComponentSetup.vue'
import ZclCustomZclView from '../src/components/ZclCustomZclView.vue'
import About from '../src/pages/About.vue'
import ZclLayout from '../src/layouts/ZclLayout.vue'
import { timeout } from './test-util.js'

const observable = require('../src/util/observable.js')

Vue.use(Quasar, {
  components: {
    QBtn,
    QFile,
    QSplitter,
    QSpace,
    QPageContainer,
    QDrawer,
    QToolbar,
    QToolbarTitle,
    QHeader,
    QTable,
    QLayout,
    QInput,
    QToggle,
    QCardSection,
    QCard,
    QTabs,
    QTab,
    QForm,
    QSelect,
    QSeparator,
    QList,
    QBreadcrumbs,
    QBreadcrumbsEl,
    QTooltip,
    QField,
    QCardActions,
    QDialog,
    QItem,
    QIcon,
    QTree,
    QImg,
    QTabPanel,
    QTabPanels,
    QExpansionItem,
    QBtnDropdown,
    QScrollArea,
    QScrollObserver,
    QCheckbox,
  },
  directives: {
    ClosePopup,
  },
})

describe('Component mounting test', () => {
  test(
    'ZclAttributeManager',
    () => {
      const wrapper = shallowMount(ZclAttributeManager, { store: ZapStore() })
      expect(wrapper.html().length).toBeGreaterThan(100)
    },
    timeout.short()
  )
  test(
    'ZclAttributeReportingManager',
    () => {
      const wrapper = shallowMount(ZclAttributeReportingManager, {
        store: ZapStore(),
      })
      expect(ZclAttributeReportingManager.data()).not.toBe(null)
      expect(wrapper.html().length).toBeGreaterThan(100)
    },
    timeout.short()
  )
  test(
    'ZclClusterManager',
    () => {
      const wrapper = shallowMount(ZclClusterManager, { store: ZapStore() })
      expect(wrapper.html().length).toBeGreaterThan(100)
    },
    timeout.short()
  )
  test(
    'ZclClusterView',
    () => {
      const wrapper = shallowMount(ZclClusterView, { store: ZapStore() })
      expect(wrapper.html().includes('Endpoint')).toBe(true)
    },
    timeout.short()
  )
  test(
    'ZclCommandManager',
    () => {
      const wrapper = shallowMount(ZclCommandManager, { store: ZapStore() })
      expect(wrapper.html().length).toBeGreaterThan(100)
    },
    timeout.short()
  )
  test(
    'ZclCreateModifyEndpoint',
    () => {
      const wrapper = shallowMount(ZclCreateModifyEndpoint, {
        store: ZapStore(),
      })
      expect(wrapper.html().length).toBeGreaterThan(100)
    },
    timeout.short()
  )
  test(
    'ZclDomainClusterView',
    () => {
      const wrapper = shallowMount(ZclDomainClusterView, { store: ZapStore() })
      expect(wrapper.html().length).toBeGreaterThan(100)
    },
    timeout.short()
  )
  test(
    'ZclEndpointCard',
    () => {
      const wrapper = shallowMount(ZclEndpointCard, { store: ZapStore() })
      expect(wrapper.html().length).toBeGreaterThan(100)
    },
    timeout.short()
  )
  test(
    'ZclEndpointManager',
    () => {
      const wrapper = shallowMount(ZclEndpointManager, { store: ZapStore() })
      expect(wrapper.html().length).toBeGreaterThan(100)
    },
    timeout.short()
  )
  test(
    'ZclGeneralOptionsBar',
    () => {
      const wrapper = shallowMount(ZclGeneralOptionsBar, { store: ZapStore() })
      expect(wrapper.html().length).toBeGreaterThan(100)
    },
    timeout.short()
  )
  test(
    'ZclInformationSetup',
    () => {
      const wrapper = shallowMount(ZclInformationSetup, { store: ZapStore() })
      expect(wrapper.html().length).toBeGreaterThan(100)
    },
    timeout.short()
  )
  test(
    'ZclClusterLayout',
    () => {
      const wrapper = shallowMount(ZclClusterLayout, { store: ZapStore() })
      expect(wrapper.html().length).toBeGreaterThan(90)
    },
    timeout.short()
  )
  test(
    'ZclLayout',
    () => {
      const wrapper = shallowMount(ZclLayout, { store: ZapStore() })
      expect(wrapper.html().length).toBeGreaterThan(90)
    },
    timeout.short()
  )
  test(
    'ZclConfiguratorLayout',
    () => {
      const wrapper = shallowMount(ZclConfiguratorLayout, { store: ZapStore() })
      expect(wrapper.html().length).toBeGreaterThan(100)
    },
    timeout.short()
  )
  test(
    'Error404',
    () => {
      const wrapper = shallowMount(Error404, { store: ZapStore() })
      expect(wrapper.html().length).toBeGreaterThan(50)
    },
    timeout.short()
  )
  test(
    'Preference',
    () => {
      const wrapper = shallowMount(Preference, { store: ZapStore() })
      expect(wrapper.html().length).toBeGreaterThan(50)
    },
    timeout.short()
  )
  test(
    'PreferenceGeneration',
    () => {
      const wrapper = shallowMount(PreferenceGeneration, { store: ZapStore() })
      expect(wrapper.html().length).toBeGreaterThan(50)
    },
    timeout.short()
  )
  test(
    'PreferenceUser',
    () => {
      const wrapper = shallowMount(PreferenceUser, { store: ZapStore() })
      expect(wrapper.html().length).toBeGreaterThan(50)
    },
    timeout.short()
  )
  test(
    'PreferencePackage',
    () => {
      const wrapper = shallowMount(PreferencePackage, { store: ZapStore() })
      expect(wrapper.html().length).toBeGreaterThan(50)
    },
    timeout.short()
  )
  test(
    'ZclSettings',
    () => {
      const wrapper = shallowMount(ZclSettings, { store: ZapStore() })
      expect(wrapper.html().length).toBeGreaterThan(50)
    },
    timeout.short()
  )
  test(
    'UcComponentSetup',
    () => {
      const wrapper = shallowMount(UcComponentSetup, { store: ZapStore() })
      expect(wrapper.html().length).toBeGreaterThan(50)
    },
    timeout.short()
  )
  test(
    'ZclCustomZclView',
    () => {
      const wrapper = shallowMount(ZclCustomZclView, { store: ZapStore() })
      expect(wrapper.html().length).toBeGreaterThan(50)
    },
    timeout.short()
  )
  test(
    'About',
    () => {
      const wrapper = shallowMount(About, { store: ZapStore() })
      expect(wrapper.html().length).toBeGreaterThan(50)
    },
    timeout.short()
  )
})

describe('DOM tests', () => {
  let observedValue = null

  test(
    'Observables',
    async () => {
      const wrapper = shallowMount(About, { store: ZapStore() })
      observable.setObservableAttribute('x', 'value0')
      expect(observable.getObservableAttribute('x')).toEqual('value0')
      observable.observeAttribute('x', (value) => {
        observedValue = value
      })
      expect(observedValue).toBe(null)
      return new Promise((resolve, reject) => {
        observable.setObservableAttribute('x', 'value1')
        resolve()
      })
        .then(() => {
          return new Promise((resolve, reject) => {
            setTimeout(() => {
              resolve(observedValue)
            }, 10) // Wait for 10 ms. It should settle by then.
          })
        })
        .then((value) => {
          expect(value).toEqual('value1')
        })
    },
    timeout.short()
  )
})
