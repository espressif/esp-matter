/// <reference types="cypress" />
Cypress.on('uncaught:exception', (err, runnable) => {
  // returning false here prevents Cypress from
  // failing the test
  return false
})

describe('Testing Dimmable Light workflow', () => {
  it('create a Dimmable Light endpoint', () => {
    cy.fixture('baseurl').then((data) => {
      cy.visit(data.baseurl)
    })
    cy.get('[data-test="add-new-endpoint"]').click()
    cy.fixture('data').then((data) => {
      cy.get(
        '[data-test="select-endpoint-input"]'
      )
        .click()
        .clear({ force: true })
        .type('dimmable', { force: true })
      cy.get('div').contains(data.endpoint6).click({ force: true })
    })
    cy.get('button').contains('Create').click()
  })
  it('Search for the cluster', () => {
    cy.fixture('data').then((data) => {
      cy.get(
        '[data-test="search-clusters"]'
      )
        .clear({ force: true })
        .type(data.cluster4)
    })
  })
  it('Enabling Client & Server', () => {
    cy.get(
      ':nth-child(6) > .q-field > .q-field__inner > .q-field__control'
    ).click()
    cy.fixture('data').then((data) => {
      cy.get('.q-item__section > .q-item__label').contains(data.server2).click()
    })
  })
  it('Check Configuration page', () => {
    cy.get(
      ':nth-child(7) > .q-btn > .q-btn__wrapper > .q-btn__content > .notranslate'
    ).click()
    cy.fixture('data').then((data) => {
      cy.get('tr.table_body').contains(data.attribute3).should('be.visible')
    })
  })
})
