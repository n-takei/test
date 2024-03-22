pipeline { // Declarative pipelineであることを宣言する
  agent any // 環境の指定（anyなので指定なし）
  stages{
      stage("build"){
          steps{
            echo "Start build!"
          }
          //ステップ終了処理
          post{
              //常に実行
              always{
                echo "========End build!========"
              }
              //成功時
              success{
                  echo "========Success!!========"
              }
              //失敗時
              failure{
                echo "========Fail……========"
              }
          }
      }
      stage("check"){
        steps{
          echo 'checking'
        }
      }
    }
  post{
    always{
      echo "========Finish========"
    }
  }
}