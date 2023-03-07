/// <reference types="cypress" />
Cypress.on('uncaught:exception', (err, runnable) => {
  // returning false here prevents Cypress from
  // failing the test
  return false
})

describe('Testing Editing endpoints', () => {
  it('create a new endpoint', () => {
    cy.fixture('baseurl').then((data) => {
      cy.visit(data.baseurl)
    })
    cy.fixture('data').then((data) => {
      cy.addEndpoint(data.endpoint1)
    })
  })
  it('edit endpoint', { retries: { runMode: 2, openMode: 2 } }, () => {
    cy.get('[data-test="edit-endpoint"]').click()
    cy.get(
      '.q-form > .q-select > .q-field__inner > .q-field__control > .q-field__control-container'
    ).click()
    cy.fixture('data').then((data) => {
      cy.get('div').contains(data.endpoint2).click()
    })
    cy.get('button').contains('Save').click()
  })
  it(
    'Check if edit is successfull',
    { retries: { runMode: 2, openMode: 2 } },
    () => {
      cy.fixture('data').then((data) => {
        cy.get('aside').children().should('contain', data.endpoint2)
      })
    }
  )
})
