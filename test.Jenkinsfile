pipeline { // Declarative pipeline�ł��邱�Ƃ�錾����
  agent any // ���̎w��iany�Ȃ̂Ŏw��Ȃ��j
  stages{
      stage("build"){
          steps{
            echo "Start build!"
          }
          //�X�e�b�v�I������
          post{
              //��Ɏ��s
              always{
                echo "========End build!========"
              }
              //������
              success{
                  echo "========Success!!========"
              }
              //���s��
              failure{
                echo "========Fail�c�c========"
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