/// <reference types="cypress" />

Cypress.on('uncaught:exception', (err, runnable) => {
  // returning false here prevents Cypress from
  // failing the test
  return false
})

describe('Testing attribute search', () => {
  it('create a new endpoint and click on configure to open attributes of endpoint', () => {
    cy.fixture('baseurl').then((data) => {
      cy.visit(data.baseurl)
    })
    cy.fixture('data').then((data) => {
      cy.gotoAttributePage(data.endpoint1, data.cluster1)
    })
  })
  it(
    'check existance of ZCL version and application version',
    { retries: { runMode: 2, openMode: 2 } },
    () => {
      cy.fixture('data').then((data) => {
        cy.get('tbody')
          .children()
          .should('contain', data.attribute1)
          .and('contain', data.attribute2)
      })
    }
  )
  it('Search for application', () => {
    cy.fixture('data').then((data) => {
      cy.get(
        '.q-py-none > .q-field > .q-field__inner > .q-field__control > .q-field__control-container > input'
      )
        .clear({ force: true })
        .type(data.searchString1, { force: true })
    })
  })
  it('check if search result is correct', () => {
    cy.fixture('data').then((data) => {
      cy.get('tbody').children().contains(data.attribute1).should('not.exist')
      cy.get('tbody').children().should('contain', data.attribute2)
    })
  })
})
