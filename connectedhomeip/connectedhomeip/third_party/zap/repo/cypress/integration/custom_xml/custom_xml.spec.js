/// <reference types="cypress" />

const { find } = require('underscore')
const rendApi = require('../../../src-shared/rend-api.js')
const path = require('path')

Cypress.on('uncaught:exception', (err, runnable) => {
  // returning false here prevents Cypress from
  // failing the test
  return false
})
describe('Check ZCL Extension functionalities', () => {
  it('Verify Device Type display value', () => {
    cy.fixture('baseurl').then((data) => {
      cy.visit(data.baseurl)
    })
    cy.window()
      .then(function (window) {
        window[rendApi.GLOBAL_SYMBOL_EXECUTE](
          rendApi.id.open,
          path.join(__dirname, 'custom_xml.zap')
        )
      })
      .then(() => {
        cy.get(
          ':nth-child(3) > .q-card > .q-list > :nth-child(2) > :nth-child(2)'
        ).should('contain', 'Bead Device Type Name (0xDEAD)')

        cy.get(
          ':nth-child(3) > .q-card > .q-list > :nth-child(4) > :nth-child(2)'
        ).should('contain', '0xBEEF')

        cy.get(
          '.col-4 > .q-field__inner > .q-field__control > .q-field__control-container > input'
        )
          .clear({ force: true })
          .type('Bead', { force: true })

        cy.get('.q-table--col-auto-width').should('contain', 'Custom Bead')
        cy.get('.q-tr > :nth-child(4)').should('contain', '0xFCAA')
        cy.get('.q-tr > :nth-child(5)').should('contain', '0xBEAD')
      })
  })
})
