/// <reference types="cypress" />

Cypress.on('uncaught:exception', (err, runnable) => {
  // returning false here prevents Cypress from
  // failing the test
  return false
})

describe('Testing enabled clusters amount', () => {
  it(
    'create a new endpoint and get amount of enabled clusters',
    { retries: { runMode: 2, openMode: 2 } },
    () => {
      cy.fixture('baseurl').then((data) => {
        cy.visit(data.baseurl)
      })
      cy.fixture('data').then((data) => {
        cy.addEndpoint(data.endpoint1, data.cluster1)
      })
      cy.get('[data-test="endpoint-enabled-clusters-amount"]').then(($div) => {
        const num1 = parseFloat($div.text())
        cy.fixture('data').then((data) => {
          cy.get('.q-page-container > div')
          .children()
          .should('contain', data.cluster1)
        })
        cy.get('div').contains('General').click({force: true})
        cy.get('div').children().contains('Server').its('length').then(res=>{
          if(res > 0){
            cy.get('div').children().contains('Not Enabled').first().click()
            cy.get('.q-virtual-scroll__content > :nth-child(3)')
              .contains('Server')
              .click()
          }
      });
        
      })
    }
  )
  it(
    'checks if number is updated',
    { retries: { runMode: 2, openMode: 2 } },
    () => {
      cy.fixture('data').then((data) => {
        cy.get('[data-test="endpoint-enabled-clusters-amount"]').then(($div2) => {
          const num2 = parseFloat($div2.text())
          expect(num2).to.eq(Number(data.availableClusters1))
        })
      })
     
    }
  )
})