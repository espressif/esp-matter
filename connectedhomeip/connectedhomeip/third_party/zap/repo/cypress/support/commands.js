const rendApi = require('../../src-shared/rend-api.js')
const _ = require('lodash')

Cypress.Commands.add('addEndpoint', (name) => {
  cy.get('[data-test="add-new-endpoint"]').click()
  cy.get(
    '[data-test="select-endpoint-input"]'
  ).click()
  cy.get('div').contains(name).click({ force: true })
  cy.get('button').contains('Create').click()
})

Cypress.Commands.add('gotoAttributePage', (endpoint, cluster, nth = 0) => {
  if (endpoint) cy.addEndpoint(endpoint)
  cy.get('[data-test="Cluster"]').should('contain', cluster)
  cy.get('div').contains(cluster).click({ force: true })
  cy.get(
    `#${cluster} > .q-expansion-item__container > .q-expansion-item__content > :nth-child(1) > .q-table__container > .q-table__middle > .q-table > tbody > .text-weight-bolder > :nth-child(7) > .q-btn > .q-btn__wrapper > .q-btn__content > .notranslate`
  )
    .eq(nth)
    .click({ force: true })
})

Cypress.Commands.add('gotoAttributesTab', () => {
  cy.get(':nth-child(1) > .q-tab__content').click()
})

Cypress.Commands.add('gotoAttributeReportingTab', () => {
  cy.get(':nth-child(2) > .q-tab__content').click()
})

Cypress.Commands.add('gotoCommandsTab', () => {
  cy.get(':nth-child(3) > .q-tab__content').click()
})

Cypress.Commands.add('rendererApi', (...args) => {
  cy.window().then(function (window) {
    const log = Cypress.log({
      name: 'rendererApi',
      displayName: 'RendererApi',
      message: [`🚀 ${args[0]}(${args.slice(1)})`],
    })

    log.snapshot('before')
    window[rendApi.GLOBAL_SYMBOL_EXECUTE].apply(null, args)
    log.snapshot('after')
  })
})
