/// <reference types="cypress" />

const rendApi = require('../../../src-shared/rend-api.js')

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

  it(
    'Preference: set light theme',
    { retries: { runMode: 2, openMode: 2 } },
    () => {
      cy.get('#preference').click()
      cy.get('#darkTheme')
        .find('input')
        .uncheck({ force: true })

      cy.get('body').should('have.css', 'background-color', 'rgba(0, 0, 0, 0)')
    }
  )

  it(
    'Preference: set dark theme',
    { retries: { runMode: 2, openMode: 2 } },
    () => {
      cy.get('#preference').click()
      cy.get('#darkTheme')
        .find('input')
        .check({ force: true })

      cy.get('body').should('have.css', 'background-color', 'rgb(39, 40, 33)')
    }
  )

  it('RendererApi: set light theme', () => {
    cy.rendererApi(rendApi.id.setDarkTheme, 'false')
    cy.get('body').should('have.css', 'background-color', 'rgba(0, 0, 0, 0)')
  })

  it('RendererApi: set dark theme', () => {
    cy.rendererApi(rendApi.id.setDarkTheme, 'true')
    cy.get('body').should('have.css', 'background-color', 'rgb(39, 40, 33)')
  })
})
