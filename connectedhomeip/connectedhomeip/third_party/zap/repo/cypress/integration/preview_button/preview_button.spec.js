/// <reference types="cypress" />
Cypress.on('uncaught:exception', (err, runnable) => {
  // returning false here prevents Cypress from
  // failing the test
  return false
})
describe('Check preview buttton', () => {
  it('adding a new endpoint', () => {
    cy.fixture('baseurl').then((data) => {
      cy.visit(data.baseurl)
    })
    cy.fixture('data').then((data) => {
      cy.addEndpoint(data.endpoint1)
    })
  })
  it(
    'Checking preview button',
    { retries: { runMode: 2, openMode: 2 } },
    () => {
      cy.get('[data-test="preview"]').click()
      cy.get('[data-test="select-file-in-preview"]')
        .click()
      cy.fixture('data').then((data) => {
        cy.get('.q-list > div').should('contain', data.previewBtnData)
      })
    }
  )
})
