const scriptUtil = require('./script-util.js')
const path = require('path')

exports.default = async function (context) {
  if (
    context.electronPlatformName === 'win32' ||
    context.electronPlatformName === 'linux'
  ) {
    return scriptUtil.executeCmd({}, 'npx', [
      'copyfiles',
      '-V',
      '-f',
      path.resolve(context.outDir, '../apack.json'),
      context.appOutDir,
    ])
  }
}
