/// <reference types="cypress" />

Cypress.on('uncaught:exception', (err, runnable) => {
  // returning false here prevents Cypress from
  // failing the test
  return false
})

describe('Testing disabling enabled attributes', () => {
  it('create a new endpoint and click on configure to open attributes of endpoint', () => {
    cy.fixture('baseurl').then((data) => {
      cy.visit(data.baseurl)
    })
    cy.fixture('data').then((data) => {
      cy.gotoAttributePage(data.endpoint1, data.cluster1)
    })
  })
  it(
    'getting enabled attribute and disable it',
    { retries: { runMode: 2, openMode: 2 } },
    () => {
      cy.get(
        '.table_body:first > :nth-child(2) > .q-mt-xs > .q-toggle__inner'
      ).click()
      cy.get(
        '.table_body:first > :nth-child(2) > .q-mt-xs > .q-toggle__inner'
      ).should('be.visible')
      cy.get(
        '.table_body:first > :nth-child(2) > .q-mt-xs > .q-toggle__inner'
      ).click()
      cy.contains(
        '.table_body:first > :nth-child(2) > .q-mt-xs > .q-toggle__inner'
      ).should('not.exist')
    }
  )
})
