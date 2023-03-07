/// <reference types="cypress" />

Cypress.on('uncaught:exception', (err, runnable) => {
  // returning false here prevents Cypress from
  // failing the test
  return false
})

// Description:
//   In Zigbee, endpoints that implement the same Cluster have special
// effects on the Cluster specific attributes and commands.
// The attributes and commands behave as global variables. There is only
// one global cluster specific attribute or command value/setting shared across endpoints.
// They are not endpoint specific.

describe('Testing endpoints sharing attribute values', () => {
  it('create Endpoint 1', () => {
    cy.fixture('baseurl').then((data) => {
      cy.visit(data.baseurl)
    })
    cy.gotoAttributePage('Billing Unit (0x0203)', 'General')
  })
  it(
    'set initial values on Endpoint 1',
    { retries: { runMode: 2, openMode: 2 } },
    () => {
      // Set ZCL version - 1
      cy.get(
        ':nth-child(1) > [style="min-width: 180px;"] > .q-field > .q-field__inner .q-field__control > .q-field__control-container > input'
      )
        .clear({ force: true })
        .type('1', { force: true })

      // Set power source - 22
      cy.get(
        ':nth-child(8) > [style="min-width: 180px;"] > .q-field > .q-field__inner .q-field__control > .q-field__control-container > input'
      )
        .clear({ force: true })
        .type('22', { force: true })

      // Set cluster revision - 33
      cy.get(
        ':nth-child(19) > [style="min-width: 180px;"] > .q-field > .q-field__inner .q-field__control > .q-field__control-container > input'
      )
        .clear({ force: true })
        .type('33', { force: true })

      // Set ResetToFactoryDefaults command
      cy.gotoCommandsTab()
      cy.get(':nth-child(3) > .q-mt-xs > .q-checkbox__inner > input').check({
        force: true,
      })

      // Enable "ZCL version"
      cy.gotoAttributeReportingTab()
      cy.get(
        '[data-cy="Attributes Reporting"] > .q-table__middle > .q-table > .q-virtual-scroll__content > :nth-child(1) > :nth-child(2) > .q-mt-xs > .q-toggle__inner'
      )
        .find('input')
        .check({ force: true })
      cy.get(
        '[data-cy="Attributes Reporting"] > .q-table__middle > .q-table > .q-virtual-scroll__content > :nth-child(1) > :nth-child(2) > .q-mt-xs > .q-toggle__inner'
      ).should('be.visible')
    }
  )
  it('create Endpoint 2', () => {
    cy.fixture('baseurl').then((data) => {
      cy.visit(data.baseurl)
    })

    cy.gotoAttributePage('Chatting Station (0x0601)', 'General')
  })
  it(
    'verify initial values on Endpoint 2 are reflected from Endpoint 1',
    { retries: { runMode: 2, openMode: 2 } },
    () => {
      // check ZCL version - 1
      cy.get(
        ':nth-child(1) > [style="min-width: 180px;"] > .q-field > .q-field__inner .q-field__control > .q-field__control-container > input'
      ).should('have.value', '1')

      // check power source - 22
      cy.get(
        ':nth-child(8) > [style="min-width: 180px;"] > .q-field > .q-field__inner .q-field__control > .q-field__control-container > input'
      ).should('have.value', '22')

      // check cluster revision - 33
      cy.get(
        ':nth-child(19) > [style="min-width: 180px;"] > .q-field > .q-field__inner .q-field__control > .q-field__control-container > input'
      ).should('have.value', '33')

      // check ResetToFactoryDefaults command - true/enabled
      cy.gotoCommandsTab()
      cy.get(':nth-child(3) > .q-mt-xs').should(
        'have.attr',
        'aria-checked',
        'true'
      )

      // Uncheck ResetToFactoryDefaults command
      cy.get(':nth-child(3) > .q-mt-xs')
        .click({
          force: true,
        })
        .should('have.attr', 'aria-checked', 'false')

      // Check for enabled "ZCL version"
      cy.gotoAttributeReportingTab()
      cy.get(
        '[data-cy="Attributes Reporting"] > .q-table__middle > .q-table > .q-virtual-scroll__content > :nth-child(1) > :nth-child(2) > .q-mt-xs > .q-toggle__inner'
      ).should('be.visible')
    }
  )

  it(
    'Check Command changes on Endpoint 2 is reflected on Endpoint 1',
    { retries: { runMode: 2, openMode: 2 } },
    () => {
      // Go back to main page
      cy.get('.router-link-active').click()

      cy.gotoAttributePage(false, 'General')

      cy.gotoCommandsTab()
      cy.get(':nth-child(3) > .q-mt-xs').should(
        'have.attr',
        'aria-checked',
        'false'
      )
    }
  )
})
