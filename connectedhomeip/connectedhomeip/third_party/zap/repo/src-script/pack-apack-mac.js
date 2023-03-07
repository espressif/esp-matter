const fs = require('fs')
const path = require('path')
const Seven = require('node-7z')
const sevenBin = require('7zip-bin')
const pathTo7zip = sevenBin.path7za

exports.default = async function (buildResult) {
  buildResult?.artifactPaths?.forEach((element) => {
    if (
      (element.includes('mac') ||
        element.includes('win') ||
        element.includes('linux')) &&
      element.endsWith('.zip')
    ) {
      const myStream = Seven.add(
        element,
        path.join(buildResult.outDir, '../apack.json'),
        {
          $progress: true,
          $bin: pathTo7zip,
        }
      )
      // myStream.on('end', function () {
      //   console.log(myStream.info)
      // })

      myStream.on('error', function (err) {
        if (err) {
          console.log(err.stderr)
          throw err
        }
      })
    }
  })
}
