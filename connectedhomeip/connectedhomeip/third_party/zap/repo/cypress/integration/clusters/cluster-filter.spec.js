/// <reference types="cypress" />

Cypress.on('uncaught:exception', (err, runnable) => {
  // returning false here prevents Cypress from
  // failing the test
  return false
})

describe('Testing cluster filters', () => {
  it('create a new endpoint', { retries: { runMode: 2, openMode: 2 } }, () => {
    cy.fixture('baseurl').then((data) => {
      cy.visit(data.baseurl)
    })
    cy.fixture('data').then((data) => {
      cy.addEndpoint(data.endpoint1, data.cluster1)
    })
  })
  it(
    'filter enabled clusters and check clusters',
    { retries: { runMode: 2, openMode: 2 } },
    () => {
      cy.get(
        '[data-test="filter-input"]'
      ).click()
      cy.get('.q-virtual-scroll__content > :nth-child(3)').click()
      cy.fixture('data').then((data) => {
        cy.get('tbody').children().contains(data.cluster2).should('not.exist')
      })
    }
  )
  it(
    'enable power configuration cluster',
    { retries: { runMode: 2, openMode: 2 } },
    () => {
      cy.get(
        '[data-test="filter-input"]'
      ).click({force:true})
      cy.get('.q-virtual-scroll__content > :nth-child(1)').click()
      cy.fixture('data').then((data) => {
        cy.get('tbody').children().should('contain', data.cluster2)
      })
      cy.get(
        '#General > .q-expansion-item__container > .q-expansion-item__content > :nth-child(1) > .q-table__container > .q-table__middle > .q-table > tbody > :nth-child(2) > :nth-child(6) > .q-field > .q-field__inner > .q-field__control'
      ).click()
      cy.fixture('data').then((data) => {
        cy.get('.q-virtual-scroll__content > :nth-child(3)')
          .contains(data.server1)
          .click()
      })
      cy.get(
        '.bar > :nth-child(1) > :nth-child(2) > .q-field > .q-field__inner > .q-field__control'
      ).click({ force: true })
    }
  )
  it(
    'checks if power configuration exists',
    { retries: { runMode: 2, openMode: 2 } },
    () => {
      cy.get('.q-virtual-scroll__content > :nth-child(3)').click()
      cy.fixture('data').then((data) => {
        cy.get('tbody').children().should('contain', data.cluster2)
      })
    }
  )
})
