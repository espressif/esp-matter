module.exports = {
  globals: {
    __DEV__: true,
  },
  globalSetup: '<rootDir>/test/global-setup.js',
  globalTeardown: '<rootDir>/test/global-teardown.js',
  setupFilesAfterEnv: ['<rootDir>/test/jest/jest.setup.js'],
  // noStackTrace: true,
  // bail: true,
  // cache: false,
  // verbose: true,
  // watch: true,
  collectCoverage: true,
  coverageDirectory: 'jest-coverage',
  collectCoverageFrom: [
    '<rootDir>/src-electron/**/*.js',
    '<rootDir>/src-shared/**/*.js',
    // Enable these once the UI testing is available
    '<rootDir>/src/**/*.vue',
    '<rootDir>/src/**/*.js',
    '<rootDir>/src/**/*.jsx',
  ],
  coverageThreshold: {
    global: {
      // Note to whoever is looking at these number:
      // We need to settle on much higher number, so we will slowly
      // be inching this upward, as we add more unit tests.
      // DO NOT EVER DECREASE THESE NUMBERS, PLEASE, UNLESS FOR A GOOD REASON.
      statements: 64,
      branches: 51,
      functions: 60,
      lines: 65,
    },
  },
  testMatch: ['<rootDir>/test/*.test.js', '<rootDir>/test/*.test.ts'],
  moduleFileExtensions: ['vue', 'js', 'jsx', 'json', 'ts', 'tsx'],
  moduleNameMapper: {
    '^vue$': '<rootDir>/node_modules/vue/dist/vue.common.js',
    '^test-utils$':
      '<rootDir>/node_modules/@vue/test-utils/dist/vue-test-utils.js',
    '^quasar$': '<rootDir>/node_modules/quasar/dist/quasar.common.js',
    '^~/(.*)$': '<rootDir>/$1',
    '^src/(.*)$': '<rootDir>/src/$1',
    '.*css$': '<rootDir>/test/jest/utils/stub.css',
    '.*\\.sql$': '<rootDir>/test/jest/__mocks__/zap-sql-mock.js',
    '\\.(jpg|jpeg|png|gif|eot|otf|webp|svg|ttf|woff|woff2|mp4|webm|wav|mp3|m4a|aac|oga)$':
      '<rootDir>/test/jest/__mocks__/file-mock.js',
  },
  transform: {
    '.*\\.vue$': 'vue-jest',
    '.*\\.[jt]sx?$': 'babel-jest',
    '.+\\.(css|styl|less|sass|scss|svg|png|jpg|ttf|woff|woff2)$':
      'jest-transform-stub',
    // use these if NPM is being flaky
    // '.*\\.vue$': '<rootDir>/node_modules/@quasar/quasar-app-extension-testing-unit-jest/node_modules/vue-jest',
    // '.*\\.js$': '<rootDir>/node_modules/@quasar/quasar-app-extension-testing-unit-jest/node_modules/babel-jest'
  },
  transformIgnorePatterns: ['<rootDir>/node_modules/(?!quasar/lang)'],
  snapshotSerializers: ['<rootDir>/node_modules/jest-serializer-vue'],
  testResultsProcessor: 'jest-sonar-reporter',
  testPathIgnorePatterns: [
    '/node_modules/',
    '<rootDir>/test/download-artifact.test.js',
  ],
}
