pipeline {
  agent any
  environment {
        PATH="~/.platformio"
  }
  stages {
    stage('version') {
      steps {
        sh 'python --version'
      }
    }
  stage('Install Platformio') {
        steps {
            sh 'python -m pip install -U platformio'
        }
	}
	stage('Update Platformio') {
        steps {
            sh 'pio pkg update'
        }
	}
  stage('Compile and link + Flash') {
        steps {
            sh 'pio run -e uno'
        }
	}
  }
}
