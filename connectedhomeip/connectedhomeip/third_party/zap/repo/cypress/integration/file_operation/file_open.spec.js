/// <reference types="cypress" />

const path = require('path')
const rendApi = require('../../../src-shared/rend-api.js')
let testFile = path.resolve(
  Cypress.config('projectRoot'),
  'test/resource/three-endpoint-device.zap'
)

Cypress.on('uncaught:exception', (err, runnable) => {
  // returning false here prevents Cypress from
  // failing the test
  return false
})
describe('Check theme functionality', () => {
  beforeEach(() => {
    cy.fixture('baseurl').then((data) => {
      cy.visit(data.baseurl)
    })
  })

  it('Checkig for dirty flag after opening config', () => {
    cy.rendererApi(rendApi.id.open, testFile)
    cy.get('.q-list > :nth-child(4) > :nth-child(2)').should('have.length', 1)

    cy.window().then(function (window) {
      return window[rendApi.GLOBAL_SYMBOL_EXECUTE](rendApi.id.isDirty).then(
        (isDirty) => expect(isDirty).to.be.false
      )
    })
  })
})
