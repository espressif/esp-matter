final boolean buildForMac = true
final boolean runCypressTests = false
final boolean triggerAdapterPackJob = false
final boolean sonarScan = false
final boolean validateXml = true

pipeline
{
    agent { node { label 'Build-Farm'
            customWorkspace '/mnt/raid/workspaces/zap'    } }

    options { buildDiscarder(logRotator(artifactNumToKeepStr: '10')) }

    stages
    {
        stage('Git setup')
        {
            steps
            {
                script
                {
                    checkout scm
                }
            }
        }
        stage('Npm clean install')
        {
            steps
            {
                script
                {
                    sh 'uname -a'
                    sh 'rm -rf dist/'
                    sh 'npm --version'
                    sh 'node --version'
                    sh 'npm ci'
                    sh 'npm list || true'
                }
            }
        }
        stage('Initial checks') {
            parallel {
                stage('Version stamp')        {
                    steps
                    {
                                script
                        {
                                    sh 'npm run version-stamp'
                                    sh 'npm run version'
                                    sh 'npm rebuild canvas --update-binary'
                                    sh 'npm rebuild libxmljs --update-binary'
                        }
                    }
                }
                stage('Generate Eclipse packaging metadata')        {
                    steps
                    {
                                script
                        {
                                    sh 'npm run package-metadata'
                        }
                    }
                }
                stage('XML validation')
                {
                    when { equals expected: true, actual: validateXml }
                    steps
                    {
                        script
                        {
                            sh 'npm run xml-validate'
                        }
                    }
                }
                stage('License check')
                {
                    steps
                    {
                        script
                        {
                            sh 'npm run lic'
                        }
                    }
                }
                stage('Outdated packages report')
                {
                    steps
                    {
                        script
                        {
                            sh 'npm outdated || true'
                        }
                    }
                }
                stage('Generate HTML documentation')
                {
                    steps
                    {
                        script
                        {
                            sh 'npm run doc'
                        }
                    }
                }
            }
        }
        stage('Build SPA layout for unit tests')
        {
            steps
            {
                script
                {
                    sh 'npm run build-spa'
                }
            }
        }
        stage('Build backend')
        {
            steps
            {
                script
                {
                    sh 'npm run build-backend'
                }
            }
        }
        stage('ESLint execution')
        {
            steps
            {
                script
                {
                    sh 'npm run lint'
                }
            }
        }
        stage('Unit test execution')
        {
            steps
            {
                script
                {
                    withEnv(['ZAP_TEST_TIMEOUT=3600000', 'ZAP_TEMPSTATE=1'])
                    {
                        sh 'npm run test:unit'
                    }
                }
            }
        }
        stage('Self check')
        {
            steps
            {
                script
                {
                    sh 'rm -rf ~/.zap'
                    sh 'npm run self-check'
                }
            }
        }
        stage('Test generation') {
            parallel {
                stage('Test Zigbee generation') {
                    steps {
                        script {
                            sh 'npm run gen'
                        }
                    }
                }
                stage('Test Matter generation') {
                    steps {
                        script {
                            sh 'npm run genmatter'
                        }
                    }
                }
                stage('Test Dotdot generation') {
                    steps {
                        script {
                            sh 'npm run gendotdot'
                        }
                    }
                }
            }
        }
        stage('Cypress UI tests')
        {
            when { equals expected: true, actual: runCypressTests }
            steps
            {
                script
                {
                    sh 'rm -rf ~/.zap'
                    sh 'xvfb-run -a npm run test:e2e-ci'
                }
            }
        }
        stage('Run Sonar Scan')
        {
            when
            {
                allOf { branch 'silabs'; equals expected: true, actual: sonarScan }
            }
            steps
            {
                script
                {
                    gitBranch = "${env.BRANCH_NAME}"
                    sh '/opt/sonar-scanner-cli/sonar-scanner-4.5.0.2216-linux/bin/sonar-scanner -Dsonar.host.url=https://sonarqube.silabs.net/ -Dsonar.login=e48b8a949e2869afa974414c56b4dc7baeb146e3 -X -Dsonar.branch.name=' + gitBranch
                }
            }
        }
        stage('Building node.js artifacts') {
            steps
            {
                script
                {
                    sh 'npm run pkg'
                    sh 'mv dist/zap-linux dist/zap-cli-linux'
                    sh 'mv dist/zap-macos dist/zap-cli-macos'
                    sh 'mv dist/zap-win.exe dist/zap-cli-win.exe'
                }
            }
        }
        stage('Building electron artifacts') {
            parallel {
                stage('Building for Mac')
                {
                    when { equals expected: true, actual: buildForMac }
                    agent { label 'bgbuild-mac' }
                    steps
                    {
                        script
                        {
                            withEnv(['PATH+LOCAL_BIN=/usr/local/bin',
                                     'PATH+NODE14=/usr/local/opt/node@14/bin',
                                     //'DEBUG=electron-builder',
                                     'NODE_TLS_REJECT_UNAUTHORIZED=0']) // workaround for a self-signed cert issue on canvas 2.7.0 on mac
                            {
                                withCredentials([usernamePassword(credentialsId: 'buildengineer',
                                                                  usernameVariable: 'SL_USERNAME',
                                                                  passwordVariable: 'SL_PASSWORD')])
                                {
                                    sh 'npm --version'
                                    sh 'node --version'
                                    sh 'npm config set strict-ssl false'
                                    sh 'npm ci'
                                    sh 'npm list || true'
                                    sh 'security unlock-keychain -u  "/Library/Keychains/System.keychain"'
                                    sh 'npm run version-stamp'
                                    sh 'npm run build'
                                    sh 'npm run pack:mac:native'
                                    stash includes: 'dist/*mac.zip', name: 'zap_apack_mac'
                                }
                            }
                        }
                    }
                }
                stage('Building for Windows / Linux')
                {
                    steps
                    {
                        script
                        {
                            sh 'echo "Building for Windows"'
                            sh 'xvfb-run -a npm run pack:win'
                            stash includes: 'dist/*win.zip', name: 'zap_apack_win'

                            sh 'echo "Building for Linux"'
                            sh 'xvfb-run -a npm run pack:linux'
                            stash includes: 'dist/*linux.zip', name: 'zap_apack_linux'
                        }
                    }
                }
            }
        }

        stage('Archiving artifacts')
        {
            parallel {
                stage('Archiving for Windows / Linux')
                {
                    steps
                    {
                        script
                        {
                            def file = sh(script: 'ls dist/*win.zip | head -n 1', returnStdout: true).trim()
                            sh 'cp ' + file + ' zap_apack_win.zip'

                            file = sh(script: 'ls dist/*linux.zip | head -n 1', returnStdout: true).trim()
                            sh 'cp ' + file + ' zap_apack_linux.zip'

                            archiveArtifacts artifacts:'dist/zap*', fingerprint: true
                        }
                    }
                }
                stage('Archiving for macOS')
                {
                    when { equals expected: true, actual: buildForMac }
                    agent { label 'bgbuild-mac' }
                    options { skipDefaultCheckout() }
                    steps
                    {
                        script
                        {
                            def file = sh(script: 'ls dist/*mac.zip | head -n 1', returnStdout: true).trim()
                            sh 'cp ' + file + ' zap_apack_mac.zip'
                            archiveArtifacts artifacts:'dist/zap*', fingerprint: true
                        }
                    }
                }
                stage('Eclipse packaging metadata')
                {
                    steps
                    {
                        script
                        {
                            archiveArtifacts artifacts:'docs/releasenotes.md', fingerprint: true
                            archiveArtifacts artifacts:'apack_zap.package', fingerprint: true
                        }
                    }
                }
            }
        }

        stage('Check artifacts') {
            parallel {
                stage('Checking Windows exe')
                {
                    agent { label 'windows10' }
                    options { skipDefaultCheckout() }
                    steps
                    {
                        cleanWs()
                        dir('test_apack_bin') {
                            script
                            {
                                unstash 'zap_apack_win'
                                sh 'ls -R'
                                def file = sh(script: 'ls dist/*win.zip | head -n 1', returnStdout: true).trim()
                                unzip zipFile: file
                                String response = bat(script: 'zap.exe --version', returnStdout: true).trim()
                                echo response
                                if ( response.indexOf('undefined') == -1) {
                                    response = bat (script: 'zap.exe selfCheck', returnStdout: true).trim()
                                    echo response
                                    if ( response.indexOf('Self-check done') == -1 ) {
                                        error 'Wrong self-check result.'
                                        currentBuild.result = 'FAILURE'
                                    } else {
                                        currentBuild.result = 'SUCCESS'
                                    }
                                } else {
                                    error 'Undefined version information'
                                    currentBuild.result = 'FAILURE'
                                }
                            }
                            deleteDir()
                        }
                    }
                }
                stage('Checking macOS exe')
                {
                    when { equals expected: true, actual: buildForMac }
                    agent { label 'bgbuild-mac' }
                    options { skipDefaultCheckout() }
                    steps
                    {
                        // WORKAROUND:
                        // Skip testing zap within .zap since zip/unzip within Jenkins is unable to
                        // maintain the framework symlinks, referenced by ZAP
                        script
                        {
                            // redirect stderr to file to avoid return statusCode failing the pipeline.
                            // mac binaries would err with "Trace/BPT trap: 5".
                            // depending on electron/electron-builder version, the error appear/disappear from time to time.
                            String status = sh(script: './dist/mac/zap.app/Contents/MacOS/zap --version 2&> output.txt', returnStatus: true)
                            def output = readFile('output.txt').trim()
                            echo output
                            if ( output.indexOf('undefined') == -1) {
                                status = sh (script: './dist/mac/zap.app/Contents/MacOS/zap selfCheck 2&> output.txt', returnStdout: true)
                                output = readFile('output.txt').trim()
                                echo output
                                if ( output.indexOf('Self-check done') == -1 ) {
                                    error 'Wrong self-check result.'
                                    currentBuild.result = 'FAILURE'
                                } else {
                                    currentBuild.result = 'SUCCESS'
                                }
                          } else {
                                error 'Undefined version information'
                                currentBuild.result = 'FAILURE'
                            }
                        }
                    }
                }
                stage('Checking Linux exe')
                {
                    steps
                    {
                        dir('test_apack_bin') {
                            script
                            {
                                cleanWs()
                                unstash 'zap_apack_linux'
                                sh 'ls -R'
                                def file = sh(script: 'ls dist/*linux.zip | head -n 1', returnStdout: true).trim()
                                unzip zipFile: file
                                sh 'chmod 755 zap'
                                String response = sh(script: 'xvfb-run -a ./zap --version', returnStdout: true).trim()
                                echo response
                                if ( response.indexOf('undefined') == -1) {
                                    response = sh (script: 'xvfb-run -a ./zap selfCheck', returnStdout: true).trim()
                                    echo response
                                    if ( response.indexOf('Self-check done') == -1 ) {
                                        error 'Wrong self-check result.'
                                        currentBuild.result = 'FAILURE'
                                    } else {
                                        currentBuild.result = 'SUCCESS'
                                    }
                                } else {
                                    error 'Undefined version information'
                                    currentBuild.result = 'FAILURE'
                                }
                            }
                            deleteDir()
                        }
                    }
                }
            }
        }

        stage('Build status resolution')
        {
            steps
            {
                script
                {
                    currentBuild.result = 'SUCCESS'
                }
            }
        }

        stage('Run Adapter_Pack_ZAP_64 on JNKAUS-16.silabs.com')
        {
            when
            {
                allOf { branch 'silabs'; equals expected: true, actual: triggerAdapterPackJob }
            }
            steps
            {
                script
                {
                    try {
                        triggerRemoteJob blockBuildUntilComplete: false,
                                        job: 'https://jnkaus016.silabs.com/job/Adapter_Pack_ZAP_64/',
                                        remoteJenkinsName: 'jnkaus016',
                                        shouldNotFailBuild: false,
                                        useCrumbCache: true,
                                        useJobInfoCache: true
                    } catch (err) {
                        currentBuild.result = 'UNSTABLE'
                    }
                }
            }
        }

        stage('Building download-artifact for Mac / Linux / Windows')
        {
            steps
            {
                script
                {
                    sh 'echo "Building..."'
                    sh 'npm run pkg:download-artifact'
                    archiveArtifacts artifacts:'dist/download-artifact*', fingerprint: true
                }
            }
        }

        stage('Workspace clean up')
        {
            parallel {
                stage('Mac') {
                    when { equals expected: true, actual: buildForMac }
                    agent { label 'bgbuild-mac' }
                    options { skipDefaultCheckout() }
                    steps { cleanWs() }
                }

                stage('Linux / Windows') {
                    agent { label 'Build-Farm' }
                    options { skipDefaultCheckout() }
                    steps { cleanWs() }
                }
            }
        }
    }
    post {
        always {
            script
            {
                def committers = emailextrecipients([[$class: 'CulpritsRecipientProvider'],
                                                    [$class: 'DevelopersRecipientProvider']])

                jobName = "${currentBuild.fullDisplayName}".replace('%2', '/')
                if (currentBuild.result == 'SUCCESS') {
                    slackMessage = ":zap_success: SUCCESS: <${env.RUN_DISPLAY_URL}|" + jobName + '>, changes by: ' + committers
                    slackColor = 'good'
                    slackSend (color: slackColor, channel: '#zap', message: slackMessage)
                }
                else if (currentBuild.result == 'UNSTABLE') {
                    slackMessage = ":zap_warning: WARNING: <${env.RUN_DISPLAY_URL}|" + jobName + '>, changes by: ' + committers
                    slackColor = 'warning'
                    slackSend (color: slackColor, channel: '#zap', message: slackMessage)
                }
                else
                {
                    slackMessage = ":zap_failure: FAILED: <${env.RUN_DISPLAY_URL}|" + jobName + '>, changes by: ' + committers
                    slackColor = 'danger'
                    slackSend (color: slackColor, channel: '#zap', message: slackMessage)
                }
            }
            junit 'junit.xml'
        }
    }
}
